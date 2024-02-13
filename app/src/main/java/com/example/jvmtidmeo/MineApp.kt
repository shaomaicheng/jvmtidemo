package com.example.jvmtidmeo

import android.app.Application
import com.example.mylibrary.JvmtiConfig

/**
 * @author chenglei01
 * @date 2023/9/16
 * @time 17:51
 */
class MineApp:Application() {
    override fun onCreate() {
        super.onCreate()
        JvmtiConfig().enableJvmti(this)
    }
}