/*
 * MySoundPool.h
 *
 *  Created on: 30.9.2011
 *      Author: Janne Kytomaki
 */

#ifndef MYSOUNDPOOL_H_
#define MYSOUNDPOOL_H_

#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include "logging.h"
#include "vector"
#include "stdlib.h"

#define NDKSP_MIN_VOLUME 0.0f
#define NDKSP_MAX_VOLUME 1.0f
#define MIN_VOL_MILLIBEL -500

void createEngine();
void createBufferQueueAudioPlayer();
void shutdown();

class ResourceBuffer {
public:
	virtual ~ResourceBuffer();
	char* buf;
	int size;
};

class BufferQueue{
	public:
	virtual ~BufferQueue();
	SLBufferQueueItf* queue;
	SLObjectItf* player;
	SLPlayItf* playerPlay;
	SLVolumeItf* volume;
	bool playing;
};

class OpenSLSoundPool {
public:
	OpenSLSoundPool(int maxStreams, SLuint32 samplingRate, SLuint32 bitrate);
	virtual ~OpenSLSoundPool();

	virtual int play(int sampleId, float volume);
	virtual int load(char* buffer, int length);

	static OpenSLSoundPool* instance;


private:
	virtual void createEngine();
	static void staticBqPlayerCallback(SLBufferQueueItf bq, void *context);
	virtual void bqPlayerCallback(SLBufferQueueItf bq, void *context);

	virtual void createBufferQueueAudioPlayer();

	int maxStreams;
	SLuint32 samplingRate;
	SLuint32 bitrate;

	// device specific min and max volumes
	SLmillibel minVolume;
	SLmillibel maxVolume;

	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;

	// output mix interfaces
	SLObjectItf outputMixObject;

	//BufferQueue queues[];

	// vector for BufferQueues (one for each channel)
	std::vector<BufferQueue*>* bufferQueues;

	// vector for samples
	std::vector<ResourceBuffer*>* samples;

};

#endif /* MYSOUNDPOOL_H_ */
