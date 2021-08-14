import sys
import math
import struct

PIXEL_ARRAY_OFFSET = 62
WIDTH_RANGE = (18, 22)
HEIGHT_RANGE = (22, 26)
TO_ROWSIZE = lambda x: math.ceil(x/32) * 4
PIXELS_PER_BYTE = 8

packed = []
with open(sys.argv[1], 'rb') as bmp:
    header = bmp.read(PIXEL_ARRAY_OFFSET)
    width = struct.unpack("I",
        header[WIDTH_RANGE[0]:WIDTH_RANGE[1]])[0]
    height = struct.unpack("I",
        header[HEIGHT_RANGE[0]:HEIGHT_RANGE[1]])[0]
    rowsize = TO_ROWSIZE(width)
    rows = []
    for i in range(height):
        rows.append(bmp.read(rowsize))
    rows.reverse()
    # One-liner to print image to console
    # print('\n'.join([''.join([f'{i:08b}' for i in row]) for row in rows]))
    for i in range(0, height, PIXELS_PER_BYTE):
        for j in range(width):
            curr_col = math.floor(j/PIXELS_PER_BYTE)
            curr_bit = PIXELS_PER_BYTE - 1 - j % PIXELS_PER_BYTE
            curr_byte = 0
            for k in range(i, i + PIXELS_PER_BYTE):
                if k > len(rows):
                    break
                curr_byte |=((rows[k][curr_col] >> curr_bit) & 0b1) << (k % PIXELS_PER_BYTE)
            packed.append(~curr_byte & 0xFF)
print(', '.join([f'0x{i:02X}' for i in packed]))