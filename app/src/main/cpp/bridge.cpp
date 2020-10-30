//  g++ bridge.cpp -I $PREFIX/include/python3.8/ -l python3.8 -fPIC -shared -o libbridge.so

#include <jni.h>
#include <Python.h>

#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>


/**
 * @author Mufanc
 *
 * 为 Python 提供 C++ 扩展，使其能通过 JNI 访问 Java 包和类
 * 在 Python 中增加了模块: bridgeutils
 *
 */


/**
 * 全局变量区
 */
// 基本全局变量
static jclass IntegerClass, BooleanClass, DoubleClass, LongClass, StringClass;
static JNIEnv *javaEnv;

// Python 自定义类型变量: JavaObject
static PyObject *PyJavaObject_CallObjectMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallStringMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallIntMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallLongMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallDoubleMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallBooleanMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaObject_CallMethod(PyObject *self, PyObject *args);
static PyMethodDef JavaObjectMethods[] = {
        {"call_object_method", (PyCFunction) PyJavaObject_CallObjectMethod, METH_VARARGS, "jniutils::CallObjectMethod"},
        {"call_string_method", (PyCFunction) PyJavaObject_CallStringMethod, METH_VARARGS, "jniutils::CallStringMethod"},
        {"call_int_method", (PyCFunction) PyJavaObject_CallIntMethod, METH_VARARGS, "jniutils::CallIntMethod"},
        {"call_long_method", (PyCFunction) PyJavaObject_CallLongMethod, METH_VARARGS, "jniutils::CallLongMethod"},
        {"call_double_method", (PyCFunction) PyJavaObject_CallDoubleMethod, METH_VARARGS, "jniutils::CallDoubleMethod"},
        {"call_boolean_method", (PyCFunction) PyJavaObject_CallBooleanMethod, METH_VARARGS, "jniutils::CallBooleanMethod"},
        {"call_method", (PyCFunction) PyJavaObject_CallMethod, METH_VARARGS, "jniutils::CallMethod [Auto]"},
        {nullptr}
};

typedef struct { // Todo: 如果传递 int 类型参数出现问题，则需重构代码 (但现在看好像没问题)
    PyObject_HEAD
    jvalue mValue;
    char typeSign;
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

// Python 自定义类型变量: JavaClass
static PyObject *PyJavaClass_CallObjectMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallStringMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallIntMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallLongMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallDoubleMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallBooleanMethod(PyObject *self, PyObject *args);
static PyObject *PyJavaClass_CallMethod(PyObject *self, PyObject *args);
static PyMethodDef JavaClassMethods[] = {
        {"call_object_method", (PyCFunction) PyJavaClass_CallObjectMethod, METH_VARARGS, "jniutils::CallStaticObjectMethod"},
        {"call_string_method", (PyCFunction) PyJavaClass_CallStringMethod, METH_VARARGS, "jniutils::CallStaticStringMethod"},
        {"call_int_method", (PyCFunction) PyJavaClass_CallIntMethod, METH_VARARGS, "jniutils::CallStaticIntMethod"},
        {"call_long_method", (PyCFunction) PyJavaClass_CallLongMethod, METH_VARARGS, "jniutils::CallStaticLongMethod"},
        {"call_double_method", (PyCFunction) PyJavaClass_CallDoubleMethod, METH_VARARGS, "jniutils::CallStaticDoubleMethod"},
        {"call_boolean_method", (PyCFunction) PyJavaClass_CallBooleanMethod, METH_VARARGS, "jniutils::CallStaticBooleanMethod"},
        {"call_method", (PyCFunction) PyJavaClass_CallMethod, METH_VARARGS, "jniutils::CallStaticMethod [Auto]"},
        {nullptr}
};

typedef struct {
    PyObject_HEAD
    jclass mClass;
} PyJavaClass;

static PyObject *PyJavaClass_str(PyObject *obj);
static PyObject *PyJavaClass_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);
static PyTypeObject JavaClassType = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "bridgeutils.JavaClass",                          /* tp_name */
        sizeof(PyJavaClass),                              /* tp_basicsize */
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
        PyJavaClass_str,                                  /* tp_str */
        nullptr,                                           /* tp_getattro */
        nullptr,                                           /* tp_setattro */
        nullptr,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                /* tp_flags */
        "Adapt a java class in Python.",                  /* tp_doc */
        nullptr,                                           /* tp_traverse */
        nullptr,                                           /* tp_clear */
        nullptr,                                           /* tp_richcompare */
        0,                                                 /* tp_weaklistoffset */
        nullptr,                                           /* tp_iter */
        nullptr,                                           /* tp_iternext */
        JavaClassMethods,                                 /* tp_methods */
        nullptr,                                           /* tp_members */
        nullptr,                                           /* tp_getset */
        nullptr,                                           /* tp_base */
        nullptr,                                           /* tp_dict */
        nullptr,                                           /* tp_descr_get */
        nullptr,                                           /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        nullptr,                                           /* tp_init */
        nullptr,                                           /* tp_alloc */
        PyJavaClass_new                                   /* tp_new */
};

