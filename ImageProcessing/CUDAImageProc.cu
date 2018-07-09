#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

// gpu에 할당할 메모리를 저장할 변수를 선언한다.
// 하나는 호스트에서 받아올 용도 다른 하나는 호스트로 넘겨줄 용도
unsigned char* g_tempBuffer[2] = { nullptr };

// gpu에 할당된 메모리를 0으로 세팅하는 함수를 정의한다.
// 영상처리 할 것이므로 width와 height를 인자로 받는다.
int ImageProc_InitializeMemory(int width, int height)
{
	cudaError cudaStatus;
	cudaStatus = cudaMemset(g_tempBuffer[0], 0,
		sizeof(unsigned char)*width*height);
	// 첫번째 메모리 세팅에 실패하면 -1을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaMemset(g_tempBuffer[1], 0,
		sizeof(unsigned char)*width*height);
	// 두번째 메모리 세팅에 실패하면 -2을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -2;

	return 1;
}

// gpu에 메모리를 할당하는 함수를 정의한다.
// 해당 기능은 외부에서 쓰이므로 extern "C" 선언을 해준다.
extern "C"
int ImageProc_AllocGPUMemory(int width, int height)
{
	cudaError cudaStatus;
	cudaStatus = cudaMalloc((void**)&g_tempBuffer[0], 
		sizeof(unsigned char)*width*height);
	// 첫번째 메모리 할당에 실패하면 -1을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaMalloc((void**)&g_tempBuffer[1],
		sizeof(unsigned char)*width*height);
	// 두번째 메모리 할당에 실패하면 -2을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -2;

	// 할당한 메모리를 0 으로 세팅한다.
	return ImageProc_InitializeMemory(width, height);
}

extern "C"
// gpu에 할당한 메모리를 반환하는 기능을 정의한다.
int ImageProc_DeAllocGPUMemory(void)
{
	cudaError cudaStatus;
	cudaStatus = cudaFree(g_tempBuffer[0]);
	// 첫번째 메모리 반환에 실패하면 -1을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -1;

	cudaStatus = cudaFree(g_tempBuffer[1]);
	// 두번째 메모리 반환에 실패하면 -2을 반환한다.
	if (cudaStatus != cudaSuccess)
		return -2;

	return 1;
}

__global__ void Kernel_AdaptiveBinarization(unsigned char* image_gray,
	unsigned char* output_image, int width, int height, int ksize)
{
	if (ksize == 1 || ksize % 2 == 0) return;
	int neighbor = ksize / 2;
	// blockIdx, blockDim, threadIdx 로 좌표를 찾는다.
	// blockDim 은 block 사이즈 이므로 아래와 같이 좌표를 구할 수 있다.
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
	// blockIdx, blockDim, threadIdx 로 좌표를 찾는다.
	// blockDim 은 block 사이즈 이므로 아래와 같이 좌표를 구할 수 있다.
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	// 현재 픽셀이 255 이면 별도의 작업을 하지 않는다.
	if (image_binary[width*j + i] == 255) return;

	// 주변 픽셀이 255 이면 현재 픽셀을 255로 바꾼다.
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
	// blockIdx, blockDim, threadIdx 로 좌표를 찾는다.
	// blockDim 은 block 사이즈 이므로 아래와 같이 좌표를 구할 수 있다.
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	int j = blockIdx.y*blockDim.y + threadIdx.y;

	// 현재 픽셀이 0 이면 별도의 작업을 하지 않는다.
	if (image_binary[width*j + i] == 0) return;

	// 주변 픽셀이 0 이면 현재 픽셀을 0로 바꾼다.
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
	// GPU에 메모리 할당하고 0으로 세팅한다.
	/*if (ImageProc_AllocGPUMemory(width, height) < 0)
		return -1;*/

	// GPU 메모리에 호스트 데이터 복사한다.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0],image_gray,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// 커널 함수를 실행한다.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y 를 정의한다.
	dim3 Db = dim3(8,8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_AdaptiveBinarization<<< Dg, Db >>> (g_tempBuffer[0],
		g_tempBuffer[1],width,height,ksize);

	// 커널 함수 실행이 제대로 되었는지 확인한다.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// 커널 함수 모두 종료를 확인한다.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}
		

	// 결과를 호스트 메모리로 복사한다.
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
	// GPU 메모리에 호스트 데이터 복사한다.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	cudaStatus = cudaMemcpy(g_tempBuffer[1], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// 커널 함수를 실행한다.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y 를 정의한다.
	dim3 Db = dim3(8, 8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_BinaryDilation << < Dg, Db >> > (g_tempBuffer[0],
		g_tempBuffer[1], width, height, ksize);

	// 커널 함수 실행이 제대로 되었는지 확인한다.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// 커널 함수 모두 종료를 확인한다.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}


	// 결과를 호스트 메모리로 복사한다.
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
	// GPU 메모리에 호스트 데이터 복사한다.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	cudaStatus = cudaMemcpy(g_tempBuffer[1], image_binary,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// 커널 함수를 실행한다.
	// GridDim.x, GridDim.y, BlockDim.x, BlockDim.y 를 정의한다.
	dim3 Db = dim3(8, 8);
	dim3 Dg = dim3((width + Db.x - 1) / Db.x, (height + Db.y - 1) / Db.y);
	Kernel_BinaryErosion << < Dg, Db >> > (g_tempBuffer[0],
		g_tempBuffer[1], width, height, ksize);

	// 커널 함수 실행이 제대로 되었는지 확인한다.
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess)
		return -3;


	// 커널 함수 모두 종료를 확인한다.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		printf("cudaStatus : %d\n", cudaStatus);
		return -4;
	}


	// 결과를 호스트 메모리로 복사한다.
	cudaStatus = cudaMemcpy(image_binary, g_tempBuffer[1],
		sizeof(unsigned char)*width*height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
		return -5;

	return 1;

}