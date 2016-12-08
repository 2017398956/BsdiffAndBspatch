package com.nfl.bsdiffandbspatch;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.ProviderInfo;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.io.File;

import nfl.bspatch.BspatchUtil;

public class MainActivity extends AppCompatActivity {

    private String path ;
    private TextView tv ;
    private Button button ;
    private ProgressBar pb ;
    private Thread patchThread ;
    private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what == 0){
                tv.setText("Success");
            }else {
                tv.setText("Failed");
            }
            button.setVisibility(View.VISIBLE);
            pb.setVisibility(View.GONE);
        }
    } ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "AndroidPatch" + File.separator;
        patchThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int result = BspatchUtil.bspatch(ApkExtract.extract(MainActivity.this), path + "newDemo.apk", path + "patchFileDemo.apk") ;
                handler.sendEmptyMessage(result) ;
            }
        }) ;
        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
        button = (Button) findViewById(R.id.button);
        pb = (ProgressBar) findViewById(R.id.pb) ;
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                button.setVisibility(View.INVISIBLE);
                pb.setVisibility(View.VISIBLE);
                patchThread.start();
            }
        });
        // tv.setText(BspatchUtil.say("中文测试") + BspatchUtil.add(660 , 6));
        File newFile = new File(path + "new.apk");
        String test = null;
        if (newFile.exists()) {
            test = "exist";
        } else {
            test = "!exist";
        }
        tv.setText(ApkExtract.extract(this) + " |" + test + "| " + BspatchUtil.testAdd2(1000, 900));
        // 不知道为什么这个方法调用后会崩溃
        // tv.setText(BspatchUtil.sayHello("中文测试") + BspatchUtil.add(100 , 900));
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    // 提取当前应用的apk
    private static class ApkExtract {
        public static String extract(Context context) {
            context = context.getApplicationContext();
            ApplicationInfo applicationInfo = context.getApplicationInfo();
            String apkPath = applicationInfo.sourceDir;
            return apkPath;
        }
    }
}
