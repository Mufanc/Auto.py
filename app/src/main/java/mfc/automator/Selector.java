package mfc.automator;

import android.view.accessibility.AccessibilityNodeInfo;
import com.android.uiautomator.core.UiSelector;

public class Selector extends com.android.uiautomator.core.UiSelector {
    private UiSelector mSelector;

    public Selector(UiSelector pSelector) {
        mSelector = pSelector;
    }

    public AccessibilityNodeInfo findOne() {
        return Automaton.findAccessibilityNodeInfo(-2147483648, mSelector);
    }

    public AccessibilityNodeInfo findOne(long timeout) {
        return Automaton.findAccessibilityNodeInfo(timeout, mSelector);
    }

    public AccessibilityNodeInfo findOnce() {
        return Automaton.findAccessibilityNodeInfo(0, mSelector);
    }

    public AccessibilityNodeInfo find() {
        // Todo: 加入 “控件集合” 的概念，一次返回多个控件
        return null;
    }
}
