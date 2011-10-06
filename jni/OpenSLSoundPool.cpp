/*
 * MySoundPool.cpp
 *
 *  Created on: 30.9.2011
 *      Author: Janne Kytomaki
 */

#include "OpenSLSoundPool.h"
#include <dlfcn.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include "opensl_helpers.h"
OpenSLSoundPool* OpenSLSoundPool::instance;

BufferQueue::~BufferQueue(){
	LOGI("Deleting BufferQueue");
	(**player)->Destroy(*player);
	player = NULL;
	playerPlay = NULL;
	queue = NULL;
	volume = NULL;
}

ResourceBuffer::~ResourceBuffer(){
	LOGI("Deleting ResourceBuffer");
	free(buf);
}

OpenSLSoundPool::OpenSLSoundPool(int maxStreams, SLuint32 samplingRate, SLuint32 bitrate):
		engineObject(NULL),
		outputMixObject(NULL),
		maxStreams(maxStreams),
		samplingRate(samplingRate),
		bitrate(bitrate){

	LOGI("MySoundPool");

	if (OpenSLSoundPool::instance != NULL){
		LOGI("Can only be instantiated once");
		exit(EXIT_FAILURE);
	}
	OpenSLSoundPool::instance = this;

	// see if OpenSL library is available
	void* handle = dlopen("libOpenSLES.so", RTLD_LAZY);
	if (handle == NULL){
		LOGI("OpenSLES not available");
		exit(EXIT_FAILURE);
	}

	bufferQueues = new std::vector<BufferQueue*>();

	samples = new std::vector<ResourceBuffer*>();

	LOGI("OpenSLES available");
	LOGI("Initializing");
	createEngine();
	createBufferQueueAudioPlayer();

	minVolume = SL_MILLIBEL_MIN;
	maxVolume = 0;
}

int OpenSLSoundPool::load(char* buffer, int length){
	ResourceBuffer* b = new ResourceBuffer();
	b->buf = buffer;
	b->size = length;
	samples->push_back(b);
	return samples->size();
}

void OpenSLSoundPool::createEngine(){

    SLresult result;

    const SLInterfaceID engine_ids[] = {SL_IID_ENGINE};
	const SLboolean engine_req[] = {SL_BOOLEAN_TRUE};

    result = slCreateEngine(&engineObject, 0, NULL, 0, engine_ids, engine_req);
    check(result);

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    check(result);

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    check(result);

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_NULL};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};

    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    check(result);

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    check(result);

}

// a static callback function called every time a buffer finishes playing,
// delegates to the instance function
void OpenSLSoundPool::staticBqPlayerCallback(SLBufferQueueItf bq, void *context){
	LOGI("staticBqPlayerCallback");
	// we might be already destroyed if release() has been called mid-playback
	OpenSLSoundPool* tmpInstance;
	if ( (tmpInstance = instance) != NULL){
		tmpInstance->bqPlayerCallback(bq, context);
	}
}

// this callback handler is called every time a buffer finishes playing
void OpenSLSoundPool::bqPlayerCallback(SLBufferQueueItf bq, void *context)
{
	LOGI("bqPlayerCallback");
	BufferQueue* avail = NULL;

	for (int i = 0; i < bufferQueues->size(); i++){
		if (*bufferQueues->at(i)->queue == bq){
			avail = bufferQueues->at(i);
			avail->playing = false;
			break;
		}
	}

}

