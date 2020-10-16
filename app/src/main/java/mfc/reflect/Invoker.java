package mfc.reflect;

import java.lang.reflect.Proxy;

public class Invoker {
//    private Object mParent;
//
//    public Invoker(Object parent) {
//        mParent = parent;
//    }

    public Object getInstance(Class<?> clazz){
//        MethodProxy invocationHandler = new MethodProxy(mParent);
        MethodProxy invocationHandler = new MethodProxy();
        Object newProxyInstance = Proxy.newProxyInstance(
                clazz.getClassLoader(), new Class<?>[] {clazz}, invocationHandler
        );
        return (Object) newProxyInstance;
    }
}
