#pragma once
#include <math.h>
#include <vector>

using namespace std;

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
	static void SplitChannels_ColorToRGB(unsigned char* out_R,
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

public:
	static void MaskingImage3x3(unsigned char* image_input,
		const int width, const int height, float mask[3][3]);
	static void MaskingImage5x5(unsigned char* image_input,
		const int width, const int height, float mask[5][5]);
	static void AveragingImageUsingMask(unsigned char* image_color,
		const int width, const int height);
	static void SobelMasking(unsigned char* image_color,
		const int width, const int height);
	static void LaplacianMasking(unsigned char* image_color,
		const int width, const int height);
	static void GausianMasking(unsigned char* image_color,
		const int width, const int height);
	static void MedianFilterSingleChannel(unsigned char* image_input,
		const int width, const int height, int ksize, int sortMethod);
	static void MedianFilter(unsigned char* image_color,
		const int width, const int height, int ksize, char* sortMethod);

public:
	static void MyRadixSort(vector<unsigned char>& arr);
	static int MedianOfMedians(unsigned char *v, int n, int k);
	static void mySwap(unsigned char& num1, unsigned char& num2);
};

