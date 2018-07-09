#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

// gpu�� �Ҵ��� �޸𸮸� ������ ������ �����Ѵ�.
// �ϳ��� ȣ��Ʈ���� �޾ƿ� �뵵 �ٸ� �ϳ��� ȣ��Ʈ�� �Ѱ��� �뵵
unsigned char* g_tempBuffer[2] = { nullptr };

// gpu�� �Ҵ�� �޸𸮸� 0���� �����ϴ� �Լ��� �����Ѵ�.
// ����ó�� �� ���̹Ƿ� width�� height�� ���ڷ� �޴´�.
int ImageProc_InitializeMemory(int width, int height)
{
	cudaError cudaStatus;
	cudaStatus = cudaMemset(g_tempBuffer[0], 0,
		sizeof(unsigned char)*width*height);
	// ù��° �޸� ���ÿ� �����ϸ� -1�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaMemset(g_tempBuffer[1], 0,
		sizeof(unsigned char)*width*height);
	// �ι�° �޸� ���ÿ� �����ϸ� -2�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -2;

	return 1;
}

// gpu�� �޸𸮸� �Ҵ��ϴ� �Լ��� �����Ѵ�.
// �ش� ����� �ܺο��� ���̹Ƿ� extern "C" ������ ���ش�.
extern "C"
int ImageProc_AllocGPUMemory(int width, int height)
{
	cudaError cudaStatus;
	cudaStatus = cudaMalloc((void**)&g_tempBuffer[0], 
		sizeof(unsigned char)*width*height);
	// ù��° �޸� �Ҵ翡 �����ϸ� -1�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaMalloc((void**)&g_tempBuffer[1],
		sizeof(unsigned char)*width*height);
	// �ι�° �޸� �Ҵ翡 �����ϸ� -2�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -2;

	// �Ҵ��� �޸𸮸� 0 ���� �����Ѵ�.
	return ImageProc_InitializeMemory(width, height);
}

extern "C"
// gpu�� �Ҵ��� �޸𸮸� ��ȯ�ϴ� ����� �����Ѵ�.
int ImageProc_DeAllocGPUMemory(void)
{
	cudaError cudaStatus;
	cudaStatus = cudaFree(g_tempBuffer[0]);
	// ù��° �޸� ��ȯ�� �����ϸ� -1�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaFree(g_tempBuffer[1]);
	// �ι�° �޸� ��ȯ�� �����ϸ� -2�� ��ȯ�Ѵ�.
	if (cudaStatus != cudaSuccess)
		return -2;

	return 1;
}

__global__ void Kernel_AdaptiveBinarization(unsigned char* image_gray,
	unsigned char* output_image, int width, int height, int ksize)
{
	if (ksize == 1 || ksize % 2 == 0) return;
	int neighbor = ksize / 2;
	// blockIdx, blockDim, threadIdx �� ��ǥ�� ã�´�.
	// blockDim �� block ������ �̹Ƿ� �Ʒ��� ���� ��ǥ�� ���� �� �ִ�.
	int i = blockIdx.x*blockDim.x+threadIdx.x;
	int j = blockIdx.y*blockDim.y+threadIdx.y;

	float avg = 0.f;
	float cnt = 0.f;

	for (int x = -neighbor; x <= neighbor; x++)
	{
		for (int y = -neighbor; y <= neighbor; y++)
		{
			if (i + x < 0 || i + x >= width || j + y < 0 || j + y >= height)
				continue;
			avg += image_gray[width*(j + y) + (i + x)];
			cnt += 1.f;
		}
	}
	avg = avg / cnt;
	if (image_gray[width*j + i] > avg)
		output_image[width*j + i] = 255;
	else
		output_image[width*j + i] = 0;
}


__global__ void Kernel_BinaryDilation(unsigned char* image_binary,
	unsigned char* output_image, int width, int height, int ksize)
{
	if (ksize == 1 || ksize % 2 == 0) return;
	int neighbor = ksize / 2;
	// blockIdx, blockDim, threadIdx �� ��ǥ�� ã�´�.
	// blockDim �� block ������ �̹Ƿ� �Ʒ��� ���� ��ǥ�� ���� �� �ִ�.
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	// ���� �ȼ��� 255 �̸� ������ �۾��� ���� �ʴ´�.
	if (image_binary[width*j + i] == 255) return;

	// �ֺ� �ȼ��� 255 �̸� ���� �ȼ��� 255�� �ٲ۴�.
	for (int x = -neighbor; x <= neighbor; x++)
	{
		for (int y = -neighbor; y <= neighbor; y++)
		{
			if (i + x < 0 || i + x >= width || j + y < 0 || j + y >= height)
				continue;
			if (image_binary[width*(y + j) + i + x] == 255)
			{
				output_image[width*j + i] = 255;
				return;
			}
			
		}
	}

}

