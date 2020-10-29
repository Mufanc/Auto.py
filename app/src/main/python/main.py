from bridgeutils import *
from sys import stdout

test = JavaObject(('java/lang/Integer', '(I)V', (123,)), reflect=True)
print(test.call_int_method('intValue', '()I'))
print('???', type(test))
stdout.flush()
test = JavaObject(('mfc/autopy/Test', '(I)V', (test,)), reflect=True)
print(test)
print('Finish')
stdout.flush()