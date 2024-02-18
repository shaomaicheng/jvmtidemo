//
// Created by chenglei on 2024/2/18.
//

#include <android/log.h>
#include "jnicall.h"
#include "jvmti.h"
#include "sys/syscall.h"
#include "pthread.h"

jclass jvmtiFindClass(JNIEnv *env, jvmtiEnv *jvmtiEnv, jstring className) {
    jclass classClazz = env->FindClass("java/lang/Class");
    jmethodID forNameMethodId = env->GetStaticMethodID(classClazz, "forName",
                                                       "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;");

    jvmtiThreadInfo info;
    jclass threadClazz = env->FindClass("java/lang/Thread");
    jmethodID currentThreadMethodId = env->GetStaticMethodID(threadClazz, "currentThread",
                                                             "()Ljava/lang/Thread;");
    jthread currentThread = env->CallStaticObjectMethod(threadClazz, currentThreadMethodId);
    jvmtiEnv->GetThreadInfo(currentThread, &info);
    jobject classLoader = info.context_class_loader;

    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni",
                        "classclazz is null: %d,forNameMethodId is null:%d,,classLoader is null:%d,threadname:%s",
                        classClazz ==nullptr, forNameMethodId ==nullptr, classLoader ==nullptr,info.name);

    jclass targetClazz = static_cast<jclass>(env->CallStaticObjectMethod(classClazz,
                                                                         forNameMethodId,
                                                                         className, JNI_FALSE,
                                                                         classLoader));
    return targetClazz;
}

JNIEnv *detectJniEnv(JavaVM *g_JavaVM) {
    JNIEnv *env;
    //获取当前native线程是否有没有被附加到jvm环境中
    int getEnvStat = g_JavaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "GetEnv: %d", getEnvStat);
    return env;
}


void jvmtiReport(JNIEnv *jni_env, jvmtiEnv *jvmtiEnv, JavaVM *vm, int type, const char *log) {
    jboolean thread = false;
    if (jni_env == nullptr) {
        thread = true;
    }
    if (thread) {
        vm->AttachCurrentThread(&jni_env, nullptr);
    }
    jstring name = jni_env->NewStringUTF("com.example.mylibrary.JvmtiReport");
    jclass reportClazz = jvmtiFindClass(jni_env, jvmtiEnv, name);
    if (reportClazz == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "reportClazz is null");
        return;
    }

    jmethodID reportMethodId = jni_env->GetStaticMethodID(reportClazz, "report",
                                                          "(ILjava/lang/String;)V");
    jstring logParam = jni_env->NewStringUTF(log);
    jni_env->CallStaticVoidMethod(reportClazz, reportMethodId, type, logParam);
    if (thread) {
        vm->DetachCurrentThread();
    }
}

void jvmtiReportWithP(JvmtiReportParam param) {
    jvmtiReport(nullptr, param.jvmtiEnv, param.vm, param.type, param.log);
}