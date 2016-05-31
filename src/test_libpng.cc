/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng15 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

// compile add -lpng

#include <stdlib.h>
#include <stdio.h>
#include <png++/png.hpp>


int main(int argc, char *argv[]) {

//png::image< png::rgb_pixel > image(128, 128);
png::image< png::gray_pixel > image(128, 128);
for (size_t y = 0; y < image.get_height(); ++y)
{
   for (size_t x = 0; x < image.get_width(); ++x)
   {
       //image[y][x] = png::rgb_pixel(x, y, x + y);
       image[y][x] = png::gray_pixel(x);
       // non-checking equivalent of image.set_pixel(x, y, ...);
   }
}
image.write("rgb.png");
return 0;

}