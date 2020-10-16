package mfc.reflect;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class ReflectUtils {
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

    public static Object callMethod(Class<?> clazz, String name, Class<?>[] flags, Object[] args) {
        try {
            Method newMethod = clazz.getMethod(name, flags);
            newMethod.setAccessible(true);
            return newMethod.invoke(null, args);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }

    public static Object callMethod(Object instance, String name, Class<?>[] flags, Object[] args) {
        try {
            Method newMethod = instance.getClass().getMethod(name, flags);
            newMethod.setAccessible(true);
            return newMethod.invoke(instance, args);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }

    public static Object getMember(Object instance, String fieldName) {
        try {
            Field field = instance.getClass().getDeclaredField(fieldName);
            field.setAccessible(true);
            return field.get(instance);
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }
}