// Python 扩展模块: bridgeutils
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
    static jobject newInstance(const char *classname, const char *sign, const jvalue *args=NULL) {
        jclass mClass = javaEnv -> FindClass(classname);
        jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
        if (args) {
            return javaEnv -> NewObjectA(mClass, mID, args);
        } else {
            return javaEnv -> NewObject(mClass, mID);
        }
    }

    static jobject newInstance(jclass mClass, const char *sign, const jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetMethodID(mClass, "<init>", sign);
        if (args) {
            return javaEnv -> NewObjectA(mClass, mID, args);
        } else {
            return javaEnv -> NewObject(mClass, mID);
        }

    }

    static jobject callObjectMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallObjectMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallObjectMethod(obj, mID);
        }
    }

    static jobject callStringMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallObjectMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallObjectMethod(obj, mID);
        }
    }

    static jint callIntMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallIntMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallIntMethod(obj, mID);
        }
    }

    static jlong callLongMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallLongMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallLongMethod(obj, mID);
        }
    }

    static jdouble callDoubleMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallDoubleMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallDoubleMethod(obj, mID);
        }
    }

    static jboolean callBooleanMethod(jobject obj, const char *name, const char *sign, jvalue *args=NULL) {
        jclass mClass = javaEnv -> GetObjectClass(obj);
        jmethodID mID = javaEnv -> GetMethodID(mClass, name, sign);
        if (args) {
            return javaEnv -> CallBooleanMethodA(obj, mID, args);
        } else {
            return javaEnv -> CallBooleanMethod(obj, mID);
        }
    }

    static jobject callStaticObjectMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetStaticMethodID(clazz, name, sign);
        if (args) {
            return javaEnv -> CallStaticObjectMethodA(clazz, mID, args);
        } else {
            return javaEnv -> CallStaticObjectMethod(clazz, mID);
        }
    }

    static jobject callStaticStringMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        return callStaticObjectMethod(clazz, name, sign, args);
    }

    static jint callStaticIntMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetStaticMethodID(clazz, name, sign);
        if (args) {
            return javaEnv -> CallStaticIntMethodA(clazz, mID, args);
        } else {
            return javaEnv -> CallStaticIntMethod(clazz, mID);
        }
    }

    static jlong callStaticLongMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetStaticMethodID(clazz, name, sign);
        if (args) {
            return javaEnv -> CallStaticLongMethodA(clazz, mID, args);
        } else {
            return javaEnv -> CallStaticLongMethod(clazz, mID);
        }
    }

    static jdouble callStaticDoubleMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetStaticMethodID(clazz, name, sign);
        if (args) {
            return javaEnv -> CallStaticDoubleMethodA(clazz, mID, args);
        } else {
            return javaEnv -> CallStaticDoubleMethod(clazz, mID);
        }
    }

    static jboolean callStaticBooleanMethod(jclass clazz, const char *name, const char *sign, jvalue *args=NULL) {
        jmethodID mID = javaEnv -> GetStaticMethodID(clazz, name, sign);
        if (args) {
            return javaEnv -> CallStaticBooleanMethodA(clazz, mID, args);
        } else {
            return javaEnv -> CallStaticBooleanMethod(clazz, mID);
        }
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

    static PyObject *toPython(const jobject obj) {  // Todo: 好像没什么用
        if (javaEnv -> IsInstanceOf(obj, StringClass)) {
            return PyUnicode_FromString(jstring2charp((jstring) obj));
        }
        return NULL;
    }

    static jobject toJava(PyObject *obj) {
        /**
         * 这函数返回的一定是 jobject 类型变量
         * 比如它会把 PyLong 转换为 java.lang.Integer
         * 这样在调用方法传参的时候就很可能出问题，需要谨慎使用
         */
        jvalue *args = new jvalue[1];
        jobject result;
        if (PyBool_Check(obj)) {
            args -> z = (obj == Py_True);
            result = jniutils::newInstance(BooleanClass, "(Z)V", args);
        } else if (PyLong_Check(obj)) {
            if (PyLong_AsLongLong(obj) > INT_MAX) {
                args -> j = PyLong_AsLongLong(obj);
                result = jniutils::newInstance(LongClass, "(J)V", args);
            } else {
                args -> i = PyLong_AsLong(obj);
                result = jniutils::newInstance(IntegerClass, "(I)V", args);
            }
        } else if (PyUnicode_Check(obj)) {
            delete[] args;
            return javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
        } else if (PyFloat_Check(obj)) {
            args -> d = PyFloat_AsDouble(obj);
            result = jniutils::newInstance(DoubleClass, "(D)V", args);
        } else if (obj -> ob_type == &JavaObjectType) {
            result = ((PyJavaObject *) obj) -> mValue.l;
        } else {
            delete[] args;
            PyObject *objType = PyObject_GetAttrString(obj, "__class__");
            PyObject *typeName = PyObject_GetAttrString(objType, "__name__");
            std::stringstream sstream;
            sstream << "convert::toJava: type " << PyUnicode_AsUTF8(typeName) << " is not supported.";
            PyErr_SetString(PyExc_ValueError, sstream.str().c_str());
            return NULL;
        }
        delete[] args;
        return result;
    }

    static jvalue toJvalue(PyObject *obj, char *typeSign=NULL) {
        jvalue result; result.l = NULL;
        if (PyBool_Check(obj)) {
            typeSign && (*typeSign = 'z');
            result.z = (obj == Py_True);
        } else if (PyLong_Check(obj)) {
            if (PyLong_AsLongLong(obj) > INT_MAX) {
                typeSign && (*typeSign = 'j');
                result.j = PyLong_AsLongLong(obj);
            } else {
                typeSign && (*typeSign = 'i');
                result.i = PyLong_AsLong(obj);
            }
        } else if (PyUnicode_Check(obj)) {
            typeSign && (*typeSign = 's');
            result.l = javaEnv -> NewStringUTF(PyUnicode_AsUTF8(obj));
        } else if (PyFloat_Check(obj)) {
            typeSign && (*typeSign = 'd');
            result.d = PyFloat_AsDouble(obj);
        } else if (obj -> ob_type == &JavaObjectType) {
            PyJavaObject *mObject = (PyJavaObject *) obj;
            typeSign && (*typeSign = mObject -> typeSign);
            result = mObject -> mValue;
        } else {
            PyObject *objType = PyObject_GetAttrString(obj, "__class__");
            PyObject *type = PyObject_GetAttrString(objType, "__name__");
            std::stringstream sstream;
            sstream << "convert::toJvalue: type " << PyUnicode_AsUTF8(type) << " is not supported.";
            PyErr_SetString(PyExc_ValueError, sstream.str().c_str());
        }
        return result;
    }
};


