//
// Created by Administrator on 2019/8/6.
//
#include <curl/curl.h>
#include "OBOJni.h"
#include "cJSON.h"

#define RESPONSE_DATA_LEN 4096

typedef struct login_response_data{
    login_response_data(){
        memset(data,0,RESPONSE_DATA_LEN);
    }
    char data[RESPONSE_DATA_LEN];
    int data_len;
}response_data_t;

size_t deal_response(void *ptr,size_t n,size_t m,void *arg){
    int count=m*n;
    response_data_t *response_data=(response_data_t*)arg;
    memcpy(response_data->data,ptr,count);
    response_data->data_len=count;
    return response_data->data_len;
}
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    login
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_myapplication_OBOJNI_login
        (JNIEnv *env, jobject j_obj, jstring j_username, jstring j_pwd, jboolean j_bool){
    const char *username=env->GetStringUTFChars(j_username,0);
    const char *passwd=env->GetStringUTFChars(j_pwd,0);
    const char *isDriver=j_bool==JNI_TRUE?"yes":"no";

    char *post_str=NULL;
    CURL *curl=NULL;
    CURLcode res;
    response_data_t response_data;
    //初始化curl句柄
    curl=curl_easy_init();
    if(curl==NULL){
        __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:curl init error\n");
        return JNI_FALSE;
    }

    __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:username=%s,passwd=%s,isDriver=%s",
            username,passwd,isDriver);

    //封装一个数据协议
    /*
       ====给服务端的协议====
     http://ip:port/login [json_data]
    {
        username: "gailun",
        password: "123123",
        driver:   "yes"
    }*/
    //（1）封装一个json字符串
    cJSON* root=cJSON_CreateObject();

    cJSON_AddStringToObject(root,"username",username);
    cJSON_AddStringToObject(root,"password",passwd);
    cJSON_AddStringToObject(root,"driver",isDriver);

    post_str=cJSON_Print(root);
    cJSON_Delete(root);
    root=NULL;
    __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:post_str=[%s]\n",post_str);
    //(2) 向web服务器 发送http请求 其中post数据 json字符串
    //1 设置curl url
    curl_easy_setopt(curl,CURLOPT_URL,"http://101.200.190.150:7777/login");
    //客户端忽略CA证书认证
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,false);
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,false);
    //开启post请求开关
    curl_easy_setopt(curl,CURLOPT_POST,true);
    //添加post数据
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_str);
    //设定一个处理服务器响应的回调函数
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,deal_response);
    //给回调函数传递一个形参
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response_data);
    //向服务器发送请求，等待服务器的响应
    res=curl_easy_perform(curl);
    if(res!=CURLE_OK){
        __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-Login:perform ERROr,rescode=[%d]\n",res);
        return JNI_FALSE;
    }
    //（3）  处理服务器响应的数据 此刻的responseData就是从服务器获取的数据
    /* //成功
    {
        result: "ok",
    }
    //失败
    {
        result: "error",
        reason: "why...."
    }*/
    //(4) 解析服务器返回的json字符串
    root=cJSON_Parse(response_data.data);
    cJSON *result=cJSON_GetObjectItem(root,"result");
    if(result&&strcmp(result->valuestring,"ok")==0){
        __android_log_print(ANDROID_LOG_ERROR,TAG,"JIN_login:loing succ");
        return JNI_TRUE;
    }else{
        cJSON *resaon=cJSON_GetObjectItem(root,"reason");
        if(resaon){
            __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:loginerror reason=%s",resaon->valuestring);
        }else{
            //未知的错误
            __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login error, reason = Unknow！！！");
        }
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif
