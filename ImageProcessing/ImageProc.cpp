#include "stdafx.h"
#include "ImageProc.h"


ImageProc::ImageProc()
{

}

ImageProc::~ImageProc()
{

}

void ImageProc::MergeChannels(unsigned char* in_color,
	unsigned char* out_gray, const int width, const int height)
{
	for (int i = 0; i<width*height; i++)
	{
		out_gray[i] = (in_color[i * 4 + 0] + in_color[i * 4 + 1] + in_color[i * 4 + 2]) / 3;
	}
}