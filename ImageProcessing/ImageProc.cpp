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

void ImageProc::BinaryErosion(unsigned char* image_gray,
	const int width, const int height, unsigned char ksize)
{
	// ksize 는 홀수로 받아서 양옆 neighbor 만큼 본다.
	unsigned char neighbor = ksize / 2;

	// 침식이미지를 저장할 버퍼를 생성한다.
	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_gray, sizeof(unsigned char)*width*height);

	for (int j = 0; j<height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			//현재 픽셀이 검은색이면 건너뛴다.
			if (image_gray[width*j + i] == 0)
				continue;

			// 주변 픽셀이 검은색인지 확인
			bool bBlack = false;

			for (int l = j - neighbor; l< j + 1 + neighbor; l++)
			{
				for (int k = i - neighbor; k<i + 1 + neighbor; k++)
				{
					if (k < 0 || k >= width || l < 0 || l >= height)
						continue;

					if (image_gray[width*l + k] == 0)
					{
						temp[width*j + i] = 0;
						bBlack = true;
						break;
					}
				}
				if (bBlack)
					break;
			}
		}
	}

	memcpy(image_gray, temp, sizeof(unsigned char)*width*height);
	delete[] temp;
}

void ImageProc::BinaryDilation(unsigned char* image_gray,
	const int width, const int height, unsigned char ksize)
{
	// ksize 는 홀수로 받아서 양옆 neighbor 만큼 본다.
	unsigned char neighbor = ksize / 2;

	// 팽창 이미지를 저장할 버퍼를 생성한다.
	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_gray, sizeof(unsigned char)*width*height);

	for (int j = 0; j<height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			//현재 픽셀이 하얀색이면 건너뛴다.
			if (image_gray[width*j + i] == 255)
				continue;

			// 주변 픽셀이 하얀색인지 확인
			bool bWhite = false;

			for (int l = j - neighbor; l< j + 1 + neighbor; l++)
			{
				for (int k = i - neighbor; k<i + 1 + neighbor; k++)
				{
					if (k < 0 || k >= width || l < 0 || l >= height)
						continue;

					if (image_gray[width*l + k] == 255)
					{
						temp[width*j + i] = 255;
						bWhite = true;
						break;
					}
				}
				if (bWhite)
					break;
			}
		}
	}

	memcpy(image_gray, temp, sizeof(unsigned char)*width*height);
	delete[] temp;
}

void ImageProc::UserMasking1(unsigned char* image_color,
	const int width, const int height)
{
	float* mask = new float[width*height];

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mask[width*j + i] = static_cast<float>(i) / static_cast<float>(width);
		}
	}

	for (int i = 0; i < width*height; i++)
	{
		image_color[i * 4 + 0] *= mask[i];
		image_color[i * 4 + 1] *= mask[i];
		image_color[i * 4 + 2] *= mask[i];
	}
}