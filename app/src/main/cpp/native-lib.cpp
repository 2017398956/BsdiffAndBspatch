#include <jni.h>
#include <string>
#include <iostream>
#include <android/log.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

#define TAG "NFL" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

void *Shell() {
    LOGD("This is shell in new thread.");
    if (true) {
        return nullptr;
    }
    char shell[64];
    //fork拷贝
    sprintf(shell, "sh /data/data/com.example.jni/start.sh");
    system(shell);
    return nullptr;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_nfl_bsdiffandbspatch_JniTestActivity_stringFromJNI(
        JNIEnv *env,
        jobject) {
    std::string hello = "Hello from C++";
    LOGI("This is an android log from c");

    pid_t fpid = fork();
    if (fpid < 0) {
        LOGE("fork process exception.");
    } else if (fpid == 0) {
        LOGI("This is working in children process.");
        std::string className = "com.example.myapplication.test.MainTest";
        jstring slashClassName = env->NewStringUTF("com/example/myapplication/test/MainTest");
        jboolean isCopy = true;
        jclass mainClass = env->FindClass(env->GetStringUTFChars(slashClassName, &isCopy));
        LOGI("find class %p", env);
        LOGI("find class %p,%p", mainClass, &mainClass);
        if (mainClass == nullptr) {
            LOGE("Can not find main class.");
        } else {
            LOGI("find main class.");
            jmethodID mainMethod = env->GetStaticMethodID(mainClass, "main",
                                                          "([Ljava/lang/String;)V");
            if (mainMethod == nullptr) {
                LOGE("Can not find main method.");
            } else {
                env->CallStaticVoidMethod(mainClass, mainMethod, NULL);
            }
        }
        exit(0);
    } else {
        if (execl("ps", "ps", "-l", NULL) == -1) {
            LOGI("execl failed!");
        } else {
            LOGI("execl success!");
        }
        //创建线程id
        pthread_t tid;
        //启动线程
        pthread_create(
                &tid,
                nullptr,
                reinterpret_cast<void *(*)(void *)>(Shell),
                nullptr
        );
        // execl("/data/local/tmp/myprocess", nullptr);
        LOGI("This is working in main process.");
    }
    return env->NewStringUTF(hello.c_str());
}

//static JNINativeMethod gMethods[] = {
//        // 方法名称         各个参数的类型                             java 中的方法
//        {"stringFromJNI" , "(Ljava/lang/String;Ljava/lang/String)Ljava/lang/String;" , (void*) com_nfl_bsdiffandbspatch_MainActivity_stringFromJNI }
//};
//
//static int registerNativeMethods(JNIEnv* env, const char* className,JNINativeMethod* getMethods,int methodsNum){
//    jclass clazz;
//    //找到声明native方法的类
//    clazz = env->FindClass(className);
//    if(clazz == NULL){
//        return JNI_FALSE;
//    }
//    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
//    if(env->RegisterNatives(clazz,getMethods,methodsNum) < 0){
//        return JNI_FALSE;
//    }
//    return JNI_TRUE;
//}
//
//// 注册 JNINativeMethod 数组
//static int register_com_nfl_bsdiffandbspatch_MainActivity_stringFromJNI(JNIEnv* env){
//    //指定类的路径，通过FindClass 方法来找到对应的类
//    const char* className  = "com/nfl/bsdiffandbspatch/MainActivity";
//    return registerNativeMethods(env,className,gMethods, sizeof(gMethods)/ sizeof(gMethods[0]));
//}
//
////回调函数
//JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
//    JNIEnv* env = NULL;
//    //获取JNIEnv
//    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
//        return -1;
//    }
//    assert(env != NULL);
//    //注册函数 registerNatives ->registerNativeMethods ->env->RegisterNatives
//    if(!register_com_nfl_bsdiffandbspatch_MainActivity_stringFromJNI(env)){
//        return -1;
//    }
//    //返回jni 的版本
//    return JNI_VERSION_1_6;
//}