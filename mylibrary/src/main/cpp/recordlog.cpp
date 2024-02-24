//
// Created by chenglei on 2024/2/24.
//


#include "recordlog.h"
#include "common.h"
#include <unordered_map>
#include "mutex"
#include <sys/time.h>
#include <android/log.h>
#include "string"

std::unordered_map<int, char *> logFileMap = {};
std::mutex logMtx;

const int logtype_memory = 1;

long long currentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void record_jvmti_log(jvmtiEnv *jvmtiEnv, int type, const char* log)
{
    int log_type_temp;
    switch (type) {
        case JVMTI_ALLOC_MEMORY:
        case JVMTI_FREE_MEMORY:
        case JVMTI_GC_START:
        case JVMTI_GC_END:
            log_type_temp = logtype_memory;
            break;
//        default:
//            break;
    }
    logMtx.lock();
    char* mmapFilePath;
    char* file_path = logFileMap[log_type_temp];
    if (file_path == nullptr) {
        // 每个类型的文件在每次运行的时候只有一个
        char *file_type;
        switch (log_type_temp) {
            case logtype_memory:

                const char *temp = "memory_";
                file_type = static_cast<char *>(malloc(strlen(temp)));
                stpcpy(file_type, temp);
                break;
//        default:
//            break;
        }
        long long timestemp = currentTime();
        const char *timestemp_str = std::to_string(timestemp).c_str();
        const char *stufix = ".log";
        const char *split = "/";
        char* ret = static_cast<char *>(malloc(strlen(logDirPath)));
        strcpy(ret, logDirPath);
        char *file_name = strcat(file_type, timestemp_str);
        char *file_name_seg = strcat(ret, split);
        char *file_full_seg = strcat(file_name_seg, file_name);
        char *full_file_name = strcat(file_full_seg, stufix);
        mmapFilePath= static_cast<char *>(malloc(strlen(full_file_name)));
        logFileMap[log_type_temp] = mmapFilePath;
        __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "创建文件:%s", full_file_name);
        strcpy(mmapFilePath, full_file_name);
    } else {
        mmapFilePath= static_cast<char *>(malloc(strlen(file_path)));
        strcpy(mmapFilePath, file_path);
    }
    // 写入文件
    __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "写入文件:%s", mmapFilePath);
    logMtx.unlock();
}