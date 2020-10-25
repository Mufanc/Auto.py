////  g++ bridge.cpp -I $PREFIX/include/python3.8/ -l python3.8 ...
//
//#include <set>
//#include <map>
//#include <jni.h>
//#include <memory>
//#include <vector>
//#include <sstream>
//#include <utility>
//#include <typeinfo>
//#include <iostream>
//#include <Python.h>
//
//
///**
// * 定义 index 类，使其支持 Python
// */
//typedef struct {
//    PyObject_HEAD
//    int index = 0;
//} PyIndexObject;
//
//static PyObject *PyIndex_str(PyObject *obj) {
//    int index = ((PyIndexObject *) obj) -> index;
//    std::stringstream sstream;
//    sstream << "index:[" << index << "]";
//    return Py_BuildValue("s", sstream.str().c_str());
//}
//
//PyObject *PyIndex_index(PyObject *self, void *closure) {
//    PyIndexObject *obj = (PyIndexObject *) self;
//    return Py_BuildValue("i", obj -> index);
//}
//
//static PyGetSetDef IndexGetSet[] = {
//        {"index", (getter) PyIndex_index, nullptr, nullptr},
//        {nullptr}
//};
//
//PyObject *PyIndex_init(PyObject *self, PyObject *args) {
//    int index;
//    PyArg_ParseTuple(args, "i", &index);
//    ((PyIndexObject *) self) -> index = index;
//    // Todo: 这里自己琢磨出来的，不懂有没有问题
//    Py_RETURN_NONE;
//}
//
//static PyTypeObject IndexType = {
//        PyVarObject_HEAD_INIT(nullptr, 0)
//        "bridge.index",                                    /* tp_name */
//        sizeof(PyIndexObject),                             /* tp_basicsize */
//        0,                                                 /* tp_itemsize */
//        nullptr,                                           /* tp_dealloc */
//        (Py_ssize_t) nullptr,                              /* tp_print */
//        // Todo: 这里直接强行赋 nullptr，可能会出问题
//        nullptr,                                           /* tp_getattr */
//        nullptr,                                           /* tp_setattr */
//        nullptr,                                           /* tp_reserved */
//        nullptr,                                           /* tp_repr */
//        nullptr,                                           /* tp_as_number */
//        nullptr,                                           /* tp_as_sequence */
//        nullptr,                                           /* tp_as_mapping */
//        nullptr,                                           /* tp_hash  */
//        nullptr,                                           /* tp_call */
//        PyIndex_str,                                       /* tp_str */
//        nullptr,                                           /* tp_getattro */
//        nullptr,                                           /* tp_setattro */
//        nullptr,                                           /* tp_as_buffer */
//        Py_TPFLAGS_DEFAULT,          /* tp_flags */
//        "Custom index of object heap.",                    /* tp_doc */
//        nullptr,                                           /* tp_traverse */
//        nullptr,                                           /* tp_clear */
//        nullptr,                                           /* tp_richcompare */
//        0,                                                 /* tp_weaklistoffset */
//        nullptr,                                           /* tp_iter */
//        nullptr,                                           /* tp_iternext */
//        nullptr,                                           /* tp_methods */
//        nullptr,                                           /* tp_members */
//        IndexGetSet,                                       /* tp_getset */
//        nullptr,                                           /* tp_base */
//        nullptr,                                           /* tp_dict */
//        nullptr,                                           /* tp_descr_get */
//        nullptr,                                           /* tp_descr_set */
//        0,                                                 /* tp_dictoffset */
//        (initproc) PyIndex_init,                           /* tp_init */
//        nullptr,                                           /* tp_alloc */
//        nullptr                                            /* tp_new */
//};
//
//
///**
// * 实现自动分配下标存储的数据结构
// */
//class heap {
//private:
//    std::vector<jvalue> data;
//    std::set<int> index;
//public:
//    heap () {
//        index.insert(0);
//    }
//    PyObject *put(jvalue item) {
//        auto begin = index.begin(), end = prev(index.end());
//        if (begin == end) {
//            data.push_back(item);
//            index.insert(*end + 1);
//        } else {
//            data[*begin] = item;
//        }
//        int first = *begin;
//        index.erase(begin);
//        PyIndexObject *ret = PyObject_NEW(PyIndexObject, &IndexType);
//        ret -> index = first;
//        return (PyObject *) ret;
//    }
//    jvalue get(PyObject *obj) {
//        return data[((PyIndexObject *) obj) -> index];
//    }
//    void erase(PyObject *obj) {
//        index.insert(((PyIndexObject *) obj) -> index);
//    }
//};
//
//heap mHeap;
//JNIEnv *javaEnv;
//
//
///**
// * 将 Python 对象转换为 Java 对象
// * @param obj: Python对象
// * @return 转换出的 Java 对象
// */
//jvalue objectPython2Java(PyObject *obj) {
//    // 这里把 jlong 的空间赋 0 是为了方便之后转回 Python 类型
//    jvalue result; result.j = 0;
//    if (obj -> ob_type == PyLong_Type.tp_base) {
//        if (PyLong_AsLongLong(obj) > INT_MAX) {
//            result.j = (jlong) PyLong_AsLongLong(obj);
//        } else {
//            result.i = (jint) PyLong_AsLong(obj);
//        }
//    } else if (obj -> ob_type == PyUnicode_Type.tp_base) {
//        result.l = javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
//    } else if (obj -> ob_type == PyFloat_Type.tp_base) {
//        result.d = PyFloat_AsDouble(obj);
//    } else if (obj -> ob_type == &IndexType) {
//        // Todo: 这里的类型判断也是自己猜的，不懂会不会出问题
//        return mHeap.get(obj);
//    } else {
//        PyErr_SetString(PyExc_ValueError, "objectPython2Java: 不支持这个类型");
//    }
//    // Todo: 支持更多的类型，遇到不支持的类型时抛出异常
//    return result;
//}
//
//
///**
// * 将 Java 字符串转换为 C++ 字节指针
// * @param jstr Java 字符串
// * @param cstr 待改变的 char 二级指针
// */
//void jstring2char_p(jstring jstr, char **cstr) {
//    jclass clsstring = javaEnv -> FindClass("java/lang/String");
//    jmethodID mid = javaEnv -> GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
//    jbyteArray barr= (jbyteArray) javaEnv -> CallObjectMethod(jstr, mid, javaEnv -> NewStringUTF("utf-8"));
//    jsize length = javaEnv -> GetArrayLength(barr);
//    jbyte* barray = javaEnv -> GetByteArrayElements(barr, JNI_FALSE);
//    if (length > 0) {
//        *cstr = (char*) malloc(length + 1);
//        memcpy(*cstr, barray, length);
//        *cstr[length] = 0;
//    }
//}
//
//
///**
// * 将 Java 对象转换为指定 Python 对象
// * @param obj: Java对象
// * @param type: 期望的类型
// * @return 转换后的 Python 对象
// */
//PyObject *objectJava2Python(jvalue obj, PyTypeObject *type) {
//    if (type == PyLong_Type.tp_base) {
//        return PyLong_FromLongLong((long long) obj.j);
//    } else if (type == PyUnicode_Type.tp_base) {
//        char* str = NULL;
//        // Todo: 这里强制转换 jstring，可能会出问题
//        jstring2char_p((jstring) obj.l, &str);
//        return PyUnicode_FromString(str);
//    } else if (type == PyFloat_Type.tp_base) {
//        return PyFloat_FromDouble((double) obj.d);
//    }
//}
//
//
///**
// * 将一个 Python 对象转换成 Java 对象，并存储到堆中
// * 支持 str, int, float, bytes?, tuple/list/dict?
// * @param self
// * @param obj: Python 对象
// * @return 存储在堆中的 id (索引)
// */
//static PyObject *saveValueFromPython(PyObject *self, PyObject *args) {
//    PyObject *obj;
//    PyArg_ParseTuple(args, "O", &obj);
//    return mHeap.put(objectPython2Java(obj));
//}
//
//
///**
// * 依照给出的 id，从堆中获取一个对象，并转换成指定类型返回给 Python
// * @param self
// * @param id: 对象的索引
// * @param type
// * @return 取出的 Python 对象
// */
//static PyObject *getValueFromHeap(PyObject *self, PyObject *args) {
//    PyObject *id, *tmp;
//    PyArg_ParseTuple(args, "iO", &id, &tmp);
//    // Todo: 这里强行将 PyObject 转换为 PyTypeObject，可能会出问题
//    PyTypeObject *type = (PyTypeObject *) tmp;
//    return objectJava2Python(mHeap.get(id), type);
//}
//
//
///**
// * 依照给出的 id，在堆中删去对应的对象
// * @param self
// * @param id: 对象的索引
// */
//static PyObject *removeValue(PyObject *self, PyObject *args) {
//    PyObject *id;
//    PyArg_ParseTuple(args, "i", &id);
//    mHeap.erase(id);
//    Py_RETURN_NONE;
//}
//
//
///**
// * 实例化某个类，并将其存储到堆中，返回其索引
// * @param self
// * @param args 类名，方法签名, 构造函数的参数元组
// * @return 对象的索引
// */
//static PyObject *getInstance(PyObject *self, PyObject *args) {
//    PyObject *cname, *csign, *cargs;
//    PyArg_ParseTuple(args, "OOO", &cname, &csign, &cargs);
//    Py_ssize_t length = PyTuple_Size(cargs);
//
//    jvalue *jargs = new jvalue[length];
//    PyTupleObject *tuple = (PyTupleObject *) cargs;
//    for (Py_ssize_t i = 0; i < length; i++) {
//        jargs[i] = objectPython2Java(tuple -> ob_item[i]);
//    }
//
//    const char *jname = PyUnicode_AsUTF8(cname);
//    const char *jsign = PyUnicode_AsUTF8(csign);
//    jclass mClass = javaEnv -> FindClass(jname);
//    jmethodID mId = javaEnv -> GetMethodID(mClass, "<init>", jsign);
//    jvalue result;
//    result.l = javaEnv -> NewObjectA(mClass, mId, jargs);
//    return mHeap.put(result);
//}
//
//
///**
// * 调用某个对象的某个方法
// * @param self
// * @param args 对象，方法签名，参数元组
// * @return 返回值的索引
// */
//static PyObject *callMethod(PyObject *self, PyObject *args) {
//    PyObject *cobj, *csign, *cargs;
//    PyArg_ParseTuple(args, "OOO", &cobj, &csign, &cargs);
//    Py_ssize_t length = PyTuple_Size(cargs);
//
//    jvalue *jargs = new jvalue[length];
//    PyTupleObject *tuple = (PyTupleObject *) cargs;
//    for (Py_ssize_t i = 0; i < length; i++) {
//        jargs[i] = objectPython2Java(tuple -> ob_item[i]);
//    }
//
//    javaEnv -> Call
//}
//
//
///**
// * 实现模块: bridge 提供各种 Python 与 Java 交互的函数
// */
//static PyMethodDef bridgeMethods[] = {
//        {"save_object", saveValueFromPython, METH_VARARGS, "将一个 Python 对象转换成 Java 对象，并存储到堆中"},
//        {"get_object", getValueFromHeap, METH_VARARGS, "依照给出的 id，从堆中获取一个对象，并转换成指定类型返回给 Python"},
//        {"remove_object", removeValue, METH_VARARGS, "依照给出的 id，在堆中删去对应的对象"},
//        {NULL, NULL, 0, NULL}
//};
//
//static struct PyModuleDef bridgeModule = {
//        PyModuleDef_HEAD_INIT,
//        "bridge",
//        "为 Android 自动化提供 Python 接口",
//        -1,
//        bridgeMethods
//};
//
//// Todo: 这玩意在 Android Studio 报错，在 Termux 编译的时候应该没问题...
//PyMODINIT_FUNC PyInit_bridge(void) {
//    return PyModule_Create(&bridgeModule);
//}
//
//
///**
// * 通过 JNI 调用此函数，启动 Python 解释器
// * 从 Java 传递一个文件名，根据此文件名运行 Python 脚本
// */
//extern "C"
//JNIEXPORT void JNICALL
//Java_mfc_automator_Automaton_runPython(JNIEnv *env, jobject obj, jstring filename) {
//    javaEnv = env;
//
//    PyImport_AppendInittab("bridge", PyInit_bridge);
//    Py_Initialize();
//    char *fname;
//    jstring2char_p(filename, &fname);
//    FILE *fp = fopen(fname, "r");
//    PyRun_SimpleFile(fp, fname);
//    Py_Finalize();
//    /**
//    // 获取类字节码
//    jclass mClass = env -> FindClass("mfc/automator/Automaton");
//    // 获取方法ID
//    // javap -p [类路径]  获取方法签名
//    jmethodID mID = env -> GetStaticMethodID(mClass, "callback", "(Ljava/lang/String;)V");
//    // 实例化该类
//    // jobject mObject = (*env)->AllocObject(env, mClass);
//    // 调用方法
//    env -> CallStaticVoidMethod(obj, mID, env -> NewStringUTF("Callback to java"));
//     */
//}