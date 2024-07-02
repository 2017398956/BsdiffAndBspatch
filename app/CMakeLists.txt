# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.22.1)

# 设置工程名称 和版本
project(tutorial VERSION 1.0)

#Mode- 打印消息的类别
#FATAL_ERROR：cmake出错，停止编译和生成(信息红色)
#SEND_ERROR：cmake出错，继续编译，但是停止生成(信息红色)
#WARNING：cmake警告，继续编译(信息红色)
#AUTHOR_WARNING：开发者警告，继续编译(信息红色)
#DEPRECATION：如果使用set方法设置CMAKE_ERROR_DEPRECATED为true(不区分大小写)，编译出错，否则继续编译
#(none) or NOTICE：不设置mode，默认是NOTICE模式，不影响编译和生成，用于打印消息(信息白色)
#STATUS：编译时状态信息，左边以--开头(信息白色)
#DEBUG：针对开发人员的调试信息(信息白色)
#TRACE：日志级别的临时信息(信息白色)

SET(HELLO "Hello World")
MESSAGE(WARNING "Hello = ${HELLO}.")
MESSAGE(WARNING "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
MESSAGE(AUTHOR_WARNING "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
MESSAGE("PCL_FOUND = ${PCL_FOUND}.")
MESSAGE(AUTHOR_WARNING "PCL_INCLUDE_DIRS = ${PCL_INCLUDE_DIRS}.")
MESSAGE(STATUS "PCL_LIBRARIES = ${PCL_LIBRARIES}.")
MESSAGE(STATUS "PCL_LIBRARY_DIRS = ${PCL_LIBRARY_DIRS}.")
MESSAGE(STATUS "PCL_VERSION = ${PCL_VERSION}.")
MESSAGE(STATUS "PCL_COMPONENTS = ${PCL_COMPONENTS}.")
MESSAGE(STATUS "PCL_DEFINITIONS = ${PCL_DEFINITIONS}.")

# 增加生成可执行文件，生成的程序名称为：tutorial_first
#add_executable(tutorial src/tutorial.cpp)

# 为指定项目添加 include 路径
#target_include_directories(tutorial PUBLIC
#        "${PROJECT_BINARY_DIR}"
#)

# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定为C++11 版本
#set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations -Wdeprecated-writable-strings")

# 指定版本号的配置文件
#configure_file(include/TutorialConfig.h.in TutorialConfig.h)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.
add_library(native-lib
        SHARED
        src/main/cpp/native-lib.cpp)

add_library( # Sets the name of the library.
        nflbspatch-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        # Associated headers in the same location as their source
        # file are automatically included.
        src/main/cpp/nflbspatch-lib.cpp
        src/main/cpp/bzip2/blocksort.c
        src/main/cpp/bzip2/bzlib.c
        src/main/cpp/bzip2/compress.c
        src/main/cpp/bzip2/dlltest.c
        src/main/cpp/bzip2/randtable.c
        src/main/cpp/bzip2/bzip2.c
        src/main/cpp/bzip2/crctable.c
        src/main/cpp/bzip2/huffman.c
        src/main/cpp/bzip2/spewG.c
        #        src/main/cpp/bzip2/bzip2recover.c
        src/main/cpp/bzip2/decompress.c
        src/main/cpp/bzip2/unzcrash.c
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries(native-lib
        ${log-lib})

target_link_libraries( # Specifies the target library.
        nflbspatch-lib
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})