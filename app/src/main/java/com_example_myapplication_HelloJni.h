/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_example_myapplication_HelloJni */

#ifndef _Included_com_example_myapplication_HelloJni
#define _Included_com_example_myapplication_HelloJni
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    helloJniJava
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_myapplication_HelloJni_helloJniJava
  (JNIEnv *, jobject);

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    getLine
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_HelloJni_getLine
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    testBooleanInt
 * Signature: (IZ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_HelloJni_testBooleanInt
  (JNIEnv *, jobject, jint, jboolean);

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    testArray
 * Signature: ([I[Ljava/lang/String;[Z)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_example_myapplication_HelloJni_testArray
  (JNIEnv *, jobject, jintArray, jobjectArray, jbooleanArray);

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    nativeGetStudentInfo
 * Signature: ()Lcom/example/myapplication/Student;
 */
JNIEXPORT jobject JNICALL Java_com_example_myapplication_HelloJni_nativeGetStudentInfo
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
