//
// Created by Administrator on 2019/8/13.
//
#include "OBOJni.h"
#include "Json.h"
#include "Data.h"
#include "Curl.h"

extern "C"{
/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    DriverLocationChanged
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
//String longitude, String latitude,String address,String autoSend
JNIEXPORT jboolean JNICALL Java_com_example_myapplication_OBOJNI_DriverLocationChanged
        (JNIEnv *env, jobject obj, jstring j_longitude, jstring j_latitude,
                jstring j_address, jstring j_autoSend){

    const char *longitude=env->GetStringUTFChars(j_longitude,NULL);
    const char *latitude=env->GetStringUTFChars(j_latitude,NULL);
    const char *address=env->GetStringUTFChars(j_address,NULL);
    const char *autoSend=env->GetStringUTFChars(j_autoSend,NULL);

    Json json;
    json.insert("sessionid",Data::getInstance()->getSessionid().c_str());
    json.insert("driver","yes");
    json.insert("status",Data::getInstance()->getStatus());
    json.insert("longitude",longitude);
    json.insert("latitude",latitude);
    json.insert("address",address);
    json.insert("autosend",autoSend);

    string json_str=json.print();
    //释放资源
    env->ReleaseStringUTFChars(j_longitude,longitude);
    env->ReleaseStringUTFChars(j_latitude,latitude);
    env->ReleaseStringUTFChars(j_address,address);
    env->ReleaseStringUTFChars(j_autoSend,autoSend);

    string url = OBO_SERVER_IP;
    url +=":";
    url +=OBO_SERVER_PORT;
    url +="/locationChanged";

    Curl curl(url, true);

    if(curl.execute(json_str)==false){
        JNIINFO("%s","curl execute error");
        return JNI_FALSE;
    }

    string response_data=curl.responseData();
    Json json_response;
    json_response.parse(curl.responseData());
    string result = json_response.value("result");
    if (result.length() != 0) {
        if (result == "ok") {

            string status = json_response.value("status");
            Data::getInstance()->setStatus(status);

            string orderid = json_response.value("orderid");
            Data::getInstance()->setOrderid(orderid);

            if ((Data::getInstance()->getStatus() == OB0_STATUS_DRIVER_CATCH)
                ||  (Data::getInstance()->getStatus() == OBO_STATUS_DRIVER_DRIVE) ){
                //得到乘客的当前坐标
                Data::getInstance()->set_ptemp_location(json_response.value("ptemp_longitude"),
                                                        json_response.value("ptemp_latitude"));

            }
            else if ((Data::getInstance()->getStatus() == OBO_STATUS_DERIVER_READY)
                     && orderid != "NONE") {
                //已经有乘客下单 需要提示前端司机是否接单
                Data::getInstance()->setStatus(OBO_STATUS_DERIVER_READY);
            }
            else if ((Data::getInstance()->getStatus() == OBO_STATUS_DRIVER_DRIVE)
                     && orderid == "NONE") {
                //说明订单已经结束，完成订单
                Data::getInstance()->setStatus(OBO_STATUS_DRIVER_IDEL);
            }

            return JNI_TRUE;

        }
        else {
            JNIINFO("ret error data= %s", response_data.c_str());
            return JNI_FALSE;
        }
    }

    return JNI_TRUE;

}


}