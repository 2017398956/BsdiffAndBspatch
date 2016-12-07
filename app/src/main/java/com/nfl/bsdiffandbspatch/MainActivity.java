package com.nfl.bsdiffandbspatch;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import nfl.bspatch.BspatchUtil;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(BspatchUtil.say("中文测试") + BspatchUtil.add(660 , 6));
         tv.setText(BspatchUtil.bspatch("a", "b", "c") + " | " + BspatchUtil.testAdd2(1000, 900));
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
