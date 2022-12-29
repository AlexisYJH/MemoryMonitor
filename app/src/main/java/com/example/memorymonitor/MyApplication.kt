package com.example.memorymonitor

import android.app.Application

/**
 * @author AlexisYin
 */
class MyApplication: Application() {
    override fun onCreate() {
        super.onCreate()
        Monitor.init(applicationContext)
    }
}