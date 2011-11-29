/*
   Copyright 2011, Janne Kytömäki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 
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
