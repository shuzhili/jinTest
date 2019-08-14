//
// Created by Administrator on 2019/8/13.
//
#include "OBOJni.h"
#include "curl/curl.h"

extern "C"{

/*
* Class:     com_example_myapplication_OBOJNI
* Method:    testLibcurl
* Signature: ()V
*/

size_t print_html(void *ptr,size_t size,size_t nmemb,void *stream){
    int len=size*nmemb;
    char *buf=new char[len+1];
    memcpy(buf,ptr,len);
    buf[len]='\0';
    JNIINFO("TestLIBCURL=%s\n",buf);

    delete []buf;
    return len;
}

JNIEXPORT void JNICALL Java_com_example_myapplication_OBOJNI_testLibcurl
(JNIEnv *env, jobject obj){
    CURL *curl=NULL;
    CURLcode ret;
    curl=curl_easy_init();
    JNIINFO("TestLIBCUTL=%d",100);
    curl_easy_setopt(curl,CURLOPT_URL,"www.baidu.com");
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,print_html);
    ret=curl_easy_perform(curl);
    if(ret!=CURLE_OK){
        JNIINFO("curl_easy_perform error %s","");
        return;
    }
    curl_easy_cleanup(curl);
}

}
