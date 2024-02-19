//
// Created by chenglei on 2024/2/18.
//

#include <android/log.h>
#include <mutex>
#include "jnicall.h"

std::mutex reportMtx;


jclass jvmtiFindClass(JNIEnv *env, jvmtiEnv *jvmtiEnv, jstring className, const char *inVmName) {


    jvmtiThreadInfo info;
    jclass threadClazz = env->FindClass("java/lang/Thread");
    jmethodID currentThreadMethodId = env->GetStaticMethodID(threadClazz, "currentThread",
                                                             "()Ljava/lang/Thread;");

    jthread currentThread = env->CallStaticObjectMethod(threadClazz, currentThreadMethodId);
    jvmtiEnv->GetThreadInfo(currentThread, &info);
    jobject classLoader = info.context_class_loader;
    if (classLoader==nullptr){
        //使用默认的类加载器
       jmethodID classLoaderMethodId=env->GetMethodID(threadClazz,"getContextClassLoader", "()Ljava/lang/ClassLoader;");
       classLoader=env->CallObjectMethod(currentThread,classLoaderMethodId);
    }
    if (classLoader == nullptr)
    {
        jclass classLoaderClazz=env->FindClass("java/lang/ClassLoader");
        jmethodID getSystemClassLoaderMethodId=env->GetStaticMethodID(classLoaderClazz,"getSystemClassLoader","()Ljava/lang/ClassLoader;");
        classLoader=env->CallStaticObjectMethod(classLoaderClazz,getSystemClassLoaderMethodId);
    }


    if (classLoader==nullptr)
    {
        jclass classClazz = env->FindClass("com/example/mylibrary/JvmtiReport");
        return classClazz;

    } else
    {
        jclass classClazz = env->FindClass("java/lang/Class");
        jmethodID forNameMethodId = env->GetStaticMethodID(classClazz, "forName",
                                                           "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;");


        __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni",
                            "classclazz is null: %d,forNameMethodId is null:%d,,classLoader is null:%d,threadname:%s",
                            classClazz == nullptr, forNameMethodId == nullptr, classLoader == nullptr,
                            info.name);


        return static_cast<jclass>(env->CallStaticObjectMethod(classClazz,
                                                               forNameMethodId,
                                                               className, JNI_FALSE,
                                                               classLoader));
    }


}


void jvmtiReport(JNIEnv *jni_env, jvmtiEnv *jvmtiEnv, JavaVM *vm, int type, const char *log) {
    bool thread = false;
    if (jni_env == nullptr) {
        thread = true;
    }
    if (thread) {
        // pthread，需要attach到jvm，才能获取到 JNIEnv指针
        int ret = vm->AttachCurrentThread(&jni_env, nullptr);
        __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "当前线程关联jvm是否成功：%d",ret==JNI_OK);
    }
    jstring name = jni_env->NewStringUTF("com.example.mylibrary.JvmtiReport");
    jclass target = jvmtiFindClass(jni_env, jvmtiEnv, name, "com/example/mylibrary/JvmtiReport");
    if (target == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "target is null");
        return;
    }


    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "g_targetClazz:%p,is null:%d", target,
                        target == nullptr);
    jmethodID reportMethodId = jni_env->GetStaticMethodID(target, "report",
                                                          "(ILjava/lang/String;)V");

    jstring logParam = jni_env->NewStringUTF(log);
    jni_env->CallStaticVoidMethod(target, reportMethodId, type, logParam);
    if (thread) {
        vm->DetachCurrentThread();
    }
}

void jvmtiReportWithPInner(JvmtiReportParam param) {
    jvmtiReport(nullptr, param.jvmtiEnv, param.vm, param.type, param.log);
}

void jvmtiReportWithP(JvmtiReportParam *param) {
    reportMtx.lock();
    pthread_t pthread;
    pthread_create(&pthread, NULL, reinterpret_cast<void *(*)(void *)>(jvmtiReportWithPInner),
                   (void *) param);
    reportMtx.unlock();
}