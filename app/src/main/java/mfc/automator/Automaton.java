package mfc.automator;

import android.annotation.SuppressLint;
import android.app.UiAutomation;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import android.view.accessibility.AccessibilityNodeInfo;
import com.android.uiautomator.core.UiDevice;
import com.android.uiautomator.core.UiSelector;

public class Automaton {
    public static Class<?> uiDeviceClass = UiDevice.class;
    public static Class<?> uiSelectorClass = UiSelector.class;
    public static Class<?> uiAutomationClass = UiAutomation.class;
    public static Class<?> uiAutomatorBridgeClass;
    public static Class<?> shellUiAutomatorBridgeClass;

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
            Object connection = ReflectTools.getInstance(
                    Class.forName("android.app.UiAutomationConnection"),
                    new Class[] {}, new Object[] {}
            );
            Automaton.uiAutomation = (UiAutomation) ReflectTools.getInstance(
                    Automaton.uiAutomationClass,
                    new Class<?>[] {Looper.class, Class.forName("android.app.IUiAutomationConnection")},
                    new Object[] {mHandlerThread.getLooper(), connection}
            );

            // 反射连接无障碍
            ReflectTools.callMethod(
                    Automaton.uiAutomationClass,
                    "connect", Automaton.uiAutomation,
                    new Class<?>[] {}, new Object[] {}
            );

            // 反射获取 UiAutomatorBridge
            Automaton.uiAutomatorBridge = ReflectTools.getInstance(
                    Automaton.shellUiAutomatorBridgeClass,
                    new Class<?>[] {Automaton.uiAutomationClass}, new Object[] {Automaton.uiAutomation}
            );
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    // Todo: 这家伙每次才找一个有点垃圾，考虑自己写一个搜索算法~233
    public static AccessibilityNodeInfo findAccessibilityNodeInfo(long timeout, UiSelector mSelector) {
        try {
            AccessibilityNodeInfo node = null;
            long startMills = SystemClock.uptimeMillis();
            long currentMills = 0;
            while (currentMills <= timeout || timeout == -2147483648) {
                @SuppressLint("PrivateApi") Class<?> queryControllerClass = Class.forName("com.android.uiautomator.core.QueryController");
                Object mController = ReflectTools.getInstance(
                        queryControllerClass,
                        new Class<?>[] {Automaton.uiAutomatorBridgeClass},
                        new Object[] {Automaton.uiAutomatorBridge}
                );
                node = (AccessibilityNodeInfo) ReflectTools.callMethod(
                        queryControllerClass, "findAccessibilityNodeInfo", mController,
                        new Class<?>[] {UiSelector.class}, new Object[] {mSelector}
                );

                if (node != null) {
                    break;
                } else {
                    // does nothing if we're reentering another runWatchers()
                    UiDevice.getInstance().runWatchers();
                }
                currentMills = SystemClock.uptimeMillis() - startMills;
                if (timeout > 0 || timeout == -2147483648) {
                    SystemClock.sleep(10);  // WAIT_FOR_SELECTOR_POLL
                }
            }
            return node;
        } catch (Exception err) {
            err.printStackTrace();
        }
        return null;
    }

    public void uiTest2() {
        try {
            System.out.println(" -- Start -- ");
            new Device().setOrientationNatural();
//            Selector mSelector = new Selector(new UiSelector().resourceId("com.tencent.mobileqq:id/input"));
//            AccessibilityNode mNode = new AccessibilityNode(mSelector.findOne());
//            mNode.setText("asdfklhgfjkghvifuhvf");
//            mNode.setSelection(0, 5);
//            mNode.cut();
//            System.out.println(mNode.getText());
            System.out.println(" -- Finish -- ");
            Thread.sleep(1000);
        } catch(Exception err) {
            err.printStackTrace();
        }
    }
}
