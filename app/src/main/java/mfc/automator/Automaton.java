package mfc.automator;

import android.annotation.SuppressLint;
import android.app.UiAutomation;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import android.view.accessibility.AccessibilityNodeInfo;

import com.android.uiautomator.core.UiDevice;
import com.android.uiautomator.core.UiSelector;

import mfc.reflect.ReflectUtils;

public class Automaton {
    public static Class<?> uiDeviceClass = UiDevice.class;
    public static Class<?> uiSelectorClass = UiSelector.class;
    public static Class<?> uiAutomationClass = UiAutomation.class;
    public static Class<?> accessibilityNodeInfoClass = AccessibilityNodeInfo.class;
    public static Class<?> uiAutomatorBridgeClass;
    public static Class<?> shellUiAutomatorBridgeClass;

    public static String algorithm = "dfs";
    public static Object uiAutomatorBridge;
    public static UiAutomation uiAutomation;


    @SuppressLint("PrivateApi")
    public Automaton() {
        HandlerThread mHandlerThread = new HandlerThread("UiAutomationThread");
        mHandlerThread.start();
        try {
            // 初始化类静态属性
            Automaton.uiAutomatorBridgeClass = Class.forName("com.android.uiautomator.core.UiAutomatorBridge");
            Automaton.shellUiAutomatorBridgeClass = Class.forName("com.android.uiautomator.core.ShellUiAutomatorBridge");

            // 反射获取 UiAutomation
            Object connection = ReflectUtils.getInstance(
                    Class.forName("android.app.UiAutomationConnection"),
                    new Class[] {}, new Object[] {}
            );
            Automaton.uiAutomation = (UiAutomation) ReflectUtils.getInstance(
                    Automaton.uiAutomationClass,
                    new Class<?>[] {Looper.class, Class.forName("android.app.IUiAutomationConnection")},
                    new Object[] {mHandlerThread.getLooper(), connection}
            );

            // 反射连接无障碍
            ReflectUtils.callMethod(
                    Automaton.uiAutomation,"connect",
                    new Class<?>[] {}, new Object[] {}
            );

            // 反射获取 UiAutomatorBridge
            Automaton.uiAutomatorBridge = ReflectUtils.getInstance(
                    Automaton.shellUiAutomatorBridgeClass,
                    new Class<?>[] {Automaton.uiAutomationClass}, new Object[] {Automaton.uiAutomation}
            );
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public static void setAlgorithm(String algorithm) throws Exception {
        if (!algorithm.equals("dfs") && !algorithm.equals("bfs")) {
            throw new Exception("Invalid algorithm name: " + algorithm);
        }
        Automaton.algorithm = algorithm;
    }

//    // Todo: 这家伙每次才找一个有点垃圾，考虑自己写一个搜索算法 ~ 233
//    public static AccessibilityNodeInfo findAccessibilityNodeInfo(long timeout, UiSelector mSelector) {
//        try {
//            AccessibilityNodeInfo node = null;
//            long startMills = SystemClock.uptimeMillis();
//            long currentMills = 0;
//            while (currentMills <= timeout || timeout == -2147483648) {
//                @SuppressLint("PrivateApi") Class<?> queryControllerClass = Class.forName("com.android.uiautomator.core.QueryController");
//                Object mController = ReflectUtils.getInstance(
//                        queryControllerClass,
//                        new Class<?>[] {Automaton.uiAutomatorBridgeClass},
//                        new Object[] {Automaton.uiAutomatorBridge}
//                );
//                node = (AccessibilityNodeInfo) ReflectUtils.callMethod(
//                        mController, "findAccessibilityNodeInfo",
//                        new Class<?>[] {UiSelector.class}, new Object[] {mSelector}
//                );
//
//                if (node != null) {
//                    break;
//                } else {
//                    // does nothing if we're reentering another runWatchers()
//                    UiDevice.getInstance().runWatchers();
//                }
//                currentMills = SystemClock.uptimeMillis() - startMills;
//                if (timeout > 0 || timeout == -2147483648) {
//                    SystemClock.sleep(10);  // WAIT_FOR_SELECTOR_POLL
//                }
//            }
//            return node;
//        } catch (Exception err) {
//            err.printStackTrace();
//        }
//        return null;
//    }

    public native void runPython();

    public void uiTest2() {
        try {
            System.out.println(" -- Start -- ");
            long start = SystemClock.uptimeMillis();
            ////////////////////////////////////////////////////////
            Automaton.setAlgorithm("bfs");
            Selector mSelector = new Selector(new UiSelector().resourceId("com.tencent.mobileqq:id/input"));
            mSelector.findOne().setText("大家好，我是Auto.py【bfs】");
            Selector mButton = new Selector(new UiSelector().resourceId("com.tencent.mobileqq:id/fun_btn"));
            mButton.findOne().click();
            long end = SystemClock.uptimeMillis();
            ////////////////////////////////////////////////////////
            System.out.println(" -- Finish -- ");
            System.out.println("Process finished in " + (end-start) + "ms");
            Thread.sleep(1000);
        } catch(Exception err) {
            err.printStackTrace();
        }
    }
}
