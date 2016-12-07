//
// Created by fuli.niu on 2016/12/6.
//
#include <jni.h>
#include <string>
#include "bspatch.c"


static const char *classPathName = "nfl/bspatch/BspatchUtil";
extern "C"

jint Java_nfl_bspatch_BspatchUtil_bspatch(JNIEnv *env , jclass cls ,jstring old , jstring now , jstring patch) {
    int argc = 4;
    char * argv[argc];
    argv[0] = "bspatch";
    argv[1] = (char*) env->GetStringUTFChars(old, 0);
    argv[2] = (char*) env->GetStringUTFChars(now, 0);
    argv[3] = (char*) env->GetStringUTFChars(patch, 0);

    int ret = patchMethod(argc, argv);

    env->ReleaseStringUTFChars(old, argv[1]);
    env->ReleaseStringUTFChars(now, argv[2]);
    env->ReleaseStringUTFChars(patch, argv[3]);
    return ret;
}
// 不知道为什么这个方法调用后会崩溃
jstring Java_nfl_bspatch_BspatchUtil_sayHello(JNIEnv *env, jclass cls, jstring str){
    if(NULL != str){
        return str ;
    }
    return env->NewStringUTF("Hello from sayHello");
}

// 以下为动态注册JNI方法
jint add(JNIEnv *env, jobject thiz, jint x, jint y) {
    return x + y;
}

jint substraction(JNIEnv *env, jobject thiz, jint x, jint y) {
    return x - y;
}

jfloat multiplication(JNIEnv *env, jobject thiz, jint x, jint y) {
    return (float) x * (float) y;
}

jfloat division(JNIEnv *env, jobject thiz, jint x, jint y) {
    return (float) x / (float) y;
}

jstring say(JNIEnv *env, jclass thiz, jstring str){
    if(NULL != str){
        return str ;
    }
    return env->NewStringUTF("say");
}
jint testAdd2(JNIEnv *env, jobject thiz, jint x, jint y){
    return x + y;
}
static JNINativeMethod methods[] = {
        {"add",            "(II)I", (void *) add},
        {"substraction",   "(II)I", (void *) substraction},
        {"multiplication", "(II)F", (void *) multiplication},
        {"division",       "(II)F", (void *) division},
        {"testAdd2", "(II)I", (void *) testAdd2},
        {"say", "(Ljava/lang/String;)Ljava/lang/String;", (void *) say},
};

// 存储JNI环境
typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

// 静态注册JNI方法
static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL)
        return JNI_FALSE;
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        return JNI_FALSE;
    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, classPathName,
                               methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

// 当VM执行到System.loadLibrary()时，首先执行C组件里的JNI_OnLoad()。它的用途有二：
// 1. 告诉VM此C组件使用那一个JNI版本。如果你的*.so文件没有提供JNI_OnLoad()，VM会默认该*.so檔是使用最老的JNI 1.1版本。由于新版的JNI做了许多扩充，如果需要使用JNI的新版功能，例如JNI 1.4的java.nio.ByteBuffer, 就必须藉由JNI_OnLoad()来告知VM。
// 2. 由于VM执行到System.loadLibrary()时，就会立即先呼叫JNI_OnLoad()，所以C组件的开发者可以藉由JNI_OnLoad()来进行C组件内的初期值之设定(Initialization)。
// 初始化并返回jni版本，无法获取jni环境时返回-1
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv *env = NULL;
    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        goto bail;// 无法获得jni环境，返回-1
    }
    env = uenv.env;
    if (registerNatives(env) != JNI_TRUE) {
        goto bail;
    }
    result = JNI_VERSION_1_6;
    bail:
    return result;
}

// onUnLoad方法，在JNI组件被释放时调用
void JNI_OnUnload(JavaVM* vm, void* reserved){
}
