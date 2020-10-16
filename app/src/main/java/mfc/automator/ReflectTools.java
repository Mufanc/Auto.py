package mfc.automator;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;

public class ReflectTools {
    public static Object getInstance(Class<?> clazz, Class<?>[] args, Object[] srcs) {
        try {
            Constructor<?> newInstance = clazz.getDeclaredConstructor(args);
            newInstance.setAccessible(true);
            return newInstance.newInstance(srcs);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }

    public static Object callMethod(Class<?> clazz, String name, Object instance, Class<?>[] flags, Object[] args) {
        try {
            Method newMethod = clazz.getMethod(name, flags);
            return newMethod.invoke(instance, args);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }
}
