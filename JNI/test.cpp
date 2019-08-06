#include <jni.h>
#include <android/log.h>

void hello_jni(void)
{
    __android_log_print(ANDROID_LOG_ERROR,"jnitag","hello jni is called!!!!");
}

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    helloJniJava
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_myapplication_HelloJni_helloJniJava
        (JNIEnv *, jobject){
    hello_jni();
}

#ifdef __cplusplus
}
#endif
