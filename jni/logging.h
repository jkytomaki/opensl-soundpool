/*
 * logging.h
 *
 *  Created on: 30.9.2011
 *      Author: Janne Kytomaki
 */

#ifndef SP_LOGGING_H_
#define SP_LOGGING_H_

#include <android/log.h>

#define LOG_TAG "OpenSLSoundPool"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif /* SP_LOGGING_H_ */
