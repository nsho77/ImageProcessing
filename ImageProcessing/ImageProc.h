#pragma once

class ImageProc
{
public:
	ImageProc();
	~ImageProc();

public:
	static void MergeChannels(unsigned char* in_color,
		unsigned char* out_gray, const int width, const int height);
};
