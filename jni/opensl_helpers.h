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
 
 * opensl_helpers.h
 *
 *  Created on: 30.9.2011
 *      Author: Janne Kytomaki
 */

#ifndef OPENSL_HELPERS_H_
#define OPENSL_HELPERS_H_

#include "logging.h"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

static const char *result_strings[] = {
    "SUCCESS",
    "PRECONDITIONS_VIOLATED",
    "PARAMETER_INVALID",
    "MEMORY_FAILURE",
    "RESOURCE_ERROR",
    "RESOURCE_LOST",
    "IO_ERROR",
    "BUFFER_INSUFFICIENT",
    "CONTENT_CORRUPTED",
    "CONTENT_UNSUPPORTED",
    "CONTENT_NOT_FOUND",
    "PERMISSION_DENIED",
    "FEATURE_UNSUPPORTED",
    "INTERNAL_ERROR",
    "UNKNOWN_ERROR",
    "OPERATION_ABORTED",
    "CONTROL_LOST"
};

// Convert result to string; FIXME should move to common test library

static const char *result_to_string(SLresult result)
{
    static char buffer[32];
    if ( /* result >= 0 && */ result < sizeof(result_strings) / sizeof(result_strings[0]))
        return result_strings[result];
    LOGI(buffer, "%d", (int) result);
    return buffer;
}

// Compare result against expected and exit suddenly if wrong

void check2(SLresult result, int line)
{
    if (SL_RESULT_SUCCESS != result) {
    	LOGI("error %s at line %d\n", result_to_string(result), line);
        exit(EXIT_FAILURE);
    }
}

// Same as above but automatically adds the source code line number

#define check(result) check2(result, __LINE__)


// FIXME: GCC compiles OK, but Eclipse CDT displays errors for OpenSL slCreateEngine references.
// Redefining the function here seems to remove the error. Try commenting the declaration out
// after few CDT updates.

SLresult slCreateEngine(
	SLObjectItf             *pEngine,
	SLuint32                numOptions,
	const SLEngineOption    *pEngineOptions,
	SLuint32                numInterfaces,
	const SLInterfaceID     *pInterfaceIds,
	const SLboolean         * pInterfaceRequired
);

#endif /* OPENSL_HELPERS_H_ */
