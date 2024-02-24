//
// Created by chenglei on 2024/2/24.
//


#include "recordlog.h"
#include "common.h"
#include <unordered_map>
#include "mutex"
#include <sys/time.h>
#include <android/log.h>
#include <fcntl.h>
#include <unistd.h>
#include "string"
#include "iostream"
#include "sys/mman.h"

std::unordered_map<int, char *> logFileMap = {};
std::unordered_map<int, long> currentSizeMap = {};
std::unordered_map<int, long> sizeMap = {};
std::unordered_map<int, char*> pointMap={};
std::mutex logMtx;
int m_size = 1024;

const int logtype_memory = 1;

long long currentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void resize(int fd, int type, int needSize)
{
    int oldSize = sizeMap[type];
    do {
        sizeMap[type] += m_size;
        __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "扩容过程:页大小：%ld", sizeMap[type]);
    } while (sizeMap[type] < needSize); // 扩容
    ftruncate(fd, sizeMap[type]);
    if (pointMap[type] != nullptr) {
        munmap(pointMap[type], oldSize);
    }
    __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "mmap参数,size:%ld", sizeMap[type]);
    pointMap[type] = static_cast<char *>(mmap(0, sizeMap[type], PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
}

void record_jvmti_log(jvmtiEnv *jvmtiEnv, int type, const char* log)
{
    char* realLog = static_cast<char *>(malloc(strlen(log) + 1));
    strcat(realLog,"\n");
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
    int fd = open(mmapFilePath, O_RDWR | O_CREAT, 0664);
    if (fd == -1)
    {
        perror("open");
        __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "文件打开失败");
    } else
    {
        if (currentSizeMap.count(log_type_temp) == 0)
        {
            // 没写入过
            __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "没写入过，初始化大小");
            currentSizeMap[log_type_temp] = 0;
            sizeMap[log_type_temp] = 0;
        }
        int currentSize = currentSizeMap[log_type_temp]; // 当前写入的大小，每次写入更新
        int datalen = sizeof(realLog); // 写入大小
        if (currentSize + datalen >= sizeMap[log_type_temp])
        {
            __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "currentSize：%d,datalen:%d,sizeMap:%ld,去扩容", currentSize,datalen,sizeMap[log_type_temp]);
            // 页大小不够，扩容
            resize(fd,log_type_temp, currentSize + datalen);
        }
        // 写入文件
        char* ptr = pointMap[log_type_temp];
        __android_log_print(ANDROID_LOG_ERROR,"chenglei_jni", "memcpy参数:offset:%d,datalen:%d", currentSize,datalen);
        memcpy(ptr+currentSize,realLog,datalen);
        currentSizeMap[log_type_temp] += datalen;
    }
    logMtx.unlock();
}
