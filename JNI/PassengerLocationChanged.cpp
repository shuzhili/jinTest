//
// Created by Administrator on 2019/8/13.
//
#include "OBOJni.h"
#include "Json.h"
#include "Data.h"
#include "Curl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_example_myapplication_OBOJNI
 * Method:    PassengerLocationChanged
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
//String longitude,String latitude,String address,String dst_longitude,String dst_latitude,String dst_address
JNIEXPORT jboolean JNICALL Java_com_example_myapplication_OBOJNI_PassengerLocationChanged
        (JNIEnv *env, jobject obj, jstring j_longitude, jstring j_latitude, jstring j_address,
         jstring j_dst_longitude, jstring j_dst_latitude, jstring j_dst_address) {

    const char *longitude = env->GetStringUTFChars(j_longitude, NULL);
    const char *latitude = env->GetStringUTFChars(j_latitude, NULL);
    const char *address = env->GetStringUTFChars(j_address, NULL);
    const char *dst_longitude = env->GetStringUTFChars(j_dst_longitude, NULL);
    const char *dst_latitude = env->GetStringUTFChars(j_dst_latitude, NULL);
    const char *dst_address = env->GetStringUTFChars(j_dst_address, NULL);

    /* ==== 给服务端的协议 ====
    https://ip:port/locationChanged  [json_data]
    {
        sessionid: "online-user-xxxxxx-xxx-xxx-xxx-xxxxx",
        driver:    "no",
        status:    "idle",//当前司机端状态
        longitude: "188.123123123",//当前经度
        latitude : "39.123123123", //当前纬度
        address: "北京市西三旗金燕龙办公楼"，//当前地址

        //如果driver="no",并且 status="waiting",
        //则添加以下目标地址字段
        dst_longitude:"123.123123123",
        dst_latitude: "123.123123123",
        dst_address:"北京市西二旗软件园9号楼"
    } */
    Json json;
    json.insert("sessionid", Data::getInstance()->getSessionid().c_str());
    json.insert("driver", "no");
    json.insert("status", Data::getInstance()->getStatus().c_str());
    json.insert("longitude", longitude);
    json.insert("latitude", latitude);
    json.insert("address", address);
    if (Data::getInstance()->getStatus() == OBO_STATIS_PASSNGER_WAIT) {
        json.insert("dst_longitude", dst_longitude);
        json.insert("dst_latitude", dst_latitude);
        json.insert("dst_address", dst_address);
    }
    string json_str = json.print();
    //释放字符串资源
    env->ReleaseStringUTFChars(j_longitude, longitude);
    env->ReleaseStringUTFChars(j_latitude, latitude);
    env->ReleaseStringUTFChars(j_address, address);
    env->ReleaseStringUTFChars(j_dst_longitude, dst_longitude);
    env->ReleaseStringUTFChars(j_dst_latitude, dst_latitude);
    env->ReleaseStringUTFChars(j_dst_address, dst_address);

    string url = OBO_SERVER_IP;
    url += ":";
    url += OBO_SERVER_PORT;
    url += "/locationChanged";

    Curl curl(url, true);

    if (curl.execute(json_str) == false) {
        JNIINFO("%s", "curl execute error");
        return JNI_FALSE;
    }

    /*
 *
 *
  ====得到服务器响应数据 ====
     //成功
     {
            result: "ok",
            recode: "0",

            status: "idle",//司机or乘客当前的状态
            orderid: "orderid-xxxx-xxx-xxx-xxxx",//没有则为“NONE”

            //司机当前坐标，
            //供waiting和traveling的乘客使用，乘客端会返回该两个字段
            dtemp_longitude:  "123.122123",
            dtemp_latitude:  "32.22222222",

            //如果乘客为“idle"状态
            //需要列车如下周边已经接待的司机信息
            count:  "2", //附近司机个数
            drivers:
            [
            {
             sessionid:"online-driver-xxxx-xxx-xxx-xxx-xxxx",
             distance: "10",//与自己距离多少米
             longitude:  "97.123123123",
             latitude:   "39.123123123"
            }，
            {
             sessionid:"online-driver-xxxx-xxx-xxx-xxx-xxxx",
             distance: "15",//与自己距离多少米
             longitude:  "99.123123123",
             latitude:   "39.123123123"
            }
            ]
     }
     //失败
     {
            result: "error",
            recode: "1", //1 -代表session失效，需要重新登录
            reason: "why...."
     }*/
    string response_data = curl.responseData();
    Json json_response;
    json_response.parse(response_data);

    string result = json_response.value("result");
    if (result.length() != 0) {
        if (result == "ok") {
            string status = json_response.value("status");
            Data::getInstance()->setStatus(status);

            string orderid = json_response.value("orderid");
            Data::getInstance()->setOrderid(orderid);

            if ((Data::getInstance()->getStatus() == OBO_STATUS_PASSNGET_TRAVEL)
                && orderid == "NONE") {
                //说明订单已经结束，完成订单
                Data::getInstance()->setStatus(OBO_STATUS_PASSNGER_IDLE);
            } else if ((Data::getInstance()->getStatus() == OBO_STATUS_PASSNGER_IDLE)
                       && orderid != "NONE") {
                //已经有司机接单，更新为waiting状态
                Data::getInstance()->setStatus(OBO_STATIS_PASSNGER_WAIT);
            } else if ((Data::getInstance()->getStatus() == OBO_STATIS_PASSNGER_WAIT)
                       || (Data::getInstance()->getStatus() == OBO_STATUS_PASSNGET_TRAVEL)) {
                //得到司机的当前坐标
                Data::getInstance()->set_dtemp_location(json_response.value("dtemp_longitude"),
                                                        json_response.value("dtemp_latitude"));

                if (Data::getInstance()->getStatus() == OBO_STATIS_PASSNGER_WAIT) {
                    //得到当前订单状态
                    string order_status = json_response.value("order_status");
                    if (order_status == "ACTIVE") {
                        //司机已经确认乘客上车，需要更变为traveling状态
                        Data::getInstance()->setStatus(OBO_STATUS_PASSNGET_TRAVEL);
                    }
                }
            }
            return JNI_TRUE;
        }
    } else {
        JNIINFO("ret error data= %s", response_data.c_str());
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif
