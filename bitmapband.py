#!/usr/bin/python
# Convert bitmap.png file to column oriented format for ESC POS ESC * command.
# The output is C source code to be included in a program that knows how to
# write data to a printer with the ESC POS command set.
#
# https://reference.epson-biz.com/modules/ref_escpos/index.php
#
# On my cheap printer, using single density and default size, the bitmap
# is stretched about 50% in the vertical direction.
# Using double density and "--size 6"
from PIL import Image
density = 2 # 2= double density, 1= single density
im_orig = Image.open("bitmap.png")
cols_orig, rows_orig = im_orig.size
# Fix aspect ratio problem in single density mode.
if density == 1 :
    bandsize = 8
    pixels_orig = im_orig.load()
    im = Image.new('1', (cols_orig, (rows_orig*2)/3), 0xFFFFFF)
    pixels_new = im.load()
    row_new = 0
    for r in xrange(rows_orig) :
        if (r % 3) != 0 :
            for c in xrange(cols_orig) :
                pixels_new[c, row_new] = pixels_orig[c, r]
            row_new += 1
else :
    bandsize = 24
    im = im_orig

bandnumber=0
cols, rows = im.size
print "#define bitmap_width   " + str(cols)
print "#define bitmap_height  " + str(rows)
print "#define bitmap_density " + str(density)
print
print "const uint8_t bitmap_data[] PROGMEM = {"
for i in range(0, rows, bandsize) :
    box = (0, i, cols, min(i+bandsize, rows))
    band = im.crop(box)
    bandnumber += 1
    outbyte = 0;
    outcount = 0;
    band_cols, band_rows = band.size
    pixels = band.load()
    for col in range(0, band_cols) :
        for row in range(0, bandsize) :
            if row >= band_rows :
                outbyte = outbyte << 1
            else :
                outbyte = outbyte << 1 | ((~pixels[col, row]) & 1)
            outcount = outcount + 1
            if outcount == 8 :
                print str(outbyte) + ",",
                outcount = 0
                outbyte = 0
        if outcount != 0 :
            outbyte = outbyte << (8-outcount)
            print str(outbyte) + ",",
            outcount = 0
            outbyte = 0
    print

print "};"
quit()
