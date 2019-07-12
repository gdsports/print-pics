#!/bin/bash
# Use ImageMagick to convert any image file for printing
# on a small receipt printer.
# -resize "384x" -- 58mm printer is 384 dots wide
# -alpha off -- alpha channel off
# -dither FloydSteinberg -- approximate gray tones using dots
# -colorspace gray -colors 2 -- 2 gray colors means black and white
# -normalize -- expand contrast
# If the picture is too light or too dark use a paint program such
# as PhotoShop or GIMP for more control.
# If convert is not present do,
#  $ sudo apt-get install imagemagick
convert $1 -resize "384x" -alpha off -dither FloydSteinberg -colorspace gray -colors 2 -normalize bitmap.png
./bitmapband.py >$1.h
