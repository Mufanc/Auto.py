//  g++ bridge.cpp -I $PREFIX/include/python3.8/ -l python3.8 -fPIC -shared -o libbridge.so

#include <jni.h>
#include <Python.h>

#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>

/**
 * 2020-10-27注:
 * 在 PyJavaObject 中调用方法时，一定要注意将
 * Java 类型的返回值转换为 Python 类型的问题
 */

/**
 * 全局变量区
 */
// 基本全局变量
static jclass IntegerClass, BooleanClass, DoubleClass, LongClass, StringClass;
static JNIEnv *javaEnv;

// Python 自定义类型变量
static PyObject *PyJavaObject_CallObjectMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallStringMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallIntMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallLongMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallDoubleMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallBooleanMethod(PyObject *self, PyObject *args);
static PyMethodDef JavaObjectMethods[] = {
        {"call_object_method", (PyCFunction) PyJavaObject_CallObjectMethod, METH_VARARGS, "jniutils::CallObjectMethod"},
        {"call_string_method", (PyCFunction) PyJavaObject_CallStringMethod, METH_VARARGS, "jniutils::CallStringMethod"},
        {"call_int_method", (PyCFunction) PyJavaObject_CallIntMethod, METH_VARARGS, "jniutils::CallIntMethod"},
        {"call_long_method", (PyCFunction) PyJavaObject_CallLongMethod, METH_VARARGS, "jniutils::CallLongMethod"},
        {"call_double_method", (PyCFunction) PyJavaObject_CallDoubleMethod, METH_VARARGS, "jniutils::CallDoubleMethod"},
        {"call_boolean_method", (PyCFunction) PyJavaObject_CallBooleanMethod, METH_VARARGS, "jniutils::CallBooleanMethod"},
        {nullptr}
};

typedef struct {
    PyObject_HEAD
    jobject mObject;
} PyJavaObject;

static PyObject *PyJavaObject_str(PyObject *obj);
static PyObject *PyJavaObject_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);
static PyTypeObject JavaObjectType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "bridgeutils.JavaObject",                          /* tp_name */
        sizeof(PyJavaObject),                              /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        (destructor) nullptr,                              /* tp_dealloc */
        0,                                                 /* tp_print */
        nullptr,                                           /* tp_getattr */
        nullptr,                                           /* tp_setattr */
        nullptr,                                           /* tp_reserved */
        nullptr,                                           /* tp_repr */
        nullptr,                                           /* tp_as_number */
        nullptr,                                           /* tp_as_sequence */
        nullptr,                                           /* tp_as_mapping */
        nullptr,                                           /* tp_hash  */
        nullptr,                                           /* tp_call */
        PyJavaObject_str,                                  /* tp_str */
        nullptr,                                           /* tp_getattro */
        nullptr,                                           /* tp_setattro */
        nullptr,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                /* tp_flags */
        "Adapt a java object in Python.",                  /* tp_doc */
        nullptr,                                           /* tp_traverse */
        nullptr,                                           /* tp_clear */
        nullptr,                                           /* tp_richcompare */
        0,                                                 /* tp_weaklistoffset */
        nullptr,                                           /* tp_iter */
        nullptr,                                           /* tp_iternext */
        JavaObjectMethods,                                 /* tp_methods */
        nullptr,                                           /* tp_members */
        nullptr,                                           /* tp_getset */
        nullptr,                                           /* tp_base */
        nullptr,                                           /* tp_dict */
        nullptr,                                           /* tp_descr_get */
        nullptr,                                           /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        nullptr,                                           /* tp_init */
        nullptr,                                           /* tp_alloc */
        PyJavaObject_new                                   /* tp_new */
};

// Python 扩展模块变量
static struct PyModuleDef PyBridgeModule = {
        PyModuleDef_HEAD_INIT,
        "bridgeutils",
        "Python interface for Automator on Android by app_process",
        -1,
        nullptr
};


/**
 * 与 jni 调用有关的各种函数
 */
class jniutils {
public:
    static jobject newInstance(const char *classname, const char *sign, const jvalue *args) {
        jclass mClass = javaEnv -> FindClass(classname);
        jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
        return javaEnv -> NewObjectA(mClass, mID, args);
    }

    static jobject newInstance(jclass mClass, const char *sign, const jvalue *args) {
        jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
        return javaEnv -> NewObjectA(mClass, mID, args);
    }

