#include "stdafx.h"
#include "ImageProc.h"


ImageProc::ImageProc()
{

}

ImageProc::~ImageProc()
{

}

void ImageProc::AdjustBright(unsigned char* image,
	const int width, const int height, const int bright)
{
	for (int i = 0; i < width*height; i++)
	{
		image[i * 4 + 0] = static_cast<char>(min(255, max(0, image[i * 4 + 0] + bright)));
		image[i * 4 + 1] = static_cast<char>(min(255, max(0, image[i * 4 + 1] + bright)));
		image[i * 4 + 2] = static_cast<char>(min(255, max(0, image[i * 4 + 2] + bright)));
	}
}


void ImageProc::AdjustContrast(unsigned char* image,
	const int width, const int height, const int contrast)
{
	for (int i = 0; i < width*height; i++)
	{
		image[i * 4 + 0] = static_cast<char>(min(255, max(0, image[i * 4 + 0] * contrast)));
		image[i * 4 + 1] = static_cast<char>(min(255, max(0, image[i * 4 + 1] * contrast)));
		image[i * 4 + 2] = static_cast<char>(min(255, max(0, image[i * 4 + 2] * contrast)));
	}
}

void ImageProc::MergeChannels(unsigned char* in_color,
	unsigned char* out_gray, const int width, const int height)
{
	for (int i = 0; i<width*height; i++)
	{
		out_gray[i] = (in_color[i * 4 + 0] + in_color[i * 4 + 1] + in_color[i * 4 + 2]) / 3;
	}
}

void ImageProc::Binarization(unsigned char* image_gray,
	const int width, const int height, const unsigned char threshold)
{
	for (int i = 0; i < width*height; i++)
	{
		if (image_gray[i] < threshold)
			image_gray[i] = 0;
		else
			image_gray[i] = 255;
	}
}