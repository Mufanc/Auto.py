//  g++ bridge.cpp -I $PREFIX/include/python3.8/ -l python3.8 -fPIC -shared -o libbridge.so

#include <set>
#include <map>
#include <jni.h>
#include <memory>
#include <vector>
#include <sstream>
#include <utility>
#include <typeinfo>
//#include <iostream>
#include <Python.h>


/**
 * 定义 index 类，使其支持 Python
 */
typedef struct {
    PyObject_HEAD
    int index = 0;
} PyIndexObject;

static PyObject *PyIndex_str(PyObject *obj) {
    int index = ((PyIndexObject *) obj) -> index;
    std::stringstream sstream;
    sstream << "index:[" << index << "]";
    return Py_BuildValue("s", sstream.str().c_str());
}

PyObject *PyIndex_index(PyObject *self, void *closure) {
    PyIndexObject *obj = (PyIndexObject *) self;
    return Py_BuildValue("i", obj -> index);
}

static PyGetSetDef IndexGetSet[] = {
        {"index", (getter) PyIndex_index, nullptr, nullptr},
        {nullptr}
};

PyObject *PyIndex_init(PyObject *self, PyObject *args) {
    int index;
    PyArg_ParseTuple(args, "i", &index);
    ((PyIndexObject *) self) -> index = index;
    // Todo: 这里自己琢磨出来的，不懂有没有问题
    Py_RETURN_NONE;
}

static PyTypeObject IndexType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "bridge.index",                                    /* tp_name */
        sizeof(PyIndexObject),                             /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        nullptr,                                           /* tp_dealloc */
        (Py_ssize_t) nullptr,                              /* tp_print */
        // Todo: 这里直接强行赋 nullptr，可能会出问题
        nullptr,                                           /* tp_getattr */
        nullptr,                                           /* tp_setattr */
        nullptr,                                           /* tp_reserved */
        nullptr,                                           /* tp_repr */
        nullptr,                                           /* tp_as_number */
        nullptr,                                           /* tp_as_sequence */
        nullptr,                                           /* tp_as_mapping */
        nullptr,                                           /* tp_hash  */
        nullptr,                                           /* tp_call */
        PyIndex_str,                                       /* tp_str */
        nullptr,                                           /* tp_getattro */
        nullptr,                                           /* tp_setattro */
        nullptr,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,          /* tp_flags */
        "Custom index of object heap.",                    /* tp_doc */
        nullptr,                                           /* tp_traverse */
        nullptr,                                           /* tp_clear */
        nullptr,                                           /* tp_richcompare */
        0,                                                 /* tp_weaklistoffset */
        nullptr,                                           /* tp_iter */
        nullptr,                                           /* tp_iternext */
        nullptr,                                           /* tp_methods */
        nullptr,                                           /* tp_members */
        IndexGetSet,                                       /* tp_getset */
        nullptr,                                           /* tp_base */
        nullptr,                                           /* tp_dict */
        nullptr,                                           /* tp_descr_get */
        nullptr,                                           /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        (initproc) PyIndex_init,                           /* tp_init */
        nullptr,                                           /* tp_alloc */
        nullptr                                            /* tp_new */
};


/**
 * 实现自动分配下标存储的数据结构
 */
class heap {
private:
    std::vector<jobject> data;
    std::set<int> index;
public:
    heap () {
        index.insert(0);
    }
    PyObject *put(jobject item) {
        auto begin = index.begin(), end = prev(index.end());
        if (begin == end) {
            data.push_back(item);
            index.insert(*end + 1);
        } else {
            data[*begin] = item;
        }
        int first = *begin;
        index.erase(begin);
        PyIndexObject *ret = PyObject_NEW(PyIndexObject, &IndexType);
        ret -> index = first;
        return (PyObject *) ret;
    }
    jobject get(PyObject *obj) {
        return data[((PyIndexObject *) obj) -> index];
    }
    void erase(PyObject *obj) {
        index.insert(((PyIndexObject *) obj) -> index);
    }
};

heap mHeap;
JNIEnv *javaEnv;
jclass IntegerClass, BooleanClass, DoubleClass, LongClass, StringClass;


/**
 * 根据指定的类名、方法签名、参数指针创建类的实例
 * @param name 类名
 * @param sign 构造方法的签名
 * @param args 参数列表
 * @return
 */
jobject newInstance(const char *name, const char *sign, const jvalue* args) {
    jclass mClass = javaEnv -> FindClass(name);
    jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
    return javaEnv -> NewObjectA(mClass, mID, args);
}

jobject newInstance(jclass mClass, const char *sign, const jvalue* args) {
    jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
    return javaEnv -> NewObjectA(mClass, mID, args);
}


/**
 * 将 Python 对象转换为 Java 对象
 * @param obj: Python对象
 * @return 转换出的 Java 对象
 */
