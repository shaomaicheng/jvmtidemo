#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <iosfwd>
#include "jvmti.h"
#include "sstream"
#include <unordered_map>
#include "pthread.h"
#include "memory.h"
#include "common.h"


using namespace std;


extern "C"
JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    g_vm = vm;
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "Agent OnAttacth");
    jint getEnv = vm->GetEnv(reinterpret_cast<void **>(&g_jvmti),
                             JVMTI_VERSION_1_2); // todo jvmti_version?
    if (getEnv != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "jvmti设置代理失败！");
        return JNI_ERR;
    }

    JNIEnv* jniEnv;
    vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    jvmtiCapabilities caps;
    g_jvmti->GetPotentialCapabilities(&caps);
    g_jvmti->AddCapabilities(&caps);

    g_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_OBJECT_ALLOC, nullptr);
    g_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_OBJECT_FREE, nullptr);
    g_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, nullptr);
    g_jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, nullptr);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.VMObjectAlloc = &objectAlloc;
    callbacks.ObjectFree = &objectFree;
    callbacks.GarbageCollectionStart = &gcStart;
    callbacks.GarbageCollectionFinish = &gcFinish;

    if (g_jvmti != nullptr) {
        g_jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    }


    return 0;
}


JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "Agent_OnLoad");
    return JNI_OK;
}

JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei_jni", "Agent_OnUnload");
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_mylibrary_JvmtiConfig_demo(JNIEnv *env, jclass clazz, jstring log_dir_path) {
    logDirPath = log_dir_path;
}