import usb.core
import usb.util

SEARCH_PARAMS = {'idVendor': 0x03EB, 'idProduct': 0x204F}
MSG = 'abcdefgh'
DISPLAY_STATS_REQUEST = [0x21, 0x80]
READ_STATS_REQUEST = [0xA1, 0x81]

dev = usb.core.find(**SEARCH_PARAMS)
assert dev is not None
dev.set_configuration()
assert dev.ctrl_transfer(*DISPLAY_STATS_REQUEST, 0, 0, MSG) == len(MSG)
ret = ''.join([chr(x) for x in dev.ctrl_transfer(*READ_STATS_REQUEST, 0, 0, 8)])
assert ret == MSG
print(ret)
