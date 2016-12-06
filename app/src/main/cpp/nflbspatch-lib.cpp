//
// Created by fuli.niu on 2016/12/6.
//
#include <jni.h>
#include <string>

extern "C"
jstring
Java_nfl_bspatch_BspatchUtil_bspatch(
        JNIEnv *env, jclass cls,
        jstring old, jstring now, jstring patch) {

    std::string hello = "Hello from Bspatch";
    return env->NewStringUTF(hello.c_str());
}

