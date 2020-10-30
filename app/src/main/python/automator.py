from bridgeutils import *

signs = {
    int: 'I',
    str: 'Ljava/lang/String;',
    bool: 'Z'
}


class UiSelector(object):
    def __init__(self, filters):
        pre_sel = JavaObject(('com/android/uiautomator/core/UiSelector', '()V'), reflect=True)
        for name in filters:
            pre_sel = pre_sel.call_object_method(
                name,
                f'({signs[type(filters[name])]})Lcom/android/uiautomator/core/UiSelector;',
                (filters[name],)
            )
        self.selector = selector = JavaObject(
            ('mfc/automator/Selector', '(Lcom/android/uiautomator/core/UiSelector;)V', (pre_sel,)),
            reflect=True
        )

    def findOne(self):
        pass
