LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

#libmyJni.so
LOCAL_MODULE := myJni
LOCAL_SRC_FILES := test.cpp
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)