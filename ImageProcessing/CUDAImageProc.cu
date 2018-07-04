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

extern "C"
int ImageProc_AdaptiveBinarization(unsigned char* image_gray,
	int width, int height, int ksize)
{
	// GPU에 메모리 할당하고 0으로 세팅한다.
	if (ImageProc_AllocGPUMemory(width, height) < 0)
		return -1;

	// GPU 메모리에 호스트 데이터 복사한다.
	cudaError cudaStatus;
	cudaStatus = cudaMemcpy(g_tempBuffer[0],image_gray,
		sizeof(unsigned char)*width*height, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
		return -2;

	// 커널 함수를 실행한다.


	// 커널 함수 모두 종료를 확인한다.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
		return -4;

	// 결과를 호스트 메모리로 복사한다.
	cudaStatus = cudaMemcpy(image_gray, g_tempBuffer[1],
		sizeof(unsigned char)*width*height, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
		return -5;

	return 1;
	
}