#pragma once
#include <math.h>

class ImageProc
{
public:
	ImageProc();
	~ImageProc();

public:
	static void AdjustBright(unsigned char* image,
		const int width, const int height, const int bright);
	static void AdjustContrast(unsigned char* image,
		const int width, const int height, const int contrast);
	static void MergeChannels(unsigned char* in_color,
		unsigned char* out_gray, const int width, const int height);
	static void MergeChannels_RGBToColor(unsigned char* in_R,
		unsigned char* in_G, unsigned char* in_B, unsigned char* out_color,
		const int width, const int height);
	static void MergeChannels_ColorToRGB(unsigned char* out_R,
		unsigned char* out_G, unsigned char* out_B, unsigned char* in_color,
		const int width, const int height);
	static void Binarization(unsigned char* image_gray,
		const int width, const int height, const unsigned char threshold);

public:
	static void BinaryMasking(unsigned char* image_color,
		unsigned char* image_binary, const int width, const int height);
	static void UserMasking1(unsigned char* image_color,
		const int width, const int height);
	static void UserMaskingCircle(unsigned char* image_color,
		const int width, const int height);
	static void UserMaskingUpDown(unsigned char* image_color,
		const int width, const int height);
	static void UserMaskingDivide(unsigned char* image_color,
		const int width, const int height);
	static void UserMaskingCUD(unsigned char* image_color,
		const int width, const int height);

public:
	static void BinaryErosion(unsigned char* image_gray,
		const int width, const int height, unsigned char ksize);
	static void BinaryDilation(unsigned char* image_gray,
		const int width, const int height, unsigned char ksize);
	static void AdaptiveBinarization(unsigned char* image_gray,
		const int width, const int height, int ksize);
};
