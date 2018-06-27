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
	for (int i = 0; i < width*height; i++)
	{
		out_color[i * 4 + 0] = in_B[i];
		out_color[i * 4 + 1] = in_G[i];
		out_color[i * 4 + 2] = in_R[i];
		out_color[i * 4 + 3] = 0;
	}
}

void ImageProc::SplitChannels_ColorToRGB(unsigned char* out_R,
	unsigned char* out_G, unsigned char* out_B, unsigned char* in_color,
	const int width, const int height)
{
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

void ImageProc::MaskingImage3x3(unsigned char* image_input,
	const int width, const int height, float mask[3][3])
{
	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_input, sizeof(unsigned char)*width*height);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			float sum = 0.f;
			for (int y = -1; y <= 1; y++)
			{
				for (int x = -1; x <= 1; x++)
				{
					if ((x + i) >= width || (x + i) < 0 ||
						(y + j) >= height || (y + j) < 0) continue;

					sum += image_input[width*(j + y) + (x + i)] 
						* mask[y + 1][x + 1];
				}
			}
			temp[width*j + i] = static_cast<unsigned char>(sum);
		}
	}

	memcpy(image_input, temp, sizeof(unsigned char)*width*height);
	delete[] temp;

}

void ImageProc::MaskingImage5x5(unsigned char* image_input,
	const int width, const int height, float mask[5][5])
{
	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_input, sizeof(unsigned char)*width*height);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			float sum = 0.f;
			for (int y = -2; y <= 2; y++)
			{
				for (int x = -2; x <= 2; x++)
				{
					if ((x + i) >= width || (x + i) < 0 ||
						(y + j) >= height || (y + j) < 0) continue;

					sum += image_input[width*(j + y) + (x + i)]
						* mask[y + 2][x + 2];
				}
			}
			temp[width*j + i] = static_cast<unsigned char>(sum);
		}
	}

	memcpy(image_input, temp, sizeof(unsigned char)*width*height);
	delete[] temp;

}

void ImageProc::AveragingImageUsingMask(unsigned char* image_color,
	const int width, const int height)
{
	/*float mask[3][3] =
	{
		{ 1.f / 9.f, 1.f / 9.f, 1.f / 9.f },
		{ 1.f / 9.f, 1.f / 9.f, 1.f / 9.f },
		{ 1.f / 9.f, 1.f / 9.f, 1.f / 9.f }
	};*/

	float mask[5][5] = {
	{ 1.f / 25.f , 1.f / 25.f , 1.f / 25.f, 1.f / 25.f, 1.f / 25.f },
	{ 1.f / 25.f , 1.f / 25.f , 1.f / 25.f, 1.f / 25.f, 1.f / 25.f },
	{ 1.f / 25.f , 1.f / 25.f , 1.f / 25.f, 1.f / 25.f, 1.f / 25.f },
	{ 1.f / 25.f , 1.f / 25.f , 1.f / 25.f, 1.f / 25.f, 1.f / 25.f },
	{ 1.f / 25.f , 1.f / 25.f , 1.f / 25.f, 1.f / 25.f, 1.f / 25.f }
	};

	unsigned char* image_R = new unsigned char[width*height];
	unsigned char* image_G = new unsigned char[width*height];
	unsigned char* image_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(image_R, image_G, image_B, image_color, width, height);

	/*MaskingImage3x3(image_R, width, height, mask);
	MaskingImage3x3(image_G, width, height, mask);
	MaskingImage3x3(image_B, width, height, mask);*/
	MaskingImage5x5(image_R, width, height, mask);
	MaskingImage5x5(image_G, width, height, mask);
	MaskingImage5x5(image_B, width, height, mask);

	MergeChannels_RGBToColor(image_R,image_G,image_B,image_color,width,height);

	delete[] image_R;
	delete[] image_G;
	delete[] image_B;
}

