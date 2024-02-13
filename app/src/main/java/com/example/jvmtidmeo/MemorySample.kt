package com.example.jvmtidmeo

/**
 * @author chenglei01
 * @date 2024/2/13
 * @time 11:01
 */
object MemorySample {
    private var needclearItems : ArrayList<MemoryFreeTestItem>? = null


    fun alloc() {
        if (needclearItems==null) needclearItems= ArrayList()
        for (i in 0 until 20) {
            needclearItems?.add(MemoryFreeTestItem())
        }
    }

    fun free(){
        needclearItems?.clear()
        needclearItems = null
    }
}