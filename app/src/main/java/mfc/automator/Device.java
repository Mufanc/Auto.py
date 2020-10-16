package mfc.automator;

import android.graphics.Point;
import android.os.RemoteException;

import com.android.uiautomator.core.UiDevice;

import java.io.File;

import mfc.reflect.ReflectUtils;

public class Device {
    public UiDevice mDevice;

    public Device() {
        try {
            // 获取 UiDevice 实例
            Object mUiDevice = ReflectUtils.getInstance(
                    Automaton.uiDeviceClass,
                    new Class<?>[] {}, new Object[] {}
            );

            // 用 UiAutomatorBridge 初始化 UiDevice
            ReflectUtils.callMethod(
                    mDevice, "initialize",
                    new Class<?>[] {Automaton.uiAutomatorBridgeClass},
                    new Object[] {Automaton.uiAutomatorBridge}
            );
            mDevice = (UiDevice) mUiDevice;
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void freezeRotation() {
        try {
            mDevice.freezeRotation();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void unfreezeRotation() {
        try {
            mDevice.unfreezeRotation();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void setOrientationLeft() {
        try {
            mDevice.setOrientationLeft();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void setOrientationRight() {
        try {
            mDevice.setOrientationRight();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void setOrientationNatural() {
        try {
            mDevice.setOrientationNatural();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public void wakeUp() throws RemoteException {
        mDevice.wakeUp();
    }

    public boolean isScreenOn() throws RemoteException {
        return mDevice.isScreenOn();
    }

    public void sleep() {
        try {
            mDevice.sleep();
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    public Point getDisplaySizeDp() {
        return mDevice.getDisplaySizeDp();
    }

    public String getProductName() {
        return mDevice.getProductName();
    }

    public String getLastTraversedText() {
        return mDevice.getLastTraversedText();
    }

    public void clearLastTraversedText() {
        mDevice.clearLastTraversedText();
    }

    public boolean pressMenu() {
        return mDevice.pressMenu();
    }

    public boolean pressBack() {
        return mDevice.pressBack();
    }

    public boolean pressHome() {
        return mDevice.pressHome();
    }

    public boolean pressSearch() {
        return mDevice.pressSearch();
    }

    public boolean pressDPadCenter() {
        return mDevice.pressDPadCenter();
    }

    public boolean pressDPadDown() {
        return mDevice.pressDPadDown();
    }

    public boolean pressDPadUp() {
        return mDevice.pressDPadUp();
    }

    public boolean pressDPadLeft() {
        return mDevice.pressDPadLeft();
    }

    public boolean pressDPadRight() {
        return mDevice.pressDPadRight();
    }

    public boolean pressDelete() {
        return mDevice.pressDelete();
    }

    public boolean pressEnter() {
        return mDevice.pressEnter();
    }

    public boolean pressKeyCode(int keyCode) {
        return mDevice.pressKeyCode(keyCode);
    }

    public boolean pressKeyCode(int keyCode, int metaState) {
        return mDevice.pressKeyCode(keyCode, metaState);
    }

    public boolean openNotification() {
        return mDevice.openNotification();
    }

    public boolean openQuickSettings() {
        return mDevice.openQuickSettings();
    }

    public int getDisplayWidth() {
        return mDevice.getDisplayWidth();
    }

    public int getDisplayHeight() {
        return mDevice.getDisplayHeight();
    }

    public boolean click(int x, int y) {
        return mDevice.click(x, y);
    }

    public boolean swipe(int startX, int startY, int endX, int endY, int steps) {
        return mDevice.swipe(startX, startY, endX, endY, steps);
    }

    public boolean drag(int startX, int startY, int endX, int endY, int steps) {
        return mDevice.drag(startX, startY, endX, endY, steps);
    }

    public boolean swipe(Point[] segments, int segmentSteps) {
        return mDevice.swipe(segments, segmentSteps);
    }

    public void waitForIdle() {
        mDevice.waitForIdle();
    }

    public void waitForIdle(long timeout) {
        mDevice.waitForIdle(timeout);
    }

    public String getCurrentActivityName() {
        return mDevice.getCurrentActivityName();
    }

    public String getCurrentPackageName() {
        return mDevice.getCurrentPackageName();
    }

    public boolean isNaturalOrientation() {
        return mDevice.isNaturalOrientation();
    }

    public int getDisplayRotation() {
        return mDevice.getDisplayRotation();
    }

    public void dumpWindowHierarchy(String fileName) {
        mDevice.dumpWindowHierarchy(fileName);
    }

    public boolean waitForWindowUpdate(final String packageName, long timeout) {
        return mDevice.waitForWindowUpdate(packageName, timeout);
    }

    public boolean takeScreenshot(File storePath) {
        return mDevice.takeScreenshot(storePath);
    }

    public boolean takeScreenshot(File storePath, float scale, int quality) {
        return mDevice.takeScreenshot(storePath, scale, quality);
    }
}