void ImageProc::SobelMasking(unsigned char* image_color,
	const int width, const int height)
{
	float mask_sobel[5][5] = {
		{ -1.f, -1.f, 0.f, 1.f, 1.f },
		{ -1.f, -1.f, 0.f, 1.f, 1.f },
		{ -2.f, -2.f, 0.f, 2.f, 2.f },
		{ -1.f, -1.f, 0.f, 1.f, 1.f },
		{ -1.f, -1.f, 0.f, 1.f, 1.f },
	};

	unsigned char* image_R = new unsigned char[width*height];
	unsigned char* image_G = new unsigned char[width*height];
	unsigned char* image_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(image_R, image_G, image_B, image_color, width, height);

	MaskingImage5x5(image_R, width, height, mask_sobel);
	MaskingImage5x5(image_G, width, height, mask_sobel);
	MaskingImage5x5(image_B, width, height, mask_sobel);

	MergeChannels_RGBToColor(image_R, image_G, image_B, image_color, width, height);

	delete[] image_R;
	delete[] image_G;
	delete[] image_B;
}

void ImageProc::LaplacianMasking(unsigned char* image_color,
	const int width, const int height)
{
	float mask_laplacian[5][5] = {
		{ -4.f, -1.f, 0.f, -1.f, -4.f },
		{ -1.f,  2.f, 3.f,  2.f, -1.f },
		{  0.f,  3.f, 4.f,  3.f,  0.f },
		{ -1.f,  2.f, 3.f,  2.f, -1.f },
		{ -4.f, -1.f, 0.f, -1.f, -4.f },
	};

	unsigned char* image_R = new unsigned char[width*height];
	unsigned char* image_G = new unsigned char[width*height];
	unsigned char* image_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(image_R, image_G, image_B, image_color, width, height);

	MaskingImage5x5(image_R, width, height, mask_laplacian);
	MaskingImage5x5(image_G, width, height, mask_laplacian);
	MaskingImage5x5(image_B, width, height, mask_laplacian);

	MergeChannels_RGBToColor(image_R, image_G, image_B, image_color, width, height);

	delete[] image_R;
	delete[] image_G;
	delete[] image_B;
}

void ImageProc::GausianMasking(unsigned char* image_color,
	const int width, const int height)
{
	float mask_gausian[5][5] = {
		{ 1.f / 273.f,	4.f / 273.f,  7.f / 273.f, 4.f  / 273.f, 1.f / 273.f },
		{ 4.f / 273.f, 16.f / 273.f, 26.f / 273.f, 16.f / 273.f, 4.f / 273.f },
		{ 7.f / 273.f, 26.f / 273.f, 41.f / 273.f, 26.f / 273.f, 7.f / 273.f },
		{ 4.f / 273.f, 16.f / 273.f, 26.f / 273.f, 16.f / 273.f, 4.f / 273.f },
		{ 1.f / 273.f,	4.f / 273.f,  7.f / 273.f, 4.f  / 273.f, 1.f / 273.f },
	};

	unsigned char* image_R = new unsigned char[width*height];
	unsigned char* image_G = new unsigned char[width*height];
	unsigned char* image_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(image_R, image_G, image_B, image_color, width, height);

	MaskingImage5x5(image_R, width, height, mask_gausian);
	MaskingImage5x5(image_G, width, height, mask_gausian);
	MaskingImage5x5(image_B, width, height, mask_gausian);

	MergeChannels_RGBToColor(image_R, image_G, image_B, image_color, width, height);

	delete[] image_R;
	delete[] image_G;
	delete[] image_B;
}


void ImageProc::MyRadixSort(vector<unsigned char>& arr)
{
	int bucket[256] = { 0 };

	for (int i = 0; i < arr.size(); i++)
		bucket[arr[i]]++;

	arr.clear();

	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < bucket[i]; j++)
			arr.push_back(i);
	}
}

void ImageProc::mySwap(unsigned char& num1, unsigned char& num2)
{
	int temp = num1;
	num1 = num2;
	num2 = temp;
}

