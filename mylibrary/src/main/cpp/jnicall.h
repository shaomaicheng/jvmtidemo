//
// Created by chenglei on 2024/2/18.
//

#include <jni.h>
#include "jvmti.h"

#ifndef JVMTIDMEO_JNICALL_H
#define JVMTIDMEO_JNICALL_H



#endif //JVMTIDMEO_JNICALL_H

struct JvmtiReportParam {
    jvmtiEnv *jvmtiEnv;
    JavaVM *vm;
    int type;
    const char *log;
};

void jvmtiReport(JNIEnv* jni_env,jvmtiEnv* jvmtiEnv,JavaVM *vm,int type, const char* log); // 上报日志
void jvmtiReportWithP( JvmtiReportParam* param);