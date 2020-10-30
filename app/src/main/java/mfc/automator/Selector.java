package mfc.automator;

import android.os.SystemClock;
import android.util.SparseArray;
import android.view.accessibility.AccessibilityNodeInfo;

import com.android.uiautomator.core.UiSelector;

import java.util.LinkedList;
import java.util.Queue;
import java.util.regex.Pattern;

import mfc.reflect.ReflectUtils;

public class Selector {
    static final int SELECTOR_NIL = 0;
    static final int SELECTOR_TEXT = 1;
    static final int SELECTOR_START_TEXT = 2;
    static final int SELECTOR_CONTAINS_TEXT = 3;
    static final int SELECTOR_CLASS = 4;
    static final int SELECTOR_DESCRIPTION = 5;
    static final int SELECTOR_START_DESCRIPTION = 6;
    static final int SELECTOR_CONTAINS_DESCRIPTION = 7;
    static final int SELECTOR_INDEX = 8;
    static final int SELECTOR_INSTANCE = 9;
    static final int SELECTOR_ENABLED = 10;
    static final int SELECTOR_FOCUSED = 11;
    static final int SELECTOR_FOCUSABLE = 12;
    static final int SELECTOR_SCROLLABLE = 13;
    static final int SELECTOR_CLICKABLE = 14;
    static final int SELECTOR_CHECKED = 15;
    static final int SELECTOR_SELECTED = 16;
    static final int SELECTOR_ID = 17;
    static final int SELECTOR_PACKAGE_NAME = 18;
    static final int SELECTOR_CHILD = 19;
    static final int SELECTOR_CONTAINER = 20;
    static final int SELECTOR_PATTERN = 21;
    static final int SELECTOR_PARENT = 22;
    static final int SELECTOR_COUNT = 23;
    static final int SELECTOR_LONG_CLICKABLE = 24;
    static final int SELECTOR_TEXT_REGEX = 25;
    static final int SELECTOR_CLASS_REGEX = 26;
    static final int SELECTOR_DESCRIPTION_REGEX = 27;
    static final int SELECTOR_PACKAGE_NAME_REGEX = 28;
    static final int SELECTOR_RESOURCE_ID = 29;
    static final int SELECTOR_CHECKABLE = 30;
    static final int SELECTOR_RESOURCE_ID_REGEX = 31;

    private final SparseArray<Object> mSelectorAttributes;  // Todo: final自己加的

    public Selector(UiSelector selector) {
        mSelectorAttributes = cast(ReflectUtils.getMember(
                selector,
                "mSelectorAttributes"
        ));
    }

    @SuppressWarnings("unchecked")
    public static <T> T cast(Object obj) {
        return (T) obj;
    }

    String getString(int criterion) {
        return (String) mSelectorAttributes.get(criterion, null);
    }

    boolean getBoolean(int criterion) {
        return (Boolean) mSelectorAttributes.get(criterion, false);
    }

    int getInt(int criterion) {
        return (Integer) mSelectorAttributes.get(criterion, 0);
    }

    Pattern getPattern(int criterion) {
        return (Pattern) mSelectorAttributes.get(criterion, null);
    }

