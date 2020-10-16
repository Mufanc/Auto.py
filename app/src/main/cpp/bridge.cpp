#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_mfc_autopy__1Automaton_runPython(JNIEnv *env, jclass obj) {
    // 获取类字节码
    jclass mClass = env -> FindClass("mfc/automator/Automaton");
    // 获取方法ID
    // javap -p [类路径]  获取方法签名
    jmethodID mID = env -> GetStaticMethodID(mClass, "callback", "(Ljava/lang/String;)V");
    // 实例化该类
    // jobject mObject = (*env)->AllocObject(env, mClass);
    // 调用方法
    env -> CallStaticVoidMethod(obj, mID, env -> NewStringUTF("Callback to java"));
}
