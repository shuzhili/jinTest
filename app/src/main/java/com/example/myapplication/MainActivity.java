package com.example.myapplication;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        HelloJni.getInstance().helloJniJava();

        //测试 String 传参 JNI
        String str = HelloJni.getInstance().getLine("java's string");
        Log.e("lsz", str);

        String str2 = HelloJni.getInstance().testBooleanInt(9, true);
        Log.e("lsz", str2);

        //测试 int 数组 ， boolean数组， String 数组 传参
        int[] iList = new int[]{1, 2, 3, 4, 5, 6, 7, 8};
        String[] sList = new String[]{"hello", "ITCAST", "so", "COOL!!"};
        boolean[] bList = new boolean[]{true, false, true, false};
        String retList[] = HelloJni.getInstance().testArray(iList, sList, bList);

        for (int i = 0; i < retList.length; i++) {
            Log.e("lsz", retList[i]);
        }

        Student student = HelloJni.getInstance().nativeGetStudentInfo();
        Log.e("lsz", student.toString());
    }
}