int ImageProc::MedianOfMedians(unsigned char *v, int n, int k) {
	if (n == 1 && k == 0) return v[0];

	int m = (n + 4) / 5;
	unsigned char *medians = new unsigned char[m];
	for (int i = 0; i<m; i++) {
		if (5 * i + 4 < n) {
			unsigned char *w = v + 5 * i;
			for (int j0 = 0; j0<3; j0++) {
				int jmin = j0;
				for (int j = j0 + 1; j<5; j++) {
					if (w[j] < w[jmin]) jmin = j;
				}
				mySwap(w[j0], w[jmin]);
			}
			medians[i] = w[2];
		}
		else {
			medians[i] = v[5 * i];
		}
	}

	int pivot = MedianOfMedians(medians, m, m / 2);
	delete[] medians;

	for (int i = 0; i<n; i++) {
		if (v[i] == pivot) {
			mySwap(v[i], v[n - 1]);
			break;
		}
	}

	int store = 0;
	for (int i = 0; i<n - 1; i++) {
		if (v[i] < pivot) {
			mySwap(v[i], v[store++]);
		}
	}
	mySwap(v[store], v[n - 1]);

	if (store == k) {
		return pivot;
	}
	else if (store > k) {
		return MedianOfMedians(v, store, k);
	}
	else {
		return MedianOfMedians(v + store + 1, n - store - 1, k - store - 1);
	}
}

void ImageProc::MedianFilterSingleChannel(unsigned char* image_input,
	const int width, const int height, int ksize, int sortMethod)
{
	if (ksize % 2 == 0 || ksize == 1) return;
	if (sortMethod == -1)
	{
		printf("sort method error");
		return;
	}

	unsigned char* temp = new unsigned char[width*height];
	memcpy(temp, image_input, sizeof(unsigned char)*width*height);

	int neighbor = ksize / 2;

#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			vector<unsigned char> medianArr;
			medianArr.resize(ksize*ksize);
			for (int x = -neighbor; x <= neighbor; x++)
			{
				for (int y = -neighbor; y <= neighbor; y++)
				{
					if (x + i >= width || x + i < 0 || y + j >= height
						|| y + j < 0) continue;

					medianArr[ksize*(y + neighbor) + (x + neighbor)] =
						image_input[width*(y + j) + (x + i)];
				}
			}

			if (sortMethod == 1)
			{
				//radix sort medianArr
				MyRadixSort(medianArr);
				temp[width*j + i] = medianArr[medianArr.size() / 2];
			}
			else if (sortMethod == 2)
			{
				//median of medians
				temp[width*j + i] = MedianOfMedians(&medianArr[0], medianArr.size(), medianArr.size() / 2);
			}
				
		}
	}

	memcpy(image_input, temp, sizeof(unsigned char)*width*height);
	delete[] temp;
}

void ImageProc::MedianFilter(unsigned char* image_color,
	const int width, const int height, int ksize, char* sortMethod)
{
	int numOfSortMethod = -1;
	if (!strncmp(sortMethod, "RadixSort", strlen(sortMethod)))
		numOfSortMethod = 1;
	if (!strncmp(sortMethod, "MedianOfMedians", strlen(sortMethod)))
		numOfSortMethod = 2;

	printf("numOfSortMethod : %d", numOfSortMethod);

	unsigned char* img_R = new unsigned char[width*height];
	unsigned char* img_G = new unsigned char[width*height];
	unsigned char* img_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(img_R, img_G, img_B, image_color, width, height);

	MedianFilterSingleChannel(img_R, width, height, ksize, numOfSortMethod);
	MedianFilterSingleChannel(img_G, width, height, ksize, numOfSortMethod);
	MedianFilterSingleChannel(img_B, width, height, ksize, numOfSortMethod);

	MergeChannels_RGBToColor(img_R, img_G, img_B, image_color, width, height);

	delete[] img_R;
	delete[] img_G;
	delete[] img_B;

}

