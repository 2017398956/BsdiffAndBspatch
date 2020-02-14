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

    private String path;
    private TextView tv;
    private Button button;
    private ProgressBar pb;
    private Thread patchThread;
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == 0) {
                tv.setText("Success");
            } else {
                tv.setText("Failed");
            }
            button.setVisibility(View.VISIBLE);
            pb.setVisibility(View.GONE);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        stringFromJNI("Hello C++" , "1.0") ;
//        stringFromJNI("Hello C++" , "1.0") ;
        setContentView(R.layout.activity_main);
        initViews();
        initData();
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                button.setVisibility(View.INVISIBLE);
                pb.setVisibility(View.VISIBLE);
                patchThread.start();
            }
        });
    }

    private void initViews() {
        tv = findViewById(R.id.sample_text);
        button = findViewById(R.id.button);
        pb = findViewById(R.id.pb);
        tv.setText(ApkExtract.extract(this) + " |"
                + (new File(path + "new.apk").exists() ? "exist" : "!exist")
                + "| "
                + BspatchUtil.sayHello(null));
    }

    private void initData() {
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "AndroidPatch" + File.separator;
        patchThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int result = BspatchUtil.bspatch(ApkExtract.extract(MainActivity.this), path + "newDemo.apk", path + "patchFileDemo.apk");
                handler.sendEmptyMessage(result);
            }
        });
    }

//    public native String stringFromJNI(String str , String version);
//    public native String stringFromJNI(String str);

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
