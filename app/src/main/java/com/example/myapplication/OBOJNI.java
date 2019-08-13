package com.example.myapplication;

/**
 * Created by Ace on 2017/8/5.
 */

//专门调用jni C++接口的类
public class OBOJNI {


    public static OBOJNI getInstance() {
        if (instance == null) {
            instance = new OBOJNI();
        }

        return instance;
    }

    private static  OBOJNI instance = null;
    //加载cpp给提供的 动态库
    static {
        System.loadLibrary("testjni"); //libtestjni.so
    }
    //登陆的jni接口login
    public native  boolean login(String username, String passwd, boolean isDriver);

    //注册的jni接口reg
    public native boolean reg(String username ,String passwd, String email, String phone,
                              String idCard, boolean isDriver);

    public native boolean StartOrder(String src_longtitude,
                                     String src_latitude,
                                     String src_address,
                                     String dst_longitude,
                                     String dst_latitude,
                                     String dst_address,
                                     String RMB);

    public native boolean DriverLocationChanged(String longitude,
                                                String latitude,
                                                String address,
                                                String autoSend);

    //乘客端定位发生改变，上传地理位置信息 JNI模块
    public native  boolean PassengerLocationChanged(String longitude,
                                                    String latitude,
                                                    String address,
                                                    String dst_longitude,
                                                    String dst_latitude,
                                                    String dst_address);

    public native boolean FinishOrder();

    public native void setStatus(String status);

    public native String getOrderid();
    public native String getSessionid();
    public native String getStatus();
    public native String getIsDriver();
    public native String getPtempLongitude();
    public native String getPtempLatitude();
    public native String getDtempLongitude();
    public native String getDtempLatitude();

    public native void testLibcurl();















}
