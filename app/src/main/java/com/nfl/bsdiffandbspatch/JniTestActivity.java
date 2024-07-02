package com.nfl.bsdiffandbspatch;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.nfl.bsdiffandbspatch.databinding.ActivityJniTestBinding;

public class JniTestActivity extends AppCompatActivity {

    private ActivityJniTestBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityJniTestBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        binding.btnTest.setOnClickListener(v -> {
            binding.tvText.setText(stringFromJNI());
        });
    }

    public native String stringFromJNI();
}