    static jobject callObjectMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallObjectMethodA(obj, mID, args);
    }

    static jobject callObjectMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallObjectMethod(obj, mID);
    }

    static jobject callStringMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallObjectMethodA(obj, mID, args);
    }

    static jobject callStringMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallObjectMethod(obj, mID);
    }

    static jint callIntMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallIntMethodA(obj, mID, args);
    }

    static jint callIntMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallIntMethod(obj, mID);
    }

    static jlong callLongMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallLongMethodA(obj, mID, args);
    }

    static jlong callLongMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallLongMethod(obj, mID);
    }

    static jdouble callDoubleMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallDoubleMethodA(obj, mID, args);
    }

    static jdouble callDoubleMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallDoubleMethodA(obj, mID);
    }

    static jboolean callBooleanMethod(jobject obj, const char *name, const char *sign, jvalue *args) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallBooleanMethodA(obj, mID, args);
    }

    static jboolean callBooleanMethod(jobject obj, const char *name, const char *sign) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        return javaEnv -> CallBooleanMethodA(obj, mID);
    }
};


/**
 * 提供各种类型转换函数
 */
class convert {
public:
    static const char *jstring2charp(jstring jstr) {
        jclass mClass = StringClass;
        jmethodID mID = javaEnv -> GetMethodID(mClass, "getBytes", "(Ljava/lang/String;)[B");
        jbyteArray array = (jbyteArray) javaEnv -> CallObjectMethod(jstr, mID, javaEnv -> NewStringUTF("utf-8"));
        jsize length = javaEnv -> GetArrayLength(array);
        jbyte* barray = javaEnv -> GetByteArrayElements(array, JNI_FALSE);
        char *cstr = (char*) malloc(length + 1);
        cstr[0] = 0;
        if (length > 0) {
            memcpy(cstr, barray, length);
            cstr[length] = 0;
        }
        return cstr;
    }

    static PyObject *toPython(const jobject obj) {
        if (javaEnv -> IsInstanceOf(obj, StringClass)) {
            return PyUnicode_FromString(jstring2charp((jstring) obj));
        }
    }

    static jobject toJava(PyObject *obj) {
        jvalue *args = new jvalue[1];  // Todo： 内存释放相关处理
        if (PyLong_Check(obj)) {
            if (PyLong_AsLongLong(obj) > INT_MAX) {
                args -> j = PyLong_AsLongLong(obj);
                return jniutils::newInstance(LongClass, "(J)V", args);
            } else {
                args -> i = PyLong_AsLong(obj);
                return jniutils::newInstance(IntegerClass, "(I)V", args);
            }
        } else if (PyUnicode_Check(obj)) {
            return javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
        } else if (PyFloat_Check(obj)) {
            args -> d = PyFloat_AsDouble(obj);
            return jniutils::newInstance(DoubleClass, "(D)V", args);
        } else if (obj -> ob_type == &JavaObjectType) {
            // Todo: 这个类型判断不懂对不对，翻了 Python 源码好像也是这样写的
            return ((PyJavaObject *) obj) -> mObject;
        } else {
            PyObject *objType = PyObject_GetAttrString(obj, "__class__");
            PyObject *typeName = PyObject_GetAttrString(objType, "__name__");
            std::stringstream sstream;
            sstream << "convert::toJava: type " << PyUnicode_AsUTF8(typeName) << " is not supported.";
            PyErr_SetString(PyExc_ValueError, sstream.str().c_str());
            return NULL;
        }
    }

    static jvalue toJvalue(PyObject *obj) {
        jvalue result;
        if (PyLong_Check(obj)) {
            if (PyLong_AsLongLong(obj) > INT_MAX) {
                result.j = PyLong_AsLongLong(obj);
                return result;
            } else {
                result.i = PyLong_AsLong(obj);
                return result;
            }
        } else if (PyUnicode_Check(obj)) {
            result.l = javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
            return result;
        } else if (PyFloat_Check(obj)) {
            result.d = PyFloat_AsDouble(obj);
            return result;
        } else if (obj -> ob_type == &JavaObjectType) {
            result.l = ((PyJavaObject *) obj) -> mObject;
            return result;
        } else {
            PyObject *objType = PyObject_GetAttrString(obj, "__class__");
            PyObject *typeName = PyObject_GetAttrString(objType, "__name__");
            std::stringstream sstream;
            sstream << "convert::toJvalue: type " << PyUnicode_AsUTF8(typeName) << " is not supported.";
            PyErr_SetString(PyExc_ValueError, sstream.str().c_str());
        }
    }
};


/**
 * Python 函数定义
 */
