//
// Created by 程磊 on 2023/9/16.
//


#include "jvmti.h"
#include <android/log.h>
#include <jni.h>

void jthread_stack(
        jvmtiEnv *jvmti_env,
        JNIEnv *jni_env,
        jthread thread) {

    jvmtiFrameInfo frames[50];
    jint count;
    jvmtiError error = jvmti_env->GetStackTrace(thread, 0, 50, frames, &count);
    if (error != JVMTI_ERROR_NONE) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "内存分配堆栈获取失败！");
        return;
    }

    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "内存分配堆栈！");

    for (int i = 0; i < count; i++) {
        jclass clazz;
        char* classSign;
        char* methodName;
        jmethodID method = frames[i].method;
        jlong location = frames[i].location;
        jvmti_env->GetMethodDeclaringClass(method, &clazz);
        jvmti_env->GetClassSignature(clazz, &classSign, nullptr);
        jvmti_env->GetMethodName(method, &methodName, nullptr, nullptr);
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "%s-%s-%ld", classSign, methodName, location);
        jvmti_env->Deallocate(reinterpret_cast<unsigned char *>(classSign));
        jvmti_env->Deallocate(reinterpret_cast<unsigned char *>(methodName));
    }
}