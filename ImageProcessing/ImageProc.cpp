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

void ImageProc::MergeChannels_RGBToColor(unsigned char* in_R,
	unsigned char* in_G, unsigned char* in_B, unsigned char* out_color,
	const int width, const int height)
{
	out_color = new unsigned char[width*height];
	for (int i = 0; i < width*height; i++)
	{
		out_color[i * 4 + 0] = in_B[i];
		out_color[i * 4 + 1] = in_G[i];
		out_color[i * 4 + 2] = in_R[i];
	}
}

void ImageProc::MergeChannels_ColorToRGB(unsigned char* out_R,
	unsigned char* out_G, unsigned char* out_B, unsigned char* in_color,
	const int width, const int height)
{
	out_R = new unsigned char[width*height];
	out_G = new unsigned char[width*height];
	out_B = new unsigned char[width*height];

	for (int i = 0; i < width*height; i++)
	{
		out_B[i] = in_color[i * 4 + 0];
		out_G[i] = in_color[i * 4 + 1];
		out_R[i] = in_color[i * 4 + 2];
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

void ImageProc::AdaptiveBinarization(unsigned char* image_gray,
	const int width, const int height, int ksize)
{
	int neighbor = ksize / 2;
	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_gray, sizeof(unsigned char)*width*height);

#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			float avg = 0.f;
			int cnt = 0;
			for (int y = -neighbor; y <= neighbor; y++)
			{
				for (int x = -neighbor; x <= neighbor; x++)
				{
					if (i+x < 0 || i+x >= width || j+y < 0 || j+y >= height)
						continue;
					cnt++;
					avg += image_gray[width*(j + y) + (i + x)];
				}
			}
			avg = avg / cnt;
			if (image_gray[width*j + i] < avg)
				temp[width*j + i] = 0;
			else
				temp[width*j + i] = 255;
		}
	}

	memcpy(image_gray, temp, sizeof(unsigned char)*width*height);
	delete[] temp;
}

void ImageProc::BinaryMasking(unsigned char* image_color,
	unsigned char* image_binary, const int width, const int height)
{
	for (int i = 0; i < width*height; i++)
	{
		if (image_binary[i] == 0)
		{
			image_color[i * 4 + 0] = 0;
			image_color[i * 4 + 1] = 0;
			image_color[i * 4 + 2] = 0;
		}
	}
}

void ImageProc::UserMasking1(unsigned char* image_color,
	const int width, const int height)
{
	float* mask = new float[width*height];

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mask[width*j + i] = static_cast<float>(width - i) / static_cast<float>(width);
		}
	}

	for (int i = 0; i < width*height; i++)
	{
		image_color[i * 4 + 0] *= mask[i];
		image_color[i * 4 + 1] *= mask[i];
		image_color[i * 4 + 2] *= mask[i];
	}

	delete[] mask;
}

void ImageProc::UserMaskingCircle(unsigned char* image_color,
	const int width, const int height)
{
	float* mask = new float[width*height];
	int centerX = width / 2;
	int centerY = height / 2;
	float distanceFromCenterX = 0.f;
	float distanceFromCenterY = 0.f;
	float distanceFromCenter = 0.f;

	// 중심으로 부터 거리를 구하고
	// 거리에 비례해 mask 값을 채운다.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			distanceFromCenterX = fabsf((static_cast<float>(i) - centerX));
			distanceFromCenterY = fabsf((static_cast<float>(j) - centerY));
			distanceFromCenter = sqrt((distanceFromCenterX*distanceFromCenterX)
				+ (distanceFromCenterY*distanceFromCenterY));

			mask[width*j + i] = ((width / 2) - distanceFromCenter) / (width / 2);
			if (mask[width*j + i] < 0)
				mask[width*j + i] = 0;
		}
	}

	for (int i = 0; i < width*height; i++)
	{
		image_color[i * 4 + 0] *= mask[i];
		image_color[i * 4 + 1] *= mask[i];
		image_color[i * 4 + 2] *= mask[i];
	}

	delete[] mask;
}

void ImageProc::UserMaskingUpDown(unsigned char* image_color,
	const int width, const int height)
{
	float* mask = new float[width*height];

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mask[width*j + i] = static_cast<float>(height - j) / 
				static_cast<float>(height);
		}
	}

	for (int i = 0; i < width*height; i++)
	{
		image_color[i * 4 + 0] *= mask[i];
		image_color[i * 4 + 1] *= mask[i];
		image_color[i * 4 + 2] *= mask[i];
	}

	delete[] mask;
}

void ImageProc::UserMaskingDivide(unsigned char* image_color,
	const int width, const int height)
{
	unsigned char* mask = new unsigned char[width*height];
	int changePoint = height / 16;
	int bZero = -1;

	for (int j = 0; j<height; j++)
	{
		if (j % changePoint == 0)
			bZero *= (-1);

		for (int i = 0; i<width; i++)
		{
			if (bZero == 1)
				mask[width*j + i] = 0;
			else
				mask[width*j + i] = 1;
		}
	}

	for (int i = 0; i<width*height; i++)
	{
		image_color[i * 4 + 0] *= mask[i];
		image_color[i * 4 + 1] *= mask[i];
		image_color[i * 4 + 2] *= mask[i];
	}

	delete[] mask;

}

void ImageProc::UserMaskingCUD(unsigned char* image_color,
	const int width, const int height)
{
	// mask_circle
	float* mask_circle = new float[width*height];
	int centerX = width / 2;
	int centerY = height / 2;
	float distanceFromCenterX = 0.f;
	float distanceFromCenterY = 0.f;
	float distanceFromCenter = 0.f;

	// 중심으로 부터 거리를 구하고
	// 거리에 비례해 mask 값을 채운다.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			distanceFromCenterX = fabsf((static_cast<float>(i) - centerX));
			distanceFromCenterY = fabsf((static_cast<float>(j) - centerY));
			distanceFromCenter = sqrt((distanceFromCenterX*distanceFromCenterX)
				+ (distanceFromCenterY*distanceFromCenterY));

			mask_circle[width*j + i] = ((width / 2) - distanceFromCenter) / (width / 2);
			if (mask_circle[width*j + i] < 0)
				mask_circle[width*j + i] = 0;
		}
	}


	// mask_updown
	float* mask_updown = new float[width*height];

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			mask_updown[width*j + i] = static_cast<float>(height - j) /
				static_cast<float>(height);
		}
	}

	// mask_divide
	unsigned char* mask_divide = new unsigned char[width*height];
	int changePoint = height / 16;
	int bZero = -1;

	for (int j = 0; j<height; j++)
	{
		if (j % changePoint == 0)
			bZero *= (-1);

		for (int i = 0; i<width; i++)
		{
			if (bZero == 1)
				mask_divide[width*j + i] = 0;
			else
				mask_divide[width*j + i] = 1;
		}
	}

	// masking
	for (int i = 0; i < width*height; i++)
	{
		image_color[i * 4 + 0] *= mask_updown[i];
		image_color[i * 4 + 1] *= mask_divide[i];
		image_color[i * 4 + 2] *= mask_circle[i];
	}

	delete[] mask_circle;
	delete[] mask_updown;
	delete[] mask_divide;
}