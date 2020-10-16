package mfc.automator;

import android.os.Bundle;
import android.view.accessibility.AccessibilityNodeInfo;

import java.nio.DoubleBuffer;

public class AccessibilityNode {
    private AccessibilityNodeInfo mNode;

    public AccessibilityNode(AccessibilityNodeInfo pNode) {
        mNode = pNode;
    }

    private static Bundle argumentsToBundle(ActionArgument[] arguments) {
        Bundle bundle = new Bundle();
        for (ActionArgument arg : arguments) {
            arg.putIn(bundle);
        }
        return bundle;
    }

    public boolean performAction(int action, ActionArgument... arguments) {
        Bundle bundle = argumentsToBundle(arguments);
        return mNode.performAction(action, bundle);
    }

    public boolean click() {
        if (!mNode.isClickable()) {
            AccessibilityNodeInfo parent = mNode.getParent();
            if (parent == null) {
                return false;
            }
            return new AccessibilityNode(parent).click();
        }
        return mNode.performAction(AccessibilityNodeInfo.ACTION_CLICK);
    }

    public boolean longClick(){
        if (!mNode.isClickable()) {
            AccessibilityNodeInfo parent = mNode.getParent();
            if (parent == null) {
                return false;
            }
            return new AccessibilityNode(parent).click();
        }
        return mNode.performAction(AccessibilityNodeInfo.ACTION_LONG_CLICK);
    }

    public boolean setSelection(int start, int end) {
        return performAction(
                AccessibilityNodeInfo.ACTION_SET_SELECTION,
                new ActionArgument.IntActionArgument("ACTION_ARGUMENT_SELECTION_START_INT", start),
                new ActionArgument.IntActionArgument("ACTION_ARGUMENT_SELECTION_END_INT", end)
        );
    }

    public boolean copy() {
        return mNode.performAction(AccessibilityNodeInfo.ACTION_COPY);
    }

    public boolean paste() {
        return mNode.performAction(AccessibilityNodeInfo.ACTION_PASTE);
    }

    public boolean cut() {
        return mNode.performAction(AccessibilityNodeInfo.ACTION_CUT);
    }

    public String getText() {
        return mNode.getText().toString();
    }

    public boolean setText(String text) {
        return performAction(
                0x00200000,  // ACTION_SET_TEXT
                new ActionArgument.CharSequenceActionArgument("ACTION_ARGUMENT_SET_TEXT_CHARSEQUENCE", text)
        );
    }

    // Todo: 添加获取 Node 信息的方法，如 depth，index 等
    // Todo: 添加更多的操作方法，如 scrollTo
}
