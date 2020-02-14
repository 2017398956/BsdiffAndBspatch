#include <jni.h>
#include <string>
#include <iostream>

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