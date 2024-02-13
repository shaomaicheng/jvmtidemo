#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <iosfwd>
#include "jvmti.h"
#include "sstream"
#include <unordered_map>
#include "thread_utils.h"


using namespace std;

//jvmtiEnv* mJvmtiEnv;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_mylibrary_JvmtiConfig_demo(JNIEnv *env, jclass clazz) {
}


jvmtiEnv  *jvmti = nullptr;

std::unordered_map<long, char*> clazzMap = {};
std::mutex mtx;





long tag = 0;
void JNICALL objectAlloc
        (jvmtiEnv *jvmti_env,
         JNIEnv* jni_env,
         jthread thread,
         jobject object,
         jclass object_klass,
         jlong size)
{
    char* classSignature= nullptr;
    jvmti_env->GetClassSignature(object_klass, &classSignature, nullptr);
    ostringstream  ss;
    //collect MemoryFreeTestItem
    if (std::strstr(classSignature,"com/example/jvmtidmeo")) {
        mtx.lock();
        jvmti_env->SetTag(object, tag);
        ss << "object Alloc:" << classSignature << ";tag:"<<tag;
        string logcpp = ss.str();
        const char* log = logcpp.c_str();
        // todo
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "%s", log);
        clazzMap[tag] = classSignature;
        tag++;
        mtx.unlock();
    }

}


 void JNICALL objectFree(jvmtiEnv *jvmti_env,
         jlong tag)
{
//    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "objectFree,tag is %ld", tag);
    if (clazzMap.count(tag) > 0) {
        ostringstream ss;
        mtx.lock();
        ss << "free:" << clazzMap[tag] << "; tag: " << tag;
        const char* log = ss.str().c_str();
        clazzMap.erase(tag);
        mtx.unlock();
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "%s", log);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "clazzMap内不存在%ld", tag);
    }
}


void JNICALL gcStart(jvmtiEnv *jvmti_env) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "gc Start");
}

void JNICALL gcFinish(jvmtiEnv *jvmti_env){
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "gc Finish");
}

extern "C"
JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM* vm, char* options, void* reserved) {
    __android_log_print(ANDROID_LOG_ERROR, "chenglei", "Agent OnAttacth");
    jint getEnv = vm->GetEnv(reinterpret_cast<void **>(&jvmti), JVMTI_VERSION_1_2); // todo jvmti_version?
    if (getEnv != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "chenglei", "jvmti设置代理失败！");
        return JNI_ERR;
    }



    jvmtiCapabilities caps;
    jvmti->GetPotentialCapabilities(&caps);
    jvmti->AddCapabilities(&caps);

    jvmti->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_OBJECT_ALLOC, nullptr);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_OBJECT_FREE, nullptr);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, nullptr);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, nullptr);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.VMObjectAlloc = &objectAlloc;
    callbacks.ObjectFree = &objectFree;
    callbacks.GarbageCollectionStart = &gcStart;
    callbacks.GarbageCollectionFinish = &gcFinish;

    if (jvmti != nullptr) {
        jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    }



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