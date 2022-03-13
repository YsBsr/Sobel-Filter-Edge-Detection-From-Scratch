#include <cassert>
#include <limits>
#include <tgmath.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "sobel.h"



// Returns red component (in the scale 0.0-1.0) from given RGB color.
double get_red(int rgb)
{
	double red = (rgb >> 16) & 0x0ff;

	return red / 255;
}

// Returns green component (in the scale 0.0-1.0) from given RGB color.
double get_green(int rgb)
{
	double green = (rgb >> 8) & 0x0ff;

	return green / 255;
}

// Returns blue component (in the scale 0.0-1.0) from given RGB color.
double get_blue(int rgb)
{
	double blue = (rgb) & 0x0ff;

	return blue / 255;
}

// Returns the average of red, green and blue components from given RGB color. (Scale: 0.0-1.0)
double get_gray(int rgb)
{
	return (get_red(rgb) + get_green(rgb) + get_blue(rgb)) / 3;
}

// Returns the RGB value of the given red, green and blue components.
int get_RGB(double red, double green, double blue)
{
	return (red + green + blue) * 255;
}

// Returns the RGB components from given grayscale value (between 0.0 and 1.0).
int get_RGB(double gray)
{
	int x = 0xff * gray;
	return ((x << 16) | (x << 8) | x);
}

// Converts  RGB image to grayscale double image.
GrayImage to_gray(const RGBImage& cimage)
{
	GrayImage gray_image = {};
	std::vector<double> pixels = {};
	for (size_t row = 0; row < cimage.size(); row++)
	{
		for (size_t pixel = 0; pixel < cimage[row].size(); pixel++)
		{
			pixels.push_back(get_gray(cimage[row][pixel]));
		}
		gray_image.push_back(pixels);
		pixels.clear();
	}
	return gray_image;
}

// Converts grayscale double image to an RGB image.
RGBImage to_RGB(const GrayImage& gimage)
{
	RGBImage rgbimage = {};
	std::vector <int> pixels = {};
	for (size_t row = 0; row < gimage.size(); row++) {
		for (size_t pixel = 0; pixel < gimage[row].size(); pixel++) {

			pixels.push_back(get_RGB(gimage[row][pixel]));

		}
		rgbimage.push_back(pixels);
		pixels.clear();
	}
	return rgbimage;
}

// Get a pixel without accessing out of bounds
// return nearest valid pixel color
void clamp(long int& val, long max)
{
	if (val < 0) {
		val = 0;
	}
	if (val >= max) {
		val = max;
	}
}

// Convolve a single-channel image with the given kernel.
GrayImage filter(const GrayImage& gray, const Kernel& kernel)
{
	GrayImage filtered = {};
	GrayImage clamped = {};
	std::vector <double> pixels = {};
	double conv_step = 0;

	int im_row_length = (int)gray.size();
	int im_col_length = (int)gray[0].size();

	long int dummy_im_row = 0;
	long int dummy_im_col = 0;

	for (int im_row = 0; im_row < im_row_length + 2; im_row++) {
		dummy_im_row = im_row - 1;
		clamp(dummy_im_row, im_row_length - 1);
		for (int im_col = 0; im_col < im_col_length + 2; im_col++) {
			dummy_im_col = im_col - 1;
			clamp(dummy_im_col, im_col_length - 1);
			pixels.push_back(gray[dummy_im_row][dummy_im_col]);
		}
		clamped.push_back(pixels);
		pixels.clear();
	}

	int kernel_row_length = (int)kernel.size();
	int kernel_col_length = (int)kernel[0].size();

	int cl_row = 0;
	int cl_col = 0;

	int dummy_cl_col = 0;

	while (cl_row < im_row_length) {

		if (cl_col == 0) {
			dummy_cl_col = cl_col;

			while (dummy_cl_col < im_col_length) {
				cl_col = dummy_cl_col;
				for (int k_row = 0; k_row < kernel_row_length; k_row++) {
					for (int k_col = 0; k_col < kernel_col_length; k_col++) {
						conv_step += kernel[k_row][k_col] * clamped[cl_row][cl_col];
						cl_col++;
					}
					cl_row++;
					cl_col -= kernel_col_length;
				}
				dummy_cl_col++;
				cl_row -= kernel_row_length;
				pixels.push_back(conv_step);
				conv_step = 0;
			}
		}
		cl_row++;
		cl_col = 0;
		filtered.push_back(pixels);
		pixels.clear();
	}

	return filtered;
}

// Smooth a single-channel image
GrayImage smooth(const GrayImage& gray)
{

	Kernel smoothing_kernel = { {0.1, 0.1, 0.1},
								{0.1, 0.2, 0.1},
								{0.1, 0.1, 0.1} };

	GrayImage smoothed_img(filter(gray, smoothing_kernel));

	return smoothed_img;
}

// Compute horizontal Sobel filter
GrayImage sobelX(const GrayImage& gray)
{

	Kernel sobelX_kernel = { {-1.0, 0.0, 1.0},
							 {-2.0, 0.0, 2.0},
							 {-1.0, 0.0, 1.0} };

	GrayImage sobelX_img(filter(gray, sobelX_kernel));

	return sobelX_img;
}

// Compute vertical Sobel filter
GrayImage sobelY(const GrayImage& gray)
{

	Kernel sobelY_kernel = { {-1.0, -2.0, -1.0},
							 { 0.0, 0.0, 0.0 },
							 { 1.0, 2.0, 1.0 } };

	GrayImage sobelY_img(filter(gray, sobelY_kernel));

	return sobelY_img;
}

// Compute the magnitude of combined Sobel filters
GrayImage sobel(const GrayImage& gray)
{
	GrayImage sobel_x(sobelX(gray));
	GrayImage sobel_y(sobelY(gray));

	GrayImage sobel = {};
	std::vector <double> pixels = {};

	int length_x = sobel_x.size();
	int length_y = sobel_y[0].size();

	double x = 0;
	double y = 0;
	double square_root = 0;

	for (size_t i = 0; i < length_x; i++)
	{
		for (size_t j = 0; j < length_y; j++)
		{
			x = sobel_x[i][j] * sobel_x[i][j];
			y = sobel_y[i][j] * sobel_y[i][j];
			square_root = sqrt(x + y);
			pixels.push_back(square_root);
		}
		sobel.push_back(pixels);
		pixels.clear();
	}

	return sobel;
}