package com.example.memorymonitor

import android.content.Context
import android.os.Build
import android.os.Debug
import java.io.File

/**
 * @author AlexisYin
 */
object Monitor {
    private const val LIB_NAME = "libmemorymonitor.so"

    fun init(context: Context) {
        //加载so库
        val agentPath = File(context.dataDir, "lib").absolutePath + File.separatorChar + LIB_NAME
        System.load(agentPath)
        //agent连接到jvmti
        attachAgent(agentPath, context.classLoader)
        //开始jvmti事件监听
        val logDir = File(context.filesDir, "log")
        if (!logDir.exists()) logDir.mkdirs()
        val path = "${logDir.absolutePath}/${System.currentTimeMillis()}.log"
        agentInit(path)
    }

    //连接代理
    private fun attachAgent(agentPath: String, classLoader: ClassLoader?) {
        //9.0+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            Debug.attachJvmtiAgent(agentPath, null, classLoader)
        } else {
            val vmDebugClazz = Class.forName("dalvik.system.VMDebug")
            val attachAgentMethod = vmDebugClazz.getMethod("attachAgent", String::class.java)
            attachAgentMethod.isAccessible = true
            attachAgentMethod.invoke(null, agentPath)
        }
    }

    fun release() {
        agentRelease()
    }

    private external fun agentInit(path: String)
    private external fun agentRelease()
}