__global__ void Kernel_BinaryErosion(unsigned char* image_binary,
	unsigned char* output_image, int width, int height, int ksize)
{
	if (ksize == 1 || ksize % 2 == 0) return;
	int neighbor = ksize / 2;
	// blockIdx, blockDim, threadIdx �� ��ǥ�� ã�´�.
	// blockDim �� block ������ �̹Ƿ� �Ʒ��� ���� ��ǥ�� ���� �� �ִ�.
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	// ���� �ȼ��� 0 �̸� ������ �۾��� ���� �ʴ´�.
	if (image_binary[width*j + i] == 0) return;

	// �ֺ� �ȼ��� 0 �̸� ���� �ȼ��� 0�� �ٲ۴�.
	for (int x = -neighbor; x <= neighbor; x++)
	{
		for (int y = -neighbor; y <= neighbor; y++)
		{
			if (i + x < 0 || i + x >= width || j + y < 0 || j + y >= height)
				continue;
			if (image_binary[width*(y + j) + i + x] == 0)
			{
				output_image[width*j + i] = 0;
				return;
			}

		}
	}

}

extern "C"
int ImageProc_AdaptiveBinarization(unsigned char* image_gray,
	int width, int height, int ksize)
{
	// GPU�� �޸� �Ҵ��ϰ� 0���� �����Ѵ�.
	/*if (ImageProc_AllocGPUMemory(width, height) < 0)
		return -1;*/

	// GPU �޸𸮿� ȣ��Ʈ ������ �����Ѵ�.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0],image_gray,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// Ŀ�� �Լ��� �����Ѵ�.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y �� �����Ѵ�.
	dim3 Db = dim3(8,8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_AdaptiveBinarization<<< Dg, Db >>> (g_tempBuffer[0],
		g_tempBuffer[1],width,height,ksize);

	// Ŀ�� �Լ� ������ ����� �Ǿ����� Ȯ���Ѵ�.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// Ŀ�� �Լ� ��� ���Ḧ Ȯ���Ѵ�.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}
		

	// ����� ȣ��Ʈ �޸𸮷� �����Ѵ�.
	cudaStatus = cudaMemcpy(image_gray, g_tempBuffer[1],
		sizeof(unsigned char)*width*height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
		return -5;

	return 1;
	
}

extern "C"
int ImageProc_BinaryDilation(unsigned char* image_binary,
	int width, int height, int ksize)
{
	// GPU �޸𸮿� ȣ��Ʈ ������ �����Ѵ�.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	cudaStatus = cudaMemcpy(g_tempBuffer[1], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// Ŀ�� �Լ��� �����Ѵ�.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y �� �����Ѵ�.
	dim3 Db = dim3(8, 8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_BinaryDilation << < Dg, Db >> > (g_tempBuffer[0],
		g_tempBuffer[1], width, height, ksize);

	// Ŀ�� �Լ� ������ ����� �Ǿ����� Ȯ���Ѵ�.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// Ŀ�� �Լ� ��� ���Ḧ Ȯ���Ѵ�.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}


	// ����� ȣ��Ʈ �޸𸮷� �����Ѵ�.
	cudaStatus = cudaMemcpy(image_binary, g_tempBuffer[1],
		sizeof(unsigned char)*width*height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
		return -5;

	return 1;

}


extern "C"
int ImageProc_BinaryErosion(unsigned char* image_binary,
	int width, int height, int ksize)
{
	// GPU �޸𸮿� ȣ��Ʈ ������ �����Ѵ�.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	cudaStatus = cudaMemcpy(g_tempBuffer[1], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// Ŀ�� �Լ��� �����Ѵ�.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y �� �����Ѵ�.
	dim3 Db = dim3(8, 8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_BinaryErosion << < Dg, Db >> > (g_tempBuffer[0],
		g_tempBuffer[1], width, height, ksize);

	// Ŀ�� �Լ� ������ ����� �Ǿ����� Ȯ���Ѵ�.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// Ŀ�� �Լ� ��� ���Ḧ Ȯ���Ѵ�.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}


	// ����� ȣ��Ʈ �޸𸮷� �����Ѵ�.
	cudaStatus = cudaMemcpy(image_binary, g_tempBuffer[1],
		sizeof(unsigned char)*width*height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
		return -5;

	return 1;

}