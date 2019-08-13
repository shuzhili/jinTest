//
// Created by Administrator on 2019/8/12.
//
#include "OBOJni.h"
#include "Json.h"
#include "Curl.h"
#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif


void get_time_str(char* time_str){
    time_t t=time(NULL);
    strftime(time_str,TIME_STR_LEN,"%Y-%m-%d %X",localtime(&t));
}

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    StartOrder
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_myapplication_OBOJNI_StartOrder
        (JNIEnv *env, jobject obj, jstring jstc_longitudde, jstring jsrc_latitude, jstring jsrc_address,
                jstring jdst_longitude, jstring jdst_latitude, jstring jdst_address, jstring jRMB){
    const char *src_longitude=env->GetStringUTFChars(jstc_longitudde,NULL);
    const char *src_latitude=env->GetStringUTFChars(jsrc_latitude,NULL);
    const char *src_address=env->GetStringUTFChars(jsrc_address,NULL);
    const char *dst_longitude=env->GetStringUTFChars(jdst_longitude,NULL);
    const char *dst_latitude=env->GetStringUTFChars(jdst_latitude,NULL);
    const char *dst_address=env->GetStringUTFChars(jdst_address,NULL);
    const char *RMB=env->GetStringUTFChars(jRMB,NULL);

    /*
     ==== 给服务端的协议 ====
    https://ip:port/startSetOrder  [json_data]
    {
        sessionid: "online-user-xxxxxx-xxx-xxx-xxx-xxxxx",
        driver:    "no",
        src_longitude: "108.123123123",//乘客当前经度
        src_latitude : "39.123123123", //乘客当前纬度
        dst_longitude: "90.123123123", //乘客目标经度
        dst_latitude:  "39.123123123",  //乘客目标纬度
        src_address:   "北京西三旗百汇城",
        dst_address:   "北京西二旗软件园9号楼",
        RMB:          "315￥",         //本单大约消费
        create_order_time:"2016-11-12 12:00:00" //创建订单时间
    }
   */

    Json json;
    json.insert("sessionid",Data::getInstance()->getSessionid().c_str());
    json.insert("driver","no");
    json.insert("src_longitude",src_longitude);
    json.insert("src_latitude",src_latitude);
    json.insert("dst_longitude",dst_longitude);
    json.insert("dst_latitude",dst_latitude);
    json.insert("src_address",src_address);
    json.insert("dst_address",dst_address);
    json.insert("RMB",RMB);
    char time[TIME_STR_LEN]={0};
    get_time_str(time);
    json.insert("create_order_time",time);

    string json_str=json.print();

    env->ReleaseStringUTFChars(jstc_longitudde,src_longitude);
    env->ReleaseStringUTFChars(jsrc_latitude,src_latitude);
    env->ReleaseStringUTFChars(jdst_address,src_address);
    env->ReleaseStringUTFChars(jdst_longitude,dst_longitude);
    env->ReleaseStringUTFChars(jdst_latitude,dst_latitude);
    env->ReleaseStringUTFChars(jdst_address,dst_address);
    env->ReleaseStringUTFChars(jRMB,RMB);

    string url=OBO_SERVER_IP;
    url+=":";
    url+=OBO_SERVER_PORT;
    url+="/startSetOrder";

    Curl curl(url, true);

    if(curl.execute(json_str)==false){
        JNIINFO("%s","curl execute error")
        return JNI_FALSE;
    }
    string response_data = curl.responseData();
    Json json_response;
    json_response.parse(curl.responseData());

    string result = json_response.value("result");
    if (result.length() != 0) {
        if (result == "ok") {

            string recode = json_response.value("recode");
            if (recode == "0") {
                Data::getInstance()->setOrderid(json_response.value("ordierid"));
                JNIINFO("reg succ, orderid=%s", Data::getInstance()->getOrderid().c_str());
                return JNI_TRUE;
            }
            else if (recode == "3") {
                JNIINFO("%s", "waiting driver...");
                return JNI_FALSE;
            }

        }
        else {
            JNIINFO("ret error data= %s", response_data.c_str());
            return JNI_FALSE;
        }
    }


    return JNI_TRUE;


}

#ifdef __cplusplus
}
#endif