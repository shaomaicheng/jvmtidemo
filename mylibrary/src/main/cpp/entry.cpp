#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <iosfwd>
#include "jvmti.h"
#include "sstream"


using namespace std;

//jvmtiEnv* mJvmtiEnv;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_mylibrary_JvmtiConfig_demo(JNIEnv *env, jclass clazz) {

}

jvmtiEnv  *jvmti = nullptr;

int tag = 0;
void JNICALL objectAlloc
        (jvmtiEnv *jvmti_env,
         JNIEnv* jni_env,
         jthread thread,
         jobject object,
         jclass object_klass,
         jlong size)
{
    char* classSignature= nullptr;
    jvmtiError  error = jvmti_env->SetTag(object,tag);
    tag+=1;
    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);
    ostringstream  ss;
    ss << "object Alloc:" << classSignature << ";size:"<<size<<";tag:"<<tag;
    const char* log = ss.str().c_str();
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "%s", log);
}


 void JNICALL objectFree
        (jvmtiEnv *jvmti_env,
         jlong tag)
{
    ostringstream ss;
    ss << "free:" << tag;
    const char* log = ss.str().c_str();
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "%s", log);
}


extern "C"
JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM* vm, char* options, void* reserved) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "Agent OnAttacth");
    jint getEnv = vm->GetEnv(reinterpret_cast<void **>(&jvmti), JVMTI_VERSION_1_2);
    if (getEnv != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "jvmti设置代理失败！");
        return JNI_ERR;
    }

    jvmtiCapabilities caps;
    jvmti->GetPotentialCapabilities(&caps);
    jvmti->AddCapabilities(&caps);



    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.ObjectFree = &objectFree;
    callbacks.VMObjectAlloc = &objectAlloc;

    if (jvmti != nullptr) {
        jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    }

    jvmti->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_OBJECT_ALLOC, nullptr);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_OBJECT_FREE, nullptr);

    return 0;
}


JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "Agent_OnLoad");
    return JNI_OK;
}

JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *vm) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "Agent_OnUnload");
}