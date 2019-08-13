//
// Created by Administrator on 2019/8/6.
//

#ifndef MY_PRO_OBOJNI_H
#define MY_PRO_OBOJNI_H

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <stdlib.h>

#define TAG     "JNI"
using namespace std;

#define jniLogTag           "JNI"
#define OBO_SERVER_IP       "https://101.200.190.150"
#define OBO_SERVER_PORT     "18888"

#define RESPONSE_DATA_LEN   (4096)
#define TIME_STR_LEN        (64)

#define JNIINFO(fmt, ...) \
    __android_log_print(ANDROID_LOG_INFO, jniLogTag, fmt, __VA_ARGS__);

#endif //MY_PRO_OBOJNI_H