    boolean isMatchFor(AccessibilityNodeInfo node, int index) {
        int size = mSelectorAttributes.size();
        for(int x = 0; x < size; x++) {
            CharSequence s = null;
            int criterion = mSelectorAttributes.keyAt(x);
            switch(criterion) {
                case Selector.SELECTOR_INDEX:
                    if (index != this.getInt(criterion))
                        return false;
                    break;
                case Selector.SELECTOR_CHECKED:
                    if (node.isChecked() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CLASS:
                    s = node.getClassName();
                    if (s == null || !s.toString().contentEquals(getString(criterion))) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CLASS_REGEX:
                    s = node.getClassName();
                    if (s == null || !getPattern(criterion).matcher(s).matches()) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CLICKABLE:
                    if (node.isClickable() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CHECKABLE:
                    if (node.isCheckable() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_LONG_CLICKABLE:
                    if (node.isLongClickable() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CONTAINS_DESCRIPTION:
                    s = node.getContentDescription();
                    if (s == null || !s.toString().toLowerCase()
                            .contains(getString(criterion).toLowerCase())) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_START_DESCRIPTION:
                    s = node.getContentDescription();
                    if (s == null || !s.toString().toLowerCase()
                            .startsWith(getString(criterion).toLowerCase())) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_DESCRIPTION:
                    s = node.getContentDescription();
                    if (s == null || !s.toString().contentEquals(getString(criterion))) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_DESCRIPTION_REGEX:
                    s = node.getContentDescription();
                    if (s == null || !getPattern(criterion).matcher(s).matches()) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_CONTAINS_TEXT:
                    s = node.getText();
                    if (s == null || !s.toString().toLowerCase()
                            .contains(getString(criterion).toLowerCase())) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_START_TEXT:
                    s = node.getText();
                    if (s == null || !s.toString().toLowerCase()
                            .startsWith(getString(criterion).toLowerCase())) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_TEXT:
                    s = node.getText();
                    if (s == null || !s.toString().contentEquals(getString(criterion))) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_TEXT_REGEX:
                    s = node.getText();
                    if (s == null || !getPattern(criterion).matcher(s).matches()) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_ENABLED:
                    if (node.isEnabled() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_FOCUSABLE:
                    if (node.isFocusable() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_FOCUSED:
                    if (node.isFocused() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_ID:
                    // Todo: do we need this for AccessibilityNodeInfo.id?
                    break;
                case Selector.SELECTOR_PACKAGE_NAME:
                    s = node.getPackageName();
                    if (s == null || !s.toString().contentEquals(getString(criterion))) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_PACKAGE_NAME_REGEX:
                    s = node.getPackageName();
                    if (s == null || !getPattern(criterion).matcher(s).matches()) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_SCROLLABLE:
                    if (node.isScrollable() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_SELECTED:
                    if (node.isSelected() != getBoolean(criterion)) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_RESOURCE_ID:
                    s = node.getViewIdResourceName();
                    if (s == null || !s.toString().contentEquals(getString(criterion))) {
                        return false;
                    }
                    break;
                case Selector.SELECTOR_RESOURCE_ID_REGEX:
                    s = node.getViewIdResourceName();
                    if (s == null || !getPattern(criterion).matcher(s).matches()) {
                        return false;
                    }
                    break;
            }
        }
        return matchOrUpdateInstance();
    }

    private boolean matchOrUpdateInstance() {
        int currentSelectorCounter = 0;
        int currentSelectorInstance = 0;

        // matched attributes - now check for matching instance number
        if (mSelectorAttributes.indexOfKey(Selector.SELECTOR_INSTANCE) >= 0) {
            currentSelectorInstance =
                    (Integer)mSelectorAttributes.get(Selector.SELECTOR_INSTANCE);
        }

        // instance is required. Add count if not already counting
        if (mSelectorAttributes.indexOfKey(Selector.SELECTOR_COUNT) >= 0) {
            currentSelectorCounter = (Integer)mSelectorAttributes.get(Selector.SELECTOR_COUNT);
        }

        // Verify
        if (currentSelectorInstance == currentSelectorCounter) {
            return true;
        }
        // Update count
        if (currentSelectorInstance > currentSelectorCounter) {
            mSelectorAttributes.put(Selector.SELECTOR_COUNT, ++currentSelectorCounter);
        }
        return false;
    }

    AccessibilityNode dfsSingle(AccessibilityNodeInfo root, int index) {
        // 控件发生变化时的特判
        if (root == null) {
            return null;
        }

        if (isMatchFor(root, index)) {
            return new AccessibilityNode(root);
        }
        int length = root.getChildCount();
        for (int i = 0; i < length; i++) {
            AccessibilityNode found = dfsSingle(root.getChild(i), i);
            if (found != null) {
                return found;
            }
        }
        return null;
    }

    private static class Pair {
        public AccessibilityNodeInfo node;
        public int index;

        Pair (AccessibilityNodeInfo mNode, int mIndex) {
            node = mNode;
            index = mIndex;
        }
    }

    AccessibilityNode bfsSingle(AccessibilityNodeInfo root) {
        Queue<Pair> task = new LinkedList<>();
        task.offer(new Pair(root, 0));
        while (!task.isEmpty()) {
            Pair top = task.poll();
            assert top != null;
            if (top.node == null) {
                continue;
            }
            if (isMatchFor(top.node, top.index)) {
                return new AccessibilityNode(top.node);
            }
            int length = top.node.getChildCount();
            for (int i = 0; i < length; i++) {
                task.offer(new Pair(top.node.getChild(i), i));
            }
        }
        return null;
    }

    public AccessibilityNode findOne(long timeout) throws InterruptedException {
        long start = SystemClock.uptimeMillis();
        if (Automaton.algorithm.equals("dfs")) {
            do {
                AccessibilityNode found = dfsSingle(Automaton.uiAutomation.getRootInActiveWindow(), 0);
                if (found != null) {
                    return found;
                }
//                Thread.sleep(10);
            } while (SystemClock.uptimeMillis() - start < timeout || timeout == -1);
        } else if (Automaton.algorithm.equals("bfs")) {
            do {
                AccessibilityNode found = bfsSingle(Automaton.uiAutomation.getRootInActiveWindow());
                if (found != null) {
                    return found;
                }
//                Thread.sleep(10);
            } while (SystemClock.uptimeMillis() - start < timeout || timeout == -1);
        }
        // Todo: 是否需要加入更高效智能的搜索算法？
        return null;
    }

    public AccessibilityNode findOne() throws InterruptedException {
        return findOne(-1);
    }

    public AccessibilityNode findOnce() throws InterruptedException {
        return findOne(0);
    }

    public AccessibilityNodeCollection find() {
        // Todo: 加入 “控件集合” 的概念，一次返回多个控件
        return null;
    }
}