/**
 * Python 函数定义
 */
// JavaObject 相关函数
static PyObject *PyJavaObject_str(PyObject *obj) {
    // Todo: 类型判断
    char typeSign = ((PyJavaObject *) obj) -> typeSign;
    jvalue value = ((PyJavaObject *) obj) -> mValue;
    std::stringstream sstream;
    switch (typeSign) {
        case 'i':
            sstream << "jint: " << value.i;
            break;
        case 'j':
            sstream << "jlong: " << value.j;
            break;
        case 'd':
            sstream << "jdouble: " << value.d;
            break;
        case 'z':
            sstream << "jboolean: " << value.z;
            break;
        case 's':
            sstream << convert::jstring2charp((jstring) value.l);
            break;
        default: {
            jobject jobj = value.l;
            jmethodID mID = javaEnv -> GetMethodID(javaEnv -> GetObjectClass(jobj), "toString", "()Ljava/lang/String;");
            jobject jstr = javaEnv -> CallObjectMethod(jobj, mID);
            sstream << convert::jstring2charp((jstring) jstr);
            return PyUnicode_FromString(sstream.str().c_str());
        }
    }
    return PyUnicode_FromString(sstream.str().c_str());
}

/**
 * 在 Python 中初始化一个 jobject，支持两种模式
 * @param type
 * @param args 传入一个基本类型 / 类名字符串+构造参数列表
 * @param kwargs 当 reflect 字段为 True 时，将不进行转换而是通过反射创建一个实例
 * @return 返回 Python 可存储的 Java 对象
 */
