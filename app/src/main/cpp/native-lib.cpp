#include <jni.h>
#include <string>
#include "jvmti.h"
#include "MemoryFile.h"

jvmtiEnv *mJvmtiEnv = NULL;
MemoryFile *memoryFile;
jlong tag = 0;

//初始化工作
extern "C"
JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    //准备jvmti环境
    vm->GetEnv(reinterpret_cast<void **>(&mJvmtiEnv), JVMTI_VERSION_1_2);
    //开启jvmti的能力
    jvmtiCapabilities caps;
    mJvmtiEnv->GetPotentialCapabilities(&caps);
    mJvmtiEnv->AddCapabilities(&caps);
    return JNI_OK;
}

void JNICALL objectAlloc
        (jvmtiEnv *jvmti_env,
         JNIEnv *jni_env,
         jthread thread,
         jobject object,
         jclass object_klass,
         jlong size) {
    tag += 1;
    jvmti_env->SetTag(object, tag);

    //获取线程信息
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    //获取创建的对象的类签名
    char *classSignature;
    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);

    //写入日志
    char str[500];
    char *format = "object alloc, Thread is %s, class is %s, size is %lld, tag is %lld \r\n";
    sprintf(str, format, threadInfo.name, classSignature, size, tag);
    memoryFile->write(str, sizeof(char) * strlen(str));

    jvmti_env->Deallocate((unsigned char *)classSignature);
}

void JNICALL methodEntry
        (jvmtiEnv *jvmti_env,
         JNIEnv *jni_env,
         jthread thread,
         jmethodID method) {
    jclass clazz;
    char *signature;
    char *methodName;
    //获取方法对应的类
    jvmti_env->GetMethodDeclaringClass(method, &clazz);
    //获取类的签名
    jvmti_env->GetClassSignature(clazz, &signature, nullptr);
    //获取方法名字
    jvmti_env->GetMethodName(method, &methodName, nullptr, nullptr);
    //写入日志文件
    char str[500];
    char *format = "methodEntry method name is %s %s \r\n";
    sprintf(str, format, signature, methodName);
    memoryFile->write(str, sizeof(char) * strlen(str));
    jvmti_env->Deallocate((unsigned char *)signature);
    jvmti_env->Deallocate((unsigned char *)methodName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_memorymonitor_Monitor_agentInit(JNIEnv *env, jobject thiz, jstring _path) {
    //日志文件路径
    const char *path = env->GetStringUTFChars(_path, NULL);
    memoryFile = new MemoryFile(path);
    //开启jvm事件监听
    jvmtiEventCallbacks callbacks;
    callbacks.VMObjectAlloc = &objectAlloc;
    callbacks.MethodEntry = &methodEntry;
    //设置回调函数
    mJvmtiEnv->SetEventCallbacks(&callbacks, sizeof(callbacks));
    //开启监听
    mJvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, NULL);
    mJvmtiEnv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, NULL);

    env->ReleaseStringUTFChars(_path, path);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_memorymonitor_Monitor_agentRelease(JNIEnv *env, jobject thiz) {
    delete memoryFile;
    mJvmtiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, NULL);
    mJvmtiEnv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_METHOD_ENTRY, NULL);
}