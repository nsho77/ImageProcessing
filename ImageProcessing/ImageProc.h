#pragma once

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
	static void Binarization(unsigned char* image_gray,
		const int width, const int height, const unsigned char threshold);

public:
	static void UserMasking1(unsigned char* image_color,
		const int width, const int height);

public:
	static void BinaryErosion(unsigned char* image_gray,
		const int width, const int height, unsigned char ksize);
	static void BinaryDilation(unsigned char* image_gray,
		const int width, const int height, unsigned char ksize);
};