static PyObject *PyJavaObject_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyJavaObject *self;
    self = (PyJavaObject *) type -> tp_alloc(type, 0);

    if (PyTuple_Size(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "The constructor of JavaObject accepts only one argument");
        return NULL;
    }

    PyObject *arg0, *reflect = Py_False;
    const char *kwlist[] = {"arg0", "reflect", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", (char **) kwlist, &arg0, &reflect)) {
        Py_DECREF(self);
        return NULL;
    }

    if (reflect == Py_False) {  // 将 Python 基本类型直接转换成 Java 对象
        self -> mValue = convert::toJvalue(PyTuple_GetItem(args, 0), &(self -> typeSign));
    } else {  // 通过反射创建对象
        PyTupleObject *tuple = (PyTupleObject *) arg0;
        if (PyTuple_Size(arg0) > 2) {
            std::vector<jvalue> jargs;
            Py_ssize_t length = PyTuple_Size(tuple -> ob_item[2]);
            for (Py_ssize_t i = 0; i < length; i++) {
                jargs.push_back(convert::toJvalue(PyTuple_GetItem(tuple -> ob_item[2], i)));
            }
            self -> mValue.l = jniutils::newInstance(
                    PyUnicode_AsUTF8(tuple -> ob_item[0]),
                    PyUnicode_AsUTF8(tuple -> ob_item[1]),
                    &jargs[0]
            );
        } else {
            self -> mValue.l = jniutils::newInstance(
                    PyUnicode_AsUTF8(tuple -> ob_item[0]),
                    PyUnicode_AsUTF8(tuple -> ob_item[1])
            );
        }
        self -> typeSign = 'o';
    }
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
        obj -> mValue.l = jniutils::callObjectMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        obj -> mValue.l = jniutils::callObjectMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
    }
    obj -> typeSign = 'o';
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
        result = (jstring) jniutils::callStringMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = (jstring) jniutils::callStringMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
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
        result = jniutils::callIntMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callIntMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
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
        result = jniutils::callLongMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callLongMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
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
        result = jniutils::callDoubleMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callDoubleMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
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
        result = jniutils::callBooleanMethod(((PyJavaObject *) self) -> mValue.l, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callBooleanMethod(((PyJavaObject *) self) -> mValue.l, name, sign);
    }
    return result ? Py_True : Py_False;
}

static PyObject *PyJavaObject_CallMethod(PyObject *self, PyObject *args) {
    const char *sign = PyUnicode_AsUTF8(PyTuple_GetItem(args, 2));
    int length = strlen(sign);
    switch (sign[length-1]) {
        case 'I':
            return PyJavaObject_CallIntMethod(self, args);
        case 'J':
            return PyJavaObject_CallLongMethod(self, args);
        case 'D':
            return PyJavaObject_CallDoubleMethod(self, args);
        case 'Z':
            return PyJavaObject_CallBooleanMethod(self, args);
        case ';': {
            const char *cmp = ";gnirtS/gnal/avajL";
            for (int i = 0; i < 18; i++) {
                if (sign[length - i - i] != cmp[i]) {
                    return PyJavaObject_CallObjectMethod(self, args);
                }
            }
            return PyJavaObject_CallStringMethod(self, args);
        }
        default:
            PyErr_SetString(PyExc_ValueError, "The type of returned value is not supported by JavaObject::CallMethod yet.");
            return NULL;
    }
}


