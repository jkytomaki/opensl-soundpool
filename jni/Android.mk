LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := opensl-soundpool
LOCAL_SRC_FILES := jni-soundpool-wrapper.cpp OpenSLSoundPool.cpp 
# for native audio
LOCAL_LDLIBS    += -lOpenSLES
# for logging
LOCAL_LDLIBS    += -llog

include $(BUILD_SHARED_LIBRARY)
