
// ImageProcessingDoc.h: CImageProcessingDoc 클래스의 인터페이스
//


#pragma once
#include "ImageProc.h"
#include <vector>

using namespace std;

struct IMAGE
{
	unsigned char* image_color;
	unsigned char* image_gray;
	int width;
	int height;
	int pixel_byte;
	// gray_image 가 binar_image 인가?
	bool bGrayBinary;
	IMAGE(void)
	{
		image_color = nullptr;
		image_gray = nullptr;
		width = 0;
		height = 0;
		pixel_byte = 0;
		bGrayBinary = false;
	}

	IMAGE(unsigned char* _color,
		unsigned char* _gray, int _width, int _height, int _pixel_byte)
	{
		image_color = _color;
		image_gray = _gray;
		width = _width;
		height = _height;
		pixel_byte = _pixel_byte;
		bGrayBinary = false;
	}
};

class CImageProcessingDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CImageProcessingDoc();
	DECLARE_DYNCREATE(CImageProcessingDoc)

// 특성입니다.
public:
	vector<IMAGE> m_Images;
	int cur_index;
	unsigned char* histogram; // RGBA (4byte)
	unsigned char* histogram_equalization_image; // RGBA(4byte)
	ImageProc* obj_ImageProc;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CImageProcessingDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnFileOpen();
	afx_msg void OnPixelprocessingAdjustbright();
	afx_msg void OnPixelprocessingAdjustcontrast();
	afx_msg void OnPixelprocessingBinarization();
	afx_msg void OnAreaprocessingBinaryerosion();
	afx_msg void OnAreaprocessingBinarydilation();
	afx_msg void OnNextImage();
	afx_msg void OnMaskingUsermasking1();
	afx_msg void OnMaskingBinarymasking();
	afx_msg void OnUsermasking2Circle();
	afx_msg void OnUsermasking2Updown();
	afx_msg void OnUsermasking2Divide();
	afx_msg void OnUsermasking2Cud();
	afx_msg void OnAreaprocessingAdaptivebinarization();
	afx_msg void OnWindowmaskingAveraging();
	afx_msg void OnWindowmaskingSobel();
	afx_msg void OnWindowmaskingLaplacian();
	afx_msg void OnWindowmaskingGausian();
	afx_msg void OnMedianfilterRadixsort();
	afx_msg void OnMedianfilterMedianofmedians();
	afx_msg void OnHistogramCreatehistogram();
	afx_msg void OnHistogramHistogramequalization();
	afx_msg void OnPracticeCountredbloodcell();
	afx_msg void OnImageprocessinggpuAdaptivebinarization();
	afx_msg void OnImageprocessinggpuBinarydilation();
	afx_msg void OnImageprocessinggpuBinaryerosion();
};