// JavaClass 相关函数
static PyObject *PyJavaClass_str(PyObject *obj) {
    jclass clazz = ((PyJavaClass *) obj) -> mClass;
    jmethodID mID = javaEnv -> GetMethodID(clazz, "getName", "()Ljava/lang/String;");
    std::cout << "I'm fine: " << mID << std::endl;
    jobject jstr = javaEnv -> CallObjectMethod(clazz, mID);
    std::stringstream sstream;
    sstream << convert::jstring2charp((jstring) jstr);
    return PyUnicode_FromString(sstream.str().c_str());
}

static PyObject *PyJavaClass_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyJavaClass *self;
    self = (PyJavaClass *) type -> tp_alloc(type, 0);

    if (PyTuple_Size(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "The constructor of JavaClass accepts only one argument");
        return NULL;
    }

    self -> mClass = javaEnv -> FindClass(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)));
    return (PyObject *) self;
}

static PyObject *PyJavaClass_CallObjectMethod(PyObject *self, PyObject *args) {
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
        obj -> mValue.l = jniutils::callStaticObjectMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        obj -> mValue.l = jniutils::callStaticObjectMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    obj -> typeSign = 'o';
    return (PyObject *) obj;
}

static PyObject *PyJavaClass_CallStringMethod(PyObject *self, PyObject *args) {
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
        result = (jstring) jniutils::callStaticStringMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = (jstring) jniutils::callStaticStringMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    return PyUnicode_FromString(convert::jstring2charp(result));
}

static PyObject *PyJavaClass_CallIntMethod(PyObject *self, PyObject *args) {
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
        result = jniutils::callStaticIntMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callStaticIntMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    return PyLong_FromLong(result);
}

static PyObject *PyJavaClass_CallLongMethod(PyObject *self, PyObject *args) {
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
        result = jniutils::callStaticLongMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callStaticLongMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    return PyLong_FromLongLong(result);
}

static PyObject *PyJavaClass_CallDoubleMethod(PyObject *self, PyObject *args) {
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
        result = jniutils::callStaticDoubleMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callStaticDoubleMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    return PyFloat_FromDouble(result);
}

static PyObject *PyJavaClass_CallBooleanMethod(PyObject *self, PyObject *args) {
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
        result = jniutils::callStaticBooleanMethod(((PyJavaClass *) self) -> mClass, name, sign, &jargs[0]);
    } else {
        PyArg_ParseTuple(args, "ss", &name, &sign);
        result = jniutils::callStaticBooleanMethod(((PyJavaClass *) self) -> mClass, name, sign);
    }
    return result ? Py_True : Py_False;
}

static PyObject *PyJavaClass_CallMethod(PyObject *self, PyObject *args) {
    const char *sign = PyUnicode_AsUTF8(PyTuple_GetItem(args, 2));
    int length = strlen(sign);
    switch (sign[length-1]) {
        case 'I':
            return PyJavaClass_CallIntMethod(self, args);
        case 'J':
            return PyJavaClass_CallLongMethod(self, args);
        case 'D':
            return PyJavaClass_CallDoubleMethod(self, args);
        case 'Z':
            return PyJavaClass_CallBooleanMethod(self, args);
        case ';': {
            const char *cmp = ";gnirtS/gnal/avajL";
            for (int i = 0; i < 18; i++) {
                if (sign[length - i - i] != cmp[i]) {
                    return PyJavaClass_CallObjectMethod(self, args);
                }
            }
            return PyJavaClass_CallStringMethod(self, args);
        }
        default:
            PyErr_SetString(PyExc_ValueError, "The type of returned value is not supported by JavaClass::CallMethod yet.");
            return NULL;
    }
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
    PyType_Ready(&JavaClassType);
    PyObject *module = PyModule_Create(&PyBridgeModule);
    Py_INCREF((PyObject *) &JavaObjectType);
    PyModule_AddObject(module, "JavaObject", (PyObject *) &JavaObjectType);
    Py_INCREF((PyObject *) &JavaClassType);
    PyModule_AddObject(module, "JavaClass", (PyObject *) &JavaClassType);
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
    PyRun_SimpleString("__import__('sys').path.append('/data/local/tmp/files/modules/')");
    const char *name = convert::jstring2charp(filename);
    FILE *fp = fopen(name, "r");
    PyRun_SimpleFile(fp, name);
    Py_Finalize();
}
