/*
 * jni-soundpool-wrapper.cpp
 *
 *  Created on: 30.9.2011
 *      Author: Janne Kytomaki
 */

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include "logging.h"
#include "OpenSLSoundPool.h"
#include <unistd.h>
#include <iostream>
#include "ios"
#include <fstream>
#include <stdlib.h>

OpenSLSoundPool* pool;

extern "C" {

	void Java_com_kytomaki_openslsoundpool_OpenSLSoundPool_createEngine(JNIEnv* env, jclass clazz, jint maxStreams){
		LOGI("createEngine");
		pool = new OpenSLSoundPool(maxStreams, SL_SAMPLINGRATE_44_1, SL_PCMSAMPLEFORMAT_FIXED_16);
		LOGI("createEngine done");
	}

	void Java_com_kytomaki_openslsoundpool_OpenSLSoundPool_nativeRelease(JNIEnv* env, jclass clazz){
		LOGI("release OpenSLSoundPool");
		delete pool;
		LOGI("release OpenSLSoundPool done");
	}

	int Java_com_kytomaki_openslsoundpool_OpenSLSoundPool_nativePlay(JNIEnv * env, jclass clazz, jint soundId, jfloat volume){
		return pool->play(soundId, volume);
	}

	int Java_com_kytomaki_openslsoundpool_OpenSLSoundPool_load(JNIEnv * env, jclass clazz, jobject fd_sys, jint off, jint len){
		LOGI("load(), off: %i, len: %i", off, len);
		int soundId = 0;

		jclass fdClass = env->FindClass("java/io/FileDescriptor");
		if (fdClass != NULL){
			jfieldID fdClassDescriptorFieldID = env->GetFieldID(fdClass, "descriptor", "I");
			if (fdClassDescriptorFieldID != NULL && fd_sys != NULL){
				jint fd = env->GetIntField(fd_sys, fdClassDescriptorFieldID);
				LOGI("Duplicating file descriptor");
				int fileDescriptor = dup(fd);

				LOGI("Opening file");
				FILE* file = fdopen(fileDescriptor, "rb");
				if (file == NULL){
					LOGI("File is null!");
					exit(EXIT_FAILURE);
				}

				LOGI("Seeking");
				int fsr = fseek(file, off, SEEK_SET);
				if (fsr!=0){
					LOGI("Error in seek");
					exit(EXIT_FAILURE);
				}

				LOGI("Reading");
				char* buf;

				int indx = 0;
				int headersLength = 0;
				char* dataHdr = "data";
				int dataHdrLength = 4;
				int dataHdrIndx = 0;
				bool readingRaw = false;
				int countdown = 0;

				for (int i = 0; i < len; i++){
					char c = getc(file);
					if (readingRaw){
						if (countdown == 0){
							buf[indx] = c;
							indx++;
						}else{
							countdown--;
						}
					}else{
						headersLength++;
						if (dataHdr[dataHdrIndx] == c){
							if (dataHdrIndx == dataHdrLength - 1){
								readingRaw = true;
								buf = (char*) malloc (len - headersLength - 4);
								countdown = 4;
							}else{
								dataHdrIndx++;
							}
						}else{
							dataHdrIndx = 0;
						}
					}
				}
				LOGI("Load successful");
				return pool->load(buf, (int)len - headersLength - 4);
			}
		}

		return 0;
	}

}