void ImageProc::CreateHistogramSingleChannel(unsigned char* image_input,
	unsigned char* histo_output, const int width, const int height,
	const int histo_width, const int histo_height)
{
	float valArr[256] = { 0.f };
	float max = 0.f;
	for (int i = 0; i < width*height; i++)
	{
		valArr[image_input[i]] += 1.f;
		max = __max(valArr[image_input[i]], max);
	}
		
	
	for (int i = 0; i < 256; i++)
	{
		valArr[i] = (valArr[i] / max) * histo_height;
		for (int j = 0; j < valArr[i]; j++)
		{
			histo_output[(histo_height - 1 - j)*histo_width + (i * 2) + 0] = 255;
			histo_output[(histo_height - 1 - j)*histo_width + (i * 2) + 1] = 255;
		}
	}
}

void ImageProc::CreateHistogram(unsigned char* image_input,
	unsigned char* histo_output, const int width, const int height,
	const int histo_width, const int histo_height)
{
	unsigned char* img_R = new unsigned char[width*height];
	unsigned char* img_G = new unsigned char[width*height];
	unsigned char* img_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(img_R, img_G, img_B, image_input, width, height);
	
	unsigned char* histo_R = new unsigned char[histo_width*histo_height];
	unsigned char* histo_G = new unsigned char[histo_width*histo_height];
	unsigned char* histo_B = new unsigned char[histo_width*histo_height];

	SplitChannels_ColorToRGB(histo_R, histo_G, histo_B, histo_output, histo_width, histo_height);

	CreateHistogramSingleChannel(img_R, histo_R, width, height, histo_width, histo_height);
	CreateHistogramSingleChannel(img_G, histo_G, width, height, histo_width, histo_height);
	CreateHistogramSingleChannel(img_B, histo_B, width, height, histo_width, histo_height);

	MergeChannels_RGBToColor(histo_R, histo_G, histo_B, histo_output, histo_width, histo_height);

	delete[] img_R;
	delete[] img_G;
	delete[] img_B;
	delete[] histo_R;
	delete[] histo_G;
	delete[] histo_B;
}

void ImageProc::CreateCumulativeHistogramSingleChannel(unsigned char* image_input,
	unsigned char* image_output, const int width, const int height)
{
	// 1. 히스토그램을 만든다.
	float histogram[256] = { 0.f };
	for (int i = 0; i < width*height; i++)
		histogram[image_input[i]]++;

	// 2. 누적히스토그램을 만든다.
	float cumulative_histogram[256] = { 0.f };
	float sum = 0.f;
	for (int i = 0; i < 256; i++)
	{
		sum += histogram[i];
		cumulative_histogram[i] = sum;
	}

	// 누적히스토그램을 정규화한다.
	// 정규화 = (값 / 최대값(이미지 사이즈)) * 픽셀최대값(255)
	for (int i = 0; i < 256; i++)
		cumulative_histogram[i] = (cumulative_histogram[i] / (width * height)) * 255 + 0.5f;

	// 3. 이미지에 적용한다.
	for (int i = 0; i < width*height; i++)
		image_output[i] = static_cast<unsigned char>(cumulative_histogram[image_input[i]]);

}

void ImageProc::CreateHistogramEqualization(unsigned char* image_input,
	unsigned char* image_output, const int width, const int height)
{
	unsigned char* img_R = new unsigned char[width*height];
	unsigned char* img_G = new unsigned char[width*height];
	unsigned char* img_B = new unsigned char[width*height];

	SplitChannels_ColorToRGB(img_R, img_G, img_B, image_input, width, height);

	unsigned char* equal_R = new unsigned char[width*height];
	unsigned char* equal_G = new unsigned char[width*height];
	unsigned char* equal_B = new unsigned char[width*height];

	//SplitChannels_ColorToRGB(equal_R, equal_G, equal_B, image_output, width, height);

	CreateCumulativeHistogramSingleChannel(img_R, equal_R, width, height);
	CreateCumulativeHistogramSingleChannel(img_G, equal_G, width, height);
	CreateCumulativeHistogramSingleChannel(img_B, equal_B, width, height);

	MergeChannels_RGBToColor(equal_R, equal_G, equal_B, image_output, width, height);

	delete[] img_R;
	delete[] img_G;
	delete[] img_B;

	delete[] equal_R;
	delete[] equal_G;
	delete[] equal_B;
}