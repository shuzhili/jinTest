package com.example.myapplication;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class JNITestActivity extends AppCompatActivity {

    private Button bt_jni_test1 = null;
    private Button bt_jni_test2 = null;
    private Button bt_jni_test3 = null;
    private Button bt_jni_test4 = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_jnitest);

        bt_jni_test1 = (Button)findViewById(R.id.bt_jni_test1);
        bt_jni_test2 = (Button)findViewById(R.id.bt_jni_test2);
        bt_jni_test3 = (Button)findViewById(R.id.bt_jni_test3);
        bt_jni_test4 = (Button)findViewById(R.id.bt_jni_test4);


        bt_jni_test1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OBOJNI.getInstance().hello_jni();
            }
        });

        bt_jni_test2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //调用传参int的jni接口
                int ret_int = OBOJNI.getInstance().test_jni_api(100,200);
                Log.e("testjni", "java: ret_int = "+ ret_int);
            }
        });

        bt_jni_test3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean flag = OBOJNI.getInstance().test_jni_api2(true);
                Log.e("testjni", "java: bool = "+ flag);
            }
        });

        bt_jni_test4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ret_string = OBOJNI.getInstance().test_jni_api3("Java:gailun", "Java:Yase");
                Log.e("testjni", "java: ret_string = " + ret_string);


                //测试数组jni接口
                int[] iList = {1,2,3,4,5};
                OBOJNI.getInstance().test_jni_api4_array(iList);
            }
        });

    }
}