// create buffer queue audio player
void OpenSLSoundPool::createBufferQueueAudioPlayer(){

	LOGI("createBufferQueueAudioPlayer");

    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, samplingRate,
        bitrate, bitrate,
        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // create audio player
    const SLInterfaceID player_ids[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
	const SLboolean player_req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

	LOGI("Creating %i streams", maxStreams);
    for (int i = 0; i < maxStreams; i++){

    	BufferQueue* bf = new BufferQueue();
		bf->playing = false;
		bf->queue = new SLBufferQueueItf();
		bf->player = new SLObjectItf();
		bf->playerPlay = new SLPlayItf();
		bf->volume = new SLVolumeItf();

		// configure audio sink
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		//SLVolumeItf volume;

    	LOGI("Creating SLAndroidSimpleBufferQueueItf");

    	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &*bf->player, &audioSrc, &audioSnk,
    	            3, player_ids, player_req);
    	check(result);

		// realize the player
		result = (**bf->player)->Realize(*bf->player, SL_BOOLEAN_FALSE);
		check(result);

		// get the play interface
		result = (**bf->player)->GetInterface(*bf->player, SL_IID_PLAY, &*bf->playerPlay);
		check(result);

		// get the buffer queue interface
		result = (**bf->player)->GetInterface(*bf->player, SL_IID_BUFFERQUEUE, &*bf->queue);
		check(result);

		// register callback on the buffer queue
		result = (**bf->queue)->RegisterCallback(*bf->queue, OpenSLSoundPool::staticBqPlayerCallback, NULL);
		check(result);

		result = (**bf->player)->GetInterface(*bf->player, SL_IID_VOLUME, &*bf->volume);
		check(result);

		if (i == 0){
			result = (**bf->volume)->GetMaxVolumeLevel(*bf->volume, &maxVolume);
			check(result);
		}

		result = (**bf->playerPlay)->SetPlayState(*bf->playerPlay, SL_PLAYSTATE_PLAYING);
		check(result);

	    bufferQueues->push_back(bf);
	    LOGI("Created stream %i", i);
    }
    LOGI("Created %i streams", bufferQueues->size());

    LOGI("createBufferQueueAudioPlayer done");
}

__attribute__((constructor)) static void onDlOpen(void)
{
	LOGI("onDlOpen");
}


OpenSLSoundPool::~OpenSLSoundPool() {
	instance = NULL;

	// destroy all buffer queues
	for (int i = 0; i < bufferQueues->size(); i++){
		delete bufferQueues->at(i);
	}
	delete bufferQueues;

	// destroy all samples
	for (int i = 0; i < samples->size(); i++){
		delete samples->at(i);
	}
	delete samples;

	// destroy output mix object
	if (outputMixObject != NULL) {
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (engineObject != NULL) {
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}

}

int OpenSLSoundPool::play(int sampleId, float volume){
	int streamId = 0;
	LOGI("Play sample %i at volume %f", sampleId, volume);

	// find first available buffer queue
	BufferQueue* avail = NULL;
	int i = 0;
	for (; i < bufferQueues->size(); i++){
		if (!bufferQueues->at(i)->playing){
			avail = bufferQueues->at(i);
			avail->playing = true;
			streamId = i + 1;
			break;
		}
	}

	if (avail != NULL){
		LOGI("Playing on channel %i", streamId);

		// find the requested sample
		if (sampleId > samples->size() + 1 || samples->at(sampleId - 1) == NULL){
			LOGI("No sample found");
			return 0;
		}

		ResourceBuffer* buf = samples->at(sampleId - 1);

		SLresult result;

		// convert requested volume 0.0-1.0 to millibels
		// in range max(minVolume, MIN_VOL_MILLIBEL) - maxVolume
		SLmillibel minvol = minVolume;
		if (minvol < MIN_VOL_MILLIBEL){
			minvol = MIN_VOL_MILLIBEL;
		}

		SLmillibel newVolume = ((minvol - maxVolume) * (1.0 - volume)) + maxVolume;

		//adjust volume for the buffer queue
		result = (**avail->volume)->SetVolumeLevel(*avail->volume, newVolume);
		check(result);

		//enqueue the sample
		result = (**avail->queue)->Enqueue(*avail->queue, (void*)(short*) buf->buf, buf->size);
		if (SL_RESULT_SUCCESS != result) {
			LOGI("Enqueue result not OK");
		}

	}else{
		LOGI("No channels available for playback");

	}
	return streamId;
}

