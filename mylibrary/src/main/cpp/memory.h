//
// Created by chenglei on 2024/2/24.
//

#ifndef JVMTIDMEO_MEMORY_H
#define JVMTIDMEO_MEMORY_H

#endif //JVMTIDMEO_MEMORY_H

#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <iosfwd>
#include "jvmti.h"
#include "sstream"
#include <unordered_map>
#include "jnicall.h"
#include "pthread.h"



// 内存监控相关
void JNICALL objectAlloc
        (jvmtiEnv *jvmti_env,
         JNIEnv *jni_env,
         jthread thread,
         jobject object,
         jclass object_klass,
         jlong size);


void JNICALL objectFree(jvmtiEnv *jvmti_env,
                        jlong tag);

void JNICALL gcStart(jvmtiEnv *jvmti_env);
void JNICALL gcFinish(jvmtiEnv *jvmti_env);