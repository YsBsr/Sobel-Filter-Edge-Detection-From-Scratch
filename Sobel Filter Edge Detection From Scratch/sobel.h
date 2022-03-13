#pragma once

#include <limits>
#include <vector>

#include "sobel_types.h"

// COLOR
double get_red(int rgb);
double get_green(int rgb);
double get_blue(int rgb);
double get_gray(int rgb);
int get_RGB(double red, double green, double blue);
int get_RGB(double gray);
int max(int a, int b);
GrayImage to_gray(const RGBImage &cimage);
RGBImage to_RGB(const GrayImage &gimage);

// FILTER
inline void clamp(long int& val, long max);

GrayImage filter(const GrayImage &gray, const Kernel &kernel);
GrayImage smooth(const GrayImage &gray);
GrayImage sobelX(const GrayImage &gray);
GrayImage sobelY(const GrayImage &gray);
GrayImage sobel(const GrayImage &gray);