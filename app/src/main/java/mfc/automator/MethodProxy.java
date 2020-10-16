package mfc.automator;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class MethodProxy implements InvocationHandler {
    /**
     * ClassName invoker = (ClassName) new Invoker().getInstance(ClassName.class);
     * System.out.println(invoker.MethodName());
     */

//    public HashSet<String> pMethods = new HashSet<String>();
//    public Object mParent;
//
//    public MethodProxy(Object parent) {
////        mParent = parent;
////        Method[] mth = parent.getClass().getMethods();
////        for (Method method : mth) {
////            pMethods.add(method.getName());
////        }
//    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args){
        try {
            if (Object.class.equals(method.getDeclaringClass())) {  // 类
//                if (pMethods.contains(method.getName())) {
//                    Object result = method.invoke(mParent, args);
//                    Object newObject;
//                    if (result == null) {
//                        newObject = ReflectTools.getInstance(
//                                Object.class,
//                                new Class<?>[] {},
//                                new Object[] {}
//                        );
//                    } else {
//                        newObject = ReflectTools.getInstance(
//                                Object.class,
//                                new Class<?>[]{result.getClass()},
//                                new Object[]{result}
//                        );
//                    }
////                    assert newObject != null;
//                    System.out.println("嘤嘤嘤");
//                    return newObject;
////                    return new Invoker(null).getInstance(newObject.getClass());
//                }
                return method.invoke(this, args);
            } else {  // 接口
//                if (pMethods.contains(method.getName())) {
//                    return method.invoke(mParent, args);
//                }
                return method.invoke(args);
            }
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }
}
