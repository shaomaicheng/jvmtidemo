//
// Created by chenglei on 2024/2/24.
//

#include "jvmti.h"
#include <jni.h>

#ifndef JVMTIDMEO_COMMON_H
#define JVMTIDMEO_COMMON_H

#endif //JVMTIDMEO_COMMON_H
extern jvmtiEnv *g_jvmti;
extern char *logDirPath;
extern JavaVM *g_vm;

const int JVMTI_ALLOC_MEMORY = 0;
const int JVMTI_FREE_MEMORY = 1;
const int JVMTI_GC_START = 2;
const int JVMTI_GC_END = 3;
