import usb.core
import usb.util
import time

SEARCH_PARAMS = {'idVendor': 0x1466, 'idProduct': 0x4034}
DISPLAY_STATS_REQUEST = [0x21, 0x80]
READ_STATS_REQUEST = [0xA1, 0x81]
MSG = [0xA0, 0x12, 0x43, 0x56, 0x78, 0x90, 0, 0]

dev = usb.core.find(**SEARCH_PARAMS)
assert dev is not None
dev.set_configuration()
t = time.time()
assert dev.ctrl_transfer(*DISPLAY_STATS_REQUEST, 0, 0, MSG) == len(MSG)
ret = dev.ctrl_transfer(*READ_STATS_REQUEST, 0, 0, len(MSG)).tolist()
assert ret == MSG
print(ret, time.time() - t)