//
// Created by chenglei on 2024/2/24.
//

#include "memory.h"
#include <string.h>
#include <android/log.h>
#include <iosfwd>
#include "jvmti.h"
#include "sstream"
#include <unordered_map>
#include "pthread.h"

using namespace std;

long tag = 0;

std::unordered_map<long, char *> clazzMap = {};
std::mutex mtx;


void JNICALL objectAlloc
        (jvmtiEnv *jvmti_env,
         JNIEnv *jni_env,
         jthread thread,
         jobject object,
         jclass object_klass,
         jlong size) {
    char *classSignature = nullptr;
    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);
    ostringstream ss;
    if (std::strstr(classSignature, "com/example/jvmtidmeo")) {
        ss << "object Alloc:" << classSignature << ";tag:" << tag;
        string logcpp = ss.str();
        const char *log = logcpp.c_str();
        // 反过来调用java做上报有难度
//        jvmtiReport(jni_env, jvmti, g_vm, JVMTI_ALLOC_MEMORY, log);
        mtx.lock();
        jvmti_env->SetTag(object, tag);
        clazzMap[tag] = classSignature;
        tag++;
        mtx.unlock();
    }

}


void JNICALL objectFree(jvmtiEnv *jvmti_env,
                        jlong tag) {
    if (clazzMap.count(tag) > 0) {
        ostringstream ss;
        mtx.lock();
        ss << "free:" << clazzMap[tag] << "; tag: " << tag;
        const char *log = ss.str().c_str();
        clazzMap.erase(tag);
        mtx.unlock();
        // 反过来调用java做上报有难度
        /*JvmtiReportParam *p = new JvmtiReportParam;
        p->jvmtiEnv = jvmti;
        p->vm = g_vm;
        p->type = JVMTI_FREE_MEMORY;
        p->log = log;
        jvmtiReportWithP(p);*/
    }
}


void JNICALL gcStart(jvmtiEnv *jvmti_env) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "gc Start");
    // 反过来调用java做上报有难度
    /*JvmtiReportParam *p = new JvmtiReportParam;
    p->jvmtiEnv = jvmti;
    p->vm = g_vm;
    p->type = JVMTI_GC_START;
    p->log = "gc_start";
    jvmtiReportWithP(p);*/

}

void JNICALL gcFinish(jvmtiEnv *jvmti_env) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "gc Finish");
    // 反过来调用java做上报有难度
    /*
    JvmtiReportParam *p = new JvmtiReportParam;
    p->jvmtiEnv = jvmti;
    p->vm = g_vm;
    p->type = JVMTI_GC_END;
    p->log = "gc_finish";
    jvmtiReportWithP(p);*/

}