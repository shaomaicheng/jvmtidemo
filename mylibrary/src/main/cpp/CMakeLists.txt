cmake_minimum_required(VERSION 3.4.1)

set(FILE_DIR entry.cpp jvmti.h thread_utils.h)

include_directories("${CMAKE_SOURCE_DIR}")
#aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)


add_library( # Specifies the name of the library.
        jvmtidemo

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        entry.cpp
        thread_utils.h
        jvmti.h
        jnicall.cpp
        jnicall.h
        ${FILE_DIR}
        )
add_library(include SHARED IMPORTED)


find_library(
        log-lib
        log
)





target_link_libraries(
        jvmtidemo
        ${log-lib}
)


include_directories(

)