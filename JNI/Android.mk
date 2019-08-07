LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

#libcurl.a
LOCAL_MODULE :=libcurl
LOCAL_SRC_FILES :=libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

#libmyJni.so
LOCAL_MODULE :=testjni
LOCAL_SRC_FILES :=test.cpp login.cpp cJSON.cpp
LOCAL_STATIC_LIBRARIES :=libcurl
LOCAL_LDLIBS := -llog -lz

include $(BUILD_SHARED_LIBRARY)