jobject objectPython2Java(PyObject *obj) {
    if (obj -> ob_type == PyLong_Type.tp_base) {
        if (PyLong_AsLongLong(obj) > INT_MAX) {
            jvalue args[1];
            args[0].j = PyLong_AsLongLong(obj);
            return newInstance(LongClass, "(J)V", args);
        } else {
            jvalue args[1];
            args[0].j = PyLong_AsLong(obj);
            return newInstance(IntegerClass, "(I)V", args);
        }
    } else if (obj -> ob_type == PyUnicode_Type.tp_base) {
        return javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
    } else if (obj -> ob_type == PyFloat_Type.tp_base) {
        jvalue args[1];
        args[0].j = PyFloat_AsDouble(obj);
        return newInstance(DoubleClass, "(D)V", args);
    } else if (obj -> ob_type == &IndexType) {
        // Todo: 这里的类型判断也是自己猜的，不懂会不会出问题
        return mHeap.get(obj);
    } else {
        PyErr_SetString(PyExc_ValueError, "objectPython2Java: 不支持这个类型");
    }
    // Todo: 支持更多的类型，遇到不支持的类型时抛出异常
    return NULL;
}


/**
 * 将 Java 字符串转换为 C++ 字节指针
 * @param jstr Java 字符串
 * @param cstr 待改变的 char 二级指针
 */
void jstring2char_p(jstring jstr, char **cstr) {
    jclass clsstring = StringClass;
    jmethodID mid = javaEnv -> GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray) javaEnv -> CallObjectMethod(jstr, mid, javaEnv -> NewStringUTF("utf-8"));
    jsize length = javaEnv -> GetArrayLength(barr);
    jbyte* barray = javaEnv -> GetByteArrayElements(barr, JNI_FALSE);
    if (length > 0) {
        *cstr = (char*) malloc(length + 1);
        memcpy(*cstr, barray, length);
        *cstr[length] = 0;
    }
}


/**
 * 将 Java 对象转换为指定 Python 对象
 * @param obj: Java对象
 * @param type: 期望的类型
 * @return 转换后的 Python 对象
 */
PyObject *objectJava2Python(jobject obj, PyTypeObject *type) {
    if (type == PyLong_Type.tp_base) {
        jmethodID mID = javaEnv -> GetMethodID(javaEnv -> GetObjectClass(obj), "longValue", "()J");
        return PyLong_FromLongLong((long long) javaEnv -> CallLongMethod(obj, mID));
    } else if (type == PyUnicode_Type.tp_base) {
        char* str = NULL;
        jstring2char_p((jstring) obj, &str);
        return PyUnicode_FromString(str);
    } else if (type == PyFloat_Type.tp_base) {
        jmethodID mID = javaEnv -> GetMethodID(DoubleClass, "doubleValue", "()D");
        return PyFloat_FromDouble((double) javaEnv -> CallDoubleMethod(obj, mID));
    } else {
        PyErr_SetString(PyExc_ValueError, "objectPython2Java: 不支持这个类型");
    }
    Py_RETURN_NONE;
}


/**
 * 将一个 Python 对象转换成 Java 对象，并存储到堆中
 * 支持 str, int, float, bytes?, tuple/list/dict?
 * @param self
 * @param obj: Python 对象
 * @return 存储在堆中的 id (索引)
 */
static PyObject *Py_saveValueFromPython(PyObject *self, PyObject *args) {
    PyObject *obj;
    PyArg_ParseTuple(args, "O", &obj);
    return mHeap.put(objectPython2Java(obj));
}


/**
 * 依照给出的 id，从堆中获取一个对象，并转换成指定类型返回给 Python
 * @param self
 * @param id: 对象的索引
 * @param type
 * @return 取出的 Python 对象
 */
static PyObject *Py_getValueFromHeap(PyObject *self, PyObject *args) {
    PyObject *id, *tmp;
    PyArg_ParseTuple(args, "iO", &id, &tmp);
    // Todo: 这里强行将 PyObject 转换为 PyTypeObject，可能会出问题
    PyTypeObject *type = (PyTypeObject *) tmp;
    return objectJava2Python(mHeap.get(id), type);
}


/**
 * 依照给出的 id，在堆中删去对应的对象
 * @param self
 * @param id: 对象的索引
 */
static PyObject *Py_removeValue(PyObject *self, PyObject *args) {
    PyObject *id;
    PyArg_ParseTuple(args, "i", &id);
    mHeap.erase(id);
    Py_RETURN_NONE;
}


/**
 * 实例化某个类，并将其存储到堆中，返回其索引
 * @param self
 * @param args 类名，方法签名, 构造函数的参数元组
 * @return 对象的索引
 */
static PyObject *Py_getInstance(PyObject *self, PyObject *args) {
    PyObject *cname, *csign, *cargs;
    PyArg_ParseTuple(args, "OOO", &cname, &csign, &cargs);
    Py_ssize_t length = PyTuple_Size(cargs);

    jvalue *jargs = new jvalue[length];
    PyTupleObject *tuple = (PyTupleObject *) cargs;
    for (Py_ssize_t i = 0; i < length; i++) {
        jargs[i].l = objectPython2Java(tuple -> ob_item[i]);
    }

    jclass mClass = javaEnv -> FindClass(PyUnicode_AsUTF8(cname));
    jmethodID mId = javaEnv -> GetMethodID(mClass, "<init>", PyUnicode_AsUTF8(csign));
    return mHeap.put(javaEnv -> NewObjectA(mClass, mId, jargs));
}


