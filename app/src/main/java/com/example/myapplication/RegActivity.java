package com.example.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Toast;

public class RegActivity extends AppCompatActivity {

    private EditText et_username = null;
    private EditText et_passwd1 = null;
    private EditText et_passwd2 = null;
    private EditText et_email = null;
    private EditText et_phone = null;
    private EditText et_idCard = null;
    private CheckBox cb_isDriver = null;
    private Button bt_submit = null;

    boolean isDriver = false;

    String Tag = "Activity";


    protected  void initUI() {
        et_username = (EditText)findViewById(R.id.et_username_reg);
        et_passwd1 = (EditText)findViewById(R.id.et_passwd1_reg);
        et_passwd2 = (EditText)findViewById(R.id.et_passwd2_reg);
        et_email = (EditText)findViewById(R.id.et_email_reg);
        et_phone = (EditText)findViewById(R.id.et_phone_reg);
        et_idCard = (EditText)findViewById(R.id.et_idcard_reg);
        cb_isDriver = (CheckBox)findViewById(R.id.cb_isDrvier_reg);
        bt_submit = (Button)findViewById(R.id.bt_submit_reg);
    }

    protected  void bindUIEvent() {
        cb_isDriver.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked == true) {
                    isDriver = true;
                }
                else {
                    isDriver = false;
                }
            }
        });


        bt_submit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean reg_result = true;

                String username = et_username.getText().toString();
                String passwd1 = et_passwd1.getText().toString();
                String passwd2 = et_passwd2.getText().toString();
                String email = et_email.getText().toString();
                String phone = et_phone.getText().toString();
                String idCard = et_idCard.getText().toString();


                //判断是否为空
                if (username.isEmpty() == true) {
                    Toast.makeText(getApplicationContext(),"用户名不能为空", Toast.LENGTH_SHORT).show();
                    return;
                }
                if (passwd1.isEmpty() == true || passwd2.isEmpty() == true) {
                    Toast.makeText(getApplicationContext(),"密码不能为空", Toast.LENGTH_SHORT).show();
                    return;
                }

                if (!passwd1.equals(passwd2)) {
                    Toast.makeText(getApplicationContext(),"两次密码不同", Toast.LENGTH_SHORT).show();
                    return;
                }


                //将全部信息 发送给远程服务器   会送服务器得到一个结果 放在reg_result

                if (reg_result == true) {
                    //注册成功
                    Log.e(Tag, "注册成功 username = "+
                            username+", passwd = "+
                            passwd1+" email = "+
                            email + "phone =" +
                            phone+ "idCard = "+
                            idCard);
                    Log.e(Tag, "isDriver = "+isDriver);

                    Intent intent = new Intent();
                    if (isDriver == true) {
                        intent.setClass(RegActivity.this, DriverActivity.class);
                    }
                    else {
                        intent.setClass(RegActivity.this, PassengerActivity.class);

                    }
                    startActivity(intent);
                }
                else {
                    //注册失败
                    Log.e(Tag, "注册失败!!! username = "+
                            username+", passwd = "+
                            passwd1+" email = "+
                            email + "phone =" +
                            phone+ "idCard = "+
                            idCard);
                    Log.e(Tag, "isDriver = "+ isDriver);
                }
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_reg);

        initUI();


        bindUIEvent();
    }
}
