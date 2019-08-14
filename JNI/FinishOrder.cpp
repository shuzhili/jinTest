//
// Created by Administrator on 2019/8/13.
//
#include "OBOJni.h"
#include "Json.h"
#include "Curl.h"
#include "Data.h"

extern "C"{
/*
* Class:     com_example_myapplication_OBOJNI
* Method:    FinishOrder
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_com_example_myapplication_OBOJNI_FinishOrder
        (JNIEnv *env, jobject obj){
    Json json;
    json.insert("sessionid",Data::getInstance()->getSessionid().c_str());
    json.insert("orderid",Data::getInstance()->getOrderid().c_str());

    string json_str=json.print();

    string url=OBO_SERVER_IP;
    url+=":";
    url+=OBO_SERVER_PORT;
    url+="/finishOrder";

    Curl curl(url,true);

    if(curl.execute(json_str)== false){
        JNIINFO("%s","curl execute error");
        return JNI_FALSE;
    }

    string response_data=curl.responseData();
    Json json_response;
    json_response.parse(curl.responseData());

    string result=json_response.value("result");
    if(result.length()!=0){
        if(result=="ok"){
            Data::getInstance()->setOrderid("NONE");
            return JNI_TRUE;
        }else{
            JNIINFO("ret error data=%s",response_data.c_str());
            return JNI_FALSE;
        }
    }
    return JNI_TRUE;
}

}