/**
 * 调用某个对象的某个方法，并将返回值存储到堆中，返回其索引
 * @param self
 * @param args 对象，方法名称，方法签名，参数元组
 * @return 返回值在堆中的索引
 */
static PyObject *Py_callMethod(PyObject *self, PyObject *args) {
    PyObject *cobj, *cname, *csign, *cargs;
    PyArg_ParseTuple(args, "OOOO", &cobj, &cname, &csign, &cargs);
    Py_ssize_t length = PyTuple_Size(cargs);

    jvalue *jargs = new jvalue[length];
    PyTupleObject *tuple = (PyTupleObject *) cargs;
    for (Py_ssize_t i = 0; i < length; i++) {
        jargs[i].l = objectPython2Java(tuple -> ob_item[i]);
    }

    jobject jobj = objectPython2Java(cobj);
    jmethodID mID = javaEnv -> GetMethodID(javaEnv -> GetObjectClass(jobj), PyUnicode_AsUTF8(cname), PyUnicode_AsUTF8(csign));
    return mHeap.put(javaEnv -> CallObjectMethodA(jobj, mID, jargs));
}


/**
 * 调用某个类的某个静态方法，并将返回值存储到堆中，返回其索引
 * @param self
 * @param args 类名，方法名称，方法签名，参数元组
 * @return 返回值在堆中的索引
 */
static PyObject *Py_callStaticMethod(PyObject *self, PyObject *args) {
    PyObject *classname, *cname, *csign, *cargs;
    PyArg_ParseTuple(args, "OOOO", &classname, &cname, &csign, &cargs);
    Py_ssize_t length = PyTuple_Size(cargs);

    jvalue *jargs = new jvalue[length];
    PyTupleObject *tuple = (PyTupleObject *) cargs;
    for (Py_ssize_t i = 0; i < length; i++) {
        jargs[i].l = objectPython2Java(tuple -> ob_item[i]);
    }

    jclass mClass = javaEnv -> FindClass(PyUnicode_AsUTF8(classname));
    jmethodID mID = javaEnv -> GetStaticMethodID(mClass, PyUnicode_AsUTF8(cname), PyUnicode_AsUTF8(csign));
    return mHeap.put(javaEnv -> CallStaticObjectMethodA(mClass, mID, jargs));
}


/**
 * 实现模块: bridge 提供各种 Python 与 Java 交互的函数
 */
static PyMethodDef bridgeMethods[] = {
        {"saveObject", Py_saveValueFromPython, METH_VARARGS, "将一个 Python 对象转换成 Java 对象，并存储到堆中"},
        {"getObject", Py_getValueFromHeap, METH_VARARGS, "依照给出的 id，从堆中获取一个对象，并转换成指定类型返回给 Python"},
        {"removeObject", Py_removeValue, METH_VARARGS, "依照给出的 id，在堆中删去对应的对象"},
        {"getInstance", Py_getInstance, METH_VARARGS, "实例化某个类，并将其存储到堆中，返回其索引"},
        {"callMethod", Py_callMethod, METH_VARARGS, "调用某个对象的某个方法，并将返回值存储到堆中，返回其索引"},
        {"callStaticMethod", Py_callStaticMethod, METH_VARARGS, "调用某个类的某个静态方法，并将返回值存储到堆中，返回其索引"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef bridgeModule = {
        PyModuleDef_HEAD_INIT,
        "bridge",
        "为 Android 自动化提供 Python 接口",
        -1,
        bridgeMethods
};

// Todo: 这玩意在 Android Studio 报错，在 Termux 编译的时候应该没问题...
PyMODINIT_FUNC PyInit_bridge(void) {
    return PyModule_Create(&bridgeModule);
}


/**
 * 通过 JNI 调用此函数，启动 Python 解释器
 * 从 Java 传递一个文件名，根据此文件名运行 Python 脚本
 */
extern "C"
JNIEXPORT void JNICALL
Java_mfc_automator_Automaton_runPython(JNIEnv *env, jobject obj, jstring filename) {
    // 初始化 JNI 环境
    javaEnv = env;
    IntegerClass = javaEnv -> FindClass("java/lang/Integer");
    BooleanClass = javaEnv -> FindClass("java/lang/Boolean");
    DoubleClass = javaEnv -> FindClass("java/lang/Double");
    LongClass = javaEnv -> FindClass("java/lang/Long");
    StringClass = javaEnv -> FindClass("java/lang/String");

    // 初始化 Python 环境
    PyImport_AppendInittab("bridge", PyInit_bridge);
    Py_Initialize();
//    PyRun_SimpleString("print('Hello World!!')");
    char *fname;
    jstring2char_p(filename, &fname);
    FILE *fp = fopen(fname, "r");
    PyRun_SimpleFile(fp, fname);
    Py_Finalize();
}