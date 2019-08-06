package com.example.myapplication;

public class HelloJni {

    /*改类是一个单例，（并不一定非得单例）*/
    protected static HelloJni obj = null;
    public static HelloJni getInstance() {
        if (obj == null) {
            obj = new HelloJni();
        }
        return obj;
    }

    //表示这个 helloJniJava接口，并不是java写的。是由比人提供的。
    public native void helloJniJava();

    //加载cpp给提供的 动态库
    static {
        System.loadLibrary("myJni"); //libmyJni.so
    }
}