package com.example.myapplication;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;

public class LoginActivity extends AppCompatActivity {

    //写代码 定义数据类型 需要先自动补齐，锁定之后 直接按回车
    private TextView  tv_name = null;
    //定义一个button按钮对象
    private Button bt_test  = null;

    private TextView tv_test2 = null;
    private EditText et_test = null;

    //定义一个checkbox对象
    private CheckBox cb_test = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        //在此处就直接可以写自己的业务了。

        //将tv_name对象 和 布局页面中创建的 UI控件 TextView相关联
        tv_name = (TextView) findViewById(R.id.tv_username);
        //更改tv_username控件的文本内容
        tv_name.setText("username用户名");

        //将button对象和  bt_test控件相关联
        bt_test = (Button) findViewById(R.id.bt_test);

        tv_test2 = (TextView)findViewById(R.id.tv_test2);
        et_test = (EditText)findViewById(R.id.et_test);

        cb_test = (CheckBox)findViewById(R.id.cb_test);

        //给button设定一个点击事件
        bt_test.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //此时已经代表 bt 按钮被点击了
                tv_name.setText("按钮已经被点击");


                //先从et_test获取文本
                String text = et_test.getText().toString();

                //将获取的文本 给tv_test2
                tv_test2.setText(text);

            }
        });

        //给cb_test对象设定一个回调函数
        cb_test.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                //checkbox已经被选中
                if (isChecked == true)  {
                    //目前是被选中状态
                    tv_test2.setText("checkbox 被选中");
                }
                else {
                    //目前是不是被选中状态
                    tv_test2.setText("checkbox 没有被选中");

                }
            }
        });
    }
}
