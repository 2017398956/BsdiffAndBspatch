package com.nfl.bsdiffandbspatch;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.nfl.bsdiffandbspatch.databinding.ActivityJniTestBinding;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.Method;

public class JniTestActivity extends AppCompatActivity {

    private static final String TAG = "NFL";
    private ActivityJniTestBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityJniTestBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        binding.btnTest.setOnClickListener(v -> {
            binding.tvText.setText(stringFromJNI());
            Runtime runtime = Runtime.getRuntime();
            String filePath = "/data/local/tmp/myprocess";
            Log.d(TAG, "fileSize:" + new File(filePath).length());
            try {
                // 1.
                Process process = runtime.exec(filePath);
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                StringBuilder stringBuilder = new StringBuilder();
                String temp;
                while ((temp = bufferedReader.readLine()) != null) {
                    stringBuilder.append(temp);
                }
                Log.d(TAG, "result:" + stringBuilder);
                // 2.
                String[] args = new String[]{"ls", "-l"};
                Log.d("NFL", "ls result:" + new BufferedReader(new InputStreamReader(runtime.exec(args).getInputStream())).readLine());
                // 3.
                // android.os.Exec is not included in android.jar so we need to use reflection.
                Class execClass = Class.forName("android.os.Exec");
                Method createSubprocess = execClass.getMethod(
                        "createSubprocess",
                        String.class, String.class, String.class, int[].class
                );
                Method waitFor = execClass.getMethod("waitFor", int.class);
                // Executes the command.
                // NOTE: createSubprocess() is asynchronous.
                int[] pid = new int[]{1};
                FileDescriptor fd = (FileDescriptor) createSubprocess.invoke(
                        null,
                        "/system/bin/ls",
                        "/sdcard",
                        null,
                        pid
                );

                // Reads stdout.
                // NOTE: You can write to stdin of the command using new FileOutputStream(fd).
                FileInputStream fis = new FileInputStream(fd);
                BufferedReader reader = new BufferedReader(new InputStreamReader(fis));
                StringBuilder output = new StringBuilder();
                try {
                    String line = "";
                    while ((line = reader.readLine()) != null) {
                        output.append(line).append("\n");
                    }
                } catch (IOException e) {
                    Log.e(TAG, "readLine failed:" + e.getLocalizedMessage());
                }
                // Waits for the command to finish.
                waitFor.invoke(null, pid[0]);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
            Log.d("NFL", "click button.");
        });
        binding.btnLongWaiting.setOnClickListener(v -> testLongWaiting());
    }

    private void testLongWaiting() {
        long oldTime = System.currentTimeMillis();
        long curTime = 0;
        while ((curTime - oldTime) < 3000) {
            curTime = System.currentTimeMillis();
            Log.d(TAG, "curTime:" + curTime);
        }
    }

    public native String stringFromJNI();
}