static PyObject *PyJavaObject_str(PyObject *obj) {
    jobject jobj = ((PyJavaObject *) obj) -> mObject;
    jmethodID mID = javaEnv -> GetMethodID(javaEnv -> GetObjectClass(jobj), "toString", "()Ljava/lang/String;");
    jobject jstr = javaEnv -> CallObjectMethod(jobj, mID);
    std::stringstream sstream;
    sstream << convert::jstring2charp((jstring) jstr);
    return Py_BuildValue("s", sstream.str().c_str());
}

/**
 * 在 Python 中初始化一个 jobject，支持两种模式
 * @param type
 * @param args 传入一个基本类型 / 类名字符串+构造参数列表
 * @param kwargs 当 reflect 字段为 True 时，将不进行转换而是通过反射创建一个实例
 * @return 返回 Python 可存储的 Java 对象
 */
static PyObject *PyJavaObject_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    // Todo: 通过类名创建实例
    PyJavaObject *self;
    self = (PyJavaObject *) type -> tp_alloc(type, 0);

    if (PyTuple_Size(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "The constructor of JavaObject accepts only one argument");
        return NULL;
    }

    self -> mObject = convert::toJava(PyTuple_GetItem(args, 0));
    return (PyObject *) self;
}

static PyObject *PyJavaObject_CallObjectMethod(PyObject *self, PyObject *args) {
    PyJavaObject *obj = PyObject_NEW(PyJavaObject, &JavaObjectType);
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        obj -> mObject = jniutils::callObjectMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        obj -> mObject = jniutils::callObjectMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return (PyObject *) obj;
}

static PyObject *PyJavaObject_CallStringMethod(PyObject *self, PyObject *args) {
    jstring result;
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        result = (jstring) jniutils::callStringMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = (jstring) jniutils::callStringMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return PyUnicode_FromString(convert::jstring2charp(result));
}

static PyObject *PyJavaObject_CallIntMethod(PyObject *self, PyObject *args) {
    jint result;
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        result = jniutils::callIntMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callIntMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return PyLong_FromLong(result);
}

static PyObject *PyJavaObject_CallLongMethod(PyObject *self, PyObject *args) {
    jlong result;
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        result = jniutils::callLongMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callLongMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return PyLong_FromLongLong(result);
}

static PyObject *PyJavaObject_CallDoubleMethod(PyObject *self, PyObject *args) {
    jdouble result;
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        result = jniutils::callDoubleMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callDoubleMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return PyFloat_FromDouble(result);
}

static PyObject *PyJavaObject_CallBooleanMethod(PyObject *self, PyObject *args) {
    jboolean result;
    char *name, *sign;
    if (PyTuple_Size(args) > 2) {
        PyObject *pyargs;
        PyArg_ParseTuple(args, "ssO", &name, &sign, &pyargs);
        Py_ssize_t length = PyTuple_Size(pyargs);
        std::vector<jvalue> jargs;
        for (Py_ssize_t i = 0; i < length; i++) {
            jargs.push_back(convert::toJvalue(PyTuple_GetItem(pyargs, i)));
        }
        result = jniutils::callBooleanMethod(((PyJavaObject *) self) -> mObject, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callBooleanMethod(((PyJavaObject *) self) -> mObject, name, sign);
    }
    return result ? Py_True : Py_False;
}


/**
 * 初始化相关函数
 */
static void init(JNIEnv *env) {
    javaEnv = env;
    IntegerClass = javaEnv -> FindClass("java/lang/Integer");
    BooleanClass = javaEnv -> FindClass("java/lang/Boolean");
    DoubleClass = javaEnv -> FindClass("java/lang/Double");
    LongClass = javaEnv -> FindClass("java/lang/Long");
    StringClass = javaEnv -> FindClass("java/lang/String");
}

PyMODINIT_FUNC PyInit_bridgeutils(void) {
    PyType_Ready(&JavaObjectType);
    PyObject *module = PyModule_Create(&PyBridgeModule);
    Py_INCREF((PyObject *) &JavaObjectType);
    PyModule_AddObject(module, "JavaObject", (PyObject *) &JavaObjectType);
    return module;
}

/**
 * 通过 JNI 调用此函数，启动 Python 解释器
 * 从 Java 传递一个文件名，根据此文件名运行 Python 脚本
 */
extern "C"
JNIEXPORT void JNICALL
Java_mfc_automator_Automaton_runPython(JNIEnv *env, jobject obj, jstring filename) {
    init(env);

    // 初始化 Python 环境
    PyImport_AppendInittab("bridgeutils", PyInit_bridgeutils);
    Py_Initialize();
    const char *name = convert::jstring2charp(filename);
    FILE *fp = fopen(name, "r");
    PyRun_SimpleFile(fp, name);
    Py_Finalize();
}
