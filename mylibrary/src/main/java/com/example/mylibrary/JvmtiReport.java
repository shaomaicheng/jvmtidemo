package com.example.mylibrary;

import android.util.Log;

public class JvmtiReport {

    static void report(int type, String log) {
        String typeStr = "";
        switch (type) {
            case MEMORY_ALLOC:
                typeStr = "内存分配";
                break;
            case MEMORY_FREE:
                typeStr = "内存释放";
                break;
            case GC_START:
                typeStr = "垃圾回收开始";
                break;
            case GC_END:
                typeStr = "垃圾回收结束";
                break;
            default:
                break;
        }

        Log.e("chenglei", "上报类型："+typeStr+";上报内容："+log);
    }

    static final int MEMORY_ALLOC = 0;
    static final int MEMORY_FREE = 1;
    static final int GC_START = 2;
    static final int GC_END = 3;

}
