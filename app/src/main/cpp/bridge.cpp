#include <set>
#include <map>
#include <jni.h>
#include <vector>
#include <Python.h>
#include <iostream>

using std::cout;
using std::endl;


template <class T>
class jHeap {
    private:
        std::vector<T> data;
        std::set<int> index;
    public:
        jHeap () {
            index.insert(0);
        }
        int put(T item) {
            auto begin = index.begin(), end = prev(index.end());
            if (begin == end) {
                data.push_back(item);
                index.insert(*end + 1);
            } else {
                data[*begin] = item;
            }
            int first = *begin;
            index.erase(begin);
            return first;
        }
        T get(int n) {
            return data[n];
        }
        void erase(int n) {
            index.insert(n);
        }
};


jHeap<jobject> heap;
JNIEnv *javaEnv;


jobject objectPython2Java(PyObject *obj) {
    if (obj -> ob_type == PyLong_Type.tp_base) {
        cout << "Wow" << endl;
    } else if (obj -> ob_type == ) {

    }
    return NULL;
}


static PyObject *saveValueFromPython(PyObject *self, PyObject *args) {
    /**
     * 将 Python 的变量转换成 jobject 类型
     * 支持 str, int, float, bytes?, tuple/list/dict?
     */
    char *str, *filename = NULL;
    int bytes_copied = -1;

    /* Parse arguments */

//    if(!PyArg_ParseTuple(args, "ss", &str, &filename)) {
//        return NULL;
//    }
//
//    FILE *fp = fopen(filename, "w");
//    bytes_copied = fputs(str, fp);
//    fclose(fp);

    return (PyObject*) Py_BuildValue("i", heap.put((jobject) 1/* sth */));
}



extern "C"
JNIEXPORT void JNICALL
Java_mfc_automator_Automaton_runPython(JNIEnv *env, jobject obj) {
    javaEnv = env;
    PyObject *mInt = Py_BuildValue("i", 1);
    objectPython2Java(mInt);
    // jclass jobject jmethodID 
    /**
    // 获取类字节码
    jclass mClass = env -> FindClass("mfc/automator/Automaton");
    // 获取方法ID
    // javap -p [类路径]  获取方法签名
    jmethodID mID = env -> GetStaticMethodID(mClass, "callback", "(Ljava/lang/String;)V");
    // 实例化该类
    // jobject mObject = (*env)->AllocObject(env, mClass);
    // 调用方法
    env -> CallStaticVoidMethod(obj, mID, env -> NewStringUTF("Callback to java"));
     */

}