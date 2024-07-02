package com.nfl.bsdiffandbspatch;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInstaller;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.nfl.bsdiffandbspatch.databinding.ActivityMainBinding;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Objects;

import nfl.bspatch.BspatchUtil;

public class MainActivity extends AppCompatActivity {

    private final static String TAG = "NFL";
    private final String ACTION_INSTALL = "personal.nfl.INSTALL_APK";
    private ActivityMainBinding binding;
    private String path;
    private Thread patchThread;
    private final Handler handler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == 0) {
                binding.sampleText.setText("Success");
                binding.button2.setVisibility(View.VISIBLE);
            } else {
                binding.button2.setVisibility(View.GONE);
                binding.sampleText.setText("Failed");
            }
            binding.pb.setVisibility(View.GONE);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            boolean haveInstallPermission = getPackageManager().canRequestPackageInstalls();
            if (!haveInstallPermission) {
                Uri packageURI = Uri.parse("package:" + getPackageName());
                Intent intent = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES, packageURI);
                registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
                    if (result.getResultCode() == RESULT_OK) {
                        Toast.makeText(this, "已授权安装 APK", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this, "未授权安装 APK", Toast.LENGTH_SHORT).show();
                    }
                }).launch(intent);
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            //判断是否有管理外部存储的权限
            if (!Environment.isExternalStorageManager()) {
                // ACTION_APPLICATION_DETAILS_SETTINGS
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.fromParts("package", getPackageName(), null));
                startActivity(intent);
            }
        }
        initViews();
        initData();
        binding.button.setOnClickListener(view -> {
            binding.button.setVisibility(View.INVISIBLE);
            binding.pb.setVisibility(View.VISIBLE);
            patchThread.start();
        });
        binding.button2.setVisibility(View.VISIBLE);
        binding.button2.setOnClickListener(v -> {
            try {
                install("old.apk");
            } catch (IOException e) {
                Log.e(TAG, "install apk failed:" + e.getLocalizedMessage());
            }
        });
        binding.btnTestJni.setOnClickListener(v -> startActivity(new Intent(MainActivity.this, JniTestActivity.class)));
    }

    private void initViews() {
        binding.sampleText.setText(ApkExtract.extract(this) + " |"
                + (new File(path + "new.apk").exists() ? "exist" : "!exist")
                + "| "
                + BspatchUtil.sayHello("null"));
    }

    private void initData() {
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "AndroidPatch" + File.separator;
        Log.d(TAG, "apk merge path:" + path);
        patchThread = new Thread(() -> {
            int result = BspatchUtil.bspatch(ApkExtract.extract(MainActivity.this), path + "newDemo.apk", path + "patchFileDemo.apk");
            handler.sendEmptyMessage(result);
        });
    }

    static {
        System.loadLibrary("native-lib");
    }

    // 提取当前应用的apk
    private static class ApkExtract {
        public static String extract(Context context) {
            context = context.getApplicationContext();
            ApplicationInfo applicationInfo = context.getApplicationInfo();
            return applicationInfo.sourceDir;
        }
    }

    /**
     * 安装
     */
    private void install(String assetApkPath) throws IOException {
        PackageInstaller packageInstaller = getPackageManager().getPackageInstaller();
        PackageInstaller.SessionParams params = new PackageInstaller.SessionParams(PackageInstaller.SessionParams.MODE_FULL_INSTALL);
        // 创建ID
        int id = packageInstaller.createSession(params);
        // 打开 Session
        PackageInstaller.Session session = packageInstaller.openSession(id);
        // 写入文件
        writeAssetsApk2Session(session, assetApkPath);
        session.commit(createIntentSender());
    }

    /**
     * 写入Apk到Session输出流，该例子 获取Assets内文件，可通过其他方式获取Apk流
     */
    private void writeAssetsApk2Session(PackageInstaller.Session session, String assetApkPath) throws IOException {
        OutputStream outputStream = session.openWrite("apk", 0, -1);
        InputStream inputStream = getAssets().open(assetApkPath);
        byte[] bytes = new byte[10 * 1024];
        int length;
        while ((length = inputStream.read(bytes)) > 0) {
            outputStream.write(bytes, 0, length);
        }
        outputStream.close();
        inputStream.close();
    }

    /**
     * 新建一个IntentSender用于接收结果
     * 该例子通过当前页面接收
     */
    private IntentSender createIntentSender() {
        Intent intent = new Intent(this, MainActivity.class);
        intent.setAction(ACTION_INSTALL);
        return PendingIntent.getActivity(this, 0, intent, 0).getIntentSender();
    }

    /**
     * 接收安装结果
     */
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        if (!Objects.equals(intent.getAction(), ACTION_INSTALL)) {
            return;
        }
        Bundle bundle = intent.getExtras();
        if (bundle == null) {
            return;
        }
        int installResult = bundle.getInt(PackageInstaller.EXTRA_STATUS);
        switch (installResult) {
            case PackageInstaller.STATUS_PENDING_USER_ACTION:
                //提示用户进行安装
                startActivity((Intent) bundle.get(Intent.EXTRA_INTENT));
                break;
            case PackageInstaller.STATUS_SUCCESS:
                //安装成功
                startActivity((Intent) bundle.get(Intent.EXTRA_INTENT));
                break;
            default:
                //失败信息
                Toast.makeText(this, bundle.getString(PackageInstaller.EXTRA_STATUS_MESSAGE), Toast.LENGTH_LONG).show();
                break;
        }
    }
}
