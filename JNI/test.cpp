#include <jni.h>
#include <android/log.h>
#include <string.h>

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

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    getLine
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_HelloJni_getLine
        (JNIEnv *env, jobject obj, jstring str){
    const char *cstr;
    cstr=env->GetStringUTFChars(str,NULL);
    if(str==NULL){
        return NULL;
    }
    __android_log_print(ANDROID_LOG_ERROR,"jnitag","测试string传参：%s",cstr);

    env->ReleaseStringUTFChars(str,cstr);
    jstring retstr=env->NewStringUTF("return success");

    return retstr;
}

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    testBooleanInt
 * Signature: (IZ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_myapplication_HelloJni_testBooleanInt
        (JNIEnv *env, jobject obj, jint num, jboolean b){
    int cint=num;
    __android_log_print(ANDROID_LOG_ERROR,"jnitag","int=%d",cint);

    __android_log_print(ANDROID_LOG_ERROR,"jnitag","bool=%s",(b==JNI_TRUE)?"true":"false");

    return env->NewStringUTF("sdfdf");
}

JNIEXPORT jobjectArray JNICALL Java_com_example_myapplication_HelloJni_testArray
        (JNIEnv *env, jobject obj, jintArray java_i_array, jobjectArray java_str_array, jbooleanArray java_boo_array){
    jboolean *pba=(env)->GetBooleanArrayElements(java_boo_array,0);
    //booleanArray
    jsize len=(env)->GetArrayLength(java_boo_array);
    int i=0;
    for(i=0;i<len;i++){
        __android_log_print(ANDROID_LOG_ERROR, "jintag", "bArrat[%d]=%s", i,
                            (pba[i] == JNI_TRUE) ? "true" : "false");
    }
    env->ReleaseBooleanArrayElements(java_boo_array,pba,0);

    //intArray
    jint *pia=env->GetIntArrayElements(java_i_array,0);
    len=env->GetArrayLength(java_i_array);
    for(i=0;i<len;i++){
        __android_log_print(ANDROID_LOG_ERROR,"jnitag","iArray[%d]=%d",i,pia[i]);
    }
    env->ReleaseIntArrayElements(java_i_array,pia,0);

    //处理StringArray
    len=env->GetArrayLength(java_str_array);
    for(i=0;i<len;i++){
        //string不是基本类型，是一个实例对象，so use GetObjectArrayElement
        jstring jstrObj=(jstring)(env)->GetObjectArrayElement(java_str_array,i);
        __android_log_print(ANDROID_LOG_ERROR,"jnitag","strArray[%d]=%s",i,env->GetStringUTFChars(jstrObj,NULL));
    }

    jstring str;
    jobjectArray strArray;
    jsize arrayLen=5;

    const char *sa[]={"hello","jni","is","so","easy"};
    //string不是一个基本数据类型，是一个复杂的类型，所以需要用newObjectArray，
    //以下一句代码，表示该Object是一个String类型的数组
    strArray=env->NewObjectArray(arrayLen,env->FindClass("java/lang/String"),0);

    for(i=0;i<5;i++){
        str=env->NewStringUTF(sa[i]);
        env->SetObjectArrayElement(strArray,i,str);
    }
    return strArray;
}

/*
 * Class:     com_example_myapplication_HelloJni
 * Method:    nativeGetStudentInfo
 * Signature: ()Lcom/example/myapplication/Student;
 */
//JNIEXPORT jobject JNICALL Java_com_example_myapplication_HelloJni_nativeGetStudentInfo
//        (JNIEnv *env, jobject obj){
//    jclass studcls=env->FindClass("Lcom/example/myapplication/Student");
//    jmethodID constrocMID=env->GetMethodID(studcls,"<init>","(ILjava/long/String;)V");
//    jstring str=env->NewStringUTF("come from Native");
//    jobject stu_0bj=env->NewObject(studcls,constrocMID,11,str);
//    return stu_0bj;
//}
#ifdef __cplusplus
}
#endif
