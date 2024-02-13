package com.example.jvmtidmeo

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Toast
import java.lang.ref.WeakReference

/**
 * @author chenglei01
 * @date 2023/9/16
 * @time 16:19
 */
class SecondActivity: Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.layout_second_activity)
    }

    override fun finish() {
        super.finish()
        setResult(RESULT_OK, Intent().apply {
            putExtra("memory", true)
        })
    }


    override fun onDestroy() {
        super.onDestroy()
    }
}