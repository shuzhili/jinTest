//
// Created by Administrator on 2019/8/12.
//
#include "Data.h"
#include "OBOJni.h"

Data *Data::instance=NULL;

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getOrderid
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getOrderid
        (JNIEnv *env, jobject obj){
    jstring  retstr=env->NewStringUTF(Data::getInstance()->getOrderid().c_str());
    return retstr;
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getSessionid
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getSessionid
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->getSessionid().c_str());
    return retstr;
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    setStatus
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_example_myapplication_OBOJNI_setStatus
        (JNIEnv *env, jobject obj, jstring jstatus){
    const char *status=env->GetStringUTFChars(jstatus,NULL);
    Data::getInstance()->setStatus(status);
    env->ReleaseStringUTFChars(jstatus,status);
}
/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getStatus
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getStatus
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->getStatus().c_str());
    return retstr;
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getIsDriver
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getIsDriver
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->getIsDriver().c_str());
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getPtempLongitude
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getPtempLongitude
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->get_ptemp_longitude().c_str());
    return retstr;
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getPtempLatitude
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getPtempLatitude
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->get_dtemp_latitude().c_str());
    return retstr;
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getDtempLongitude
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getDtempLongitude
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->get_ptemp_longitude().c_str());
    return retstr;
}
/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    getDtempLatitude
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_OBOJNI_getDtempLatitude
        (JNIEnv *env, jobject obj){
    jstring retstr=env->NewStringUTF(Data::getInstance()->get_dtemp_latitude().c_str());
}


#ifdef __cplusplus
}
#endif
