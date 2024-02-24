//
// Created by chenglei on 2024/2/24.
//

#include "jvmti.h"
#include <jni.h>

#ifndef JVMTIDMEO_COMMON_H
#define JVMTIDMEO_COMMON_H

#endif //JVMTIDMEO_COMMON_H
jvmtiEnv *g_jvmti = nullptr;
jstring logDirPath;
JavaVM *g_vm;