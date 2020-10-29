from bridgeutils import *

selector = JavaObject(('com/android/uiautomator/core/UiSelector', '()V'), reflect=True)
selector = selector.call_object_method(
    'resourceId',
    '(Ljava/lang/String;)Lcom/android/uiautomator/core/UiSelector;',
    ('com.tencent.mobileqq:id/fun_btn',)
)
selector = JavaObject(
    ('mfc/automator/Selector', '(Lcom/android/uiautomator/core/UiSelector;)V', (selector,)),
    reflect=True
)
obj = selector.call_object_method(
    'findOne',
    '()Lmfc/automator/AccessibilityNode;'
)
obj.call_boolean_method(
    'click',
    '()Z'
)
