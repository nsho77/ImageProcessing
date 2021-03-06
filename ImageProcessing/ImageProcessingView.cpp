
// ImageProcessingView.cpp: CImageProcessingView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif

#include "ImageProcessingDoc.h"
#include "ImageProcessingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageProcessingView

IMPLEMENT_DYNCREATE(CImageProcessingView, CView)

BEGIN_MESSAGE_MAP(CImageProcessingView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImageProcessingView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CImageProcessingView 생성/소멸

CImageProcessingView::CImageProcessingView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_ImgWidth = 0;
	m_ImgHeight = 0;

	m_Image = nullptr;

}

CImageProcessingView::~CImageProcessingView()
{
	if (m_Image)
		delete[] m_Image;
}

bool CImageProcessingView::ReadImageFile(CString filename,
	unsigned char*& output, int& width, int& height, int& byte)
{
	CBitmap Bitmap;
	
	Bitmap.m_hObject =
		(HBITMAP)::LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!Bitmap.m_hObject) {
		printf("There is no Bitmp.m_hObject\n");
		return false;
	}
	BITMAP bm;
	Bitmap.GetBitmap(&bm);

	DWORD dwCount = bm.bmWidthBytes*bm.bmHeight;

	printf("[DOC vector] Image Memory Allocation \n");
	output = new unsigned char[dwCount];
	DWORD dwRead = Bitmap.GetBitmapBits(dwCount, output);

	width = bm.bmWidth;
	height = bm.bmHeight;
	byte = bm.bmBitsPixel / 8;

	return true;
}

void CImageProcessingView::SetDrawImage(unsigned char* image,
	const int width, const int height, const int byte)
{
	if (byte<=0) return;

	m_ImgWidth = width;
	m_ImgHeight = height;

	if (m_Image)
		delete[] m_Image;

	m_Image = new unsigned char[width*height * 4];

	if (byte > 1)
	{
		for (int i = 0; i < width*height; i++)
		{
			m_Image[i * 4 + 0] = image[i * byte + 0];
			m_Image[i * 4 + 1] = image[i * byte + 1];
			m_Image[i * 4 + 2] = image[i * byte + 2];
		}
	}
	else
	{
		for (int i = 0; i < width*height; i++)
		{
			m_Image[i * 4 + 0] = image[i];
			m_Image[i * 4 + 1] = image[i];
			m_Image[i * 4 + 2] = image[i];
		}
	}
}

void CImageProcessingView::SetDrawImage(unsigned char* image, unsigned char* image_gray,
	const int width, const int height, const int byte)
{
	m_ImgWidth = width * 2;
	m_ImgHeight = height;

	if (m_Image)
		delete[] m_Image;

	// 화면을 두 배로 써야 하기 때문에 *2를 해준다.
	m_Image = new unsigned char[width * 2 * height * 4];

	for (int i = 0; i<width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			m_Image[(m_ImgWidth*j + i) * 4 + 0] = image[(width*j + i) * 4 + 0];
			m_Image[(m_ImgWidth*j + i) * 4 + 1] = image[(width*j + i) * 4 + 1];
			m_Image[(m_ImgWidth*j + i) * 4 + 2] = image[(width*j + i) * 4 + 2];
		}
	}

	// color image 다음에 gray image 가 나오므로 + width 를 한다.
	if (byte == 1)
	{
		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				m_Image[(m_ImgWidth*j + i + width) * 4 + 0] = image_gray[(width*j + i)];
				m_Image[(m_ImgWidth*j + i + width) * 4 + 1] = image_gray[(width*j + i)];
				m_Image[(m_ImgWidth*j + i + width) * 4 + 2] = image_gray[(width*j + i)];
			}
		}
	}
	else
	{
		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				m_Image[(m_ImgWidth*j + i + width) * 4 + 0] = image_gray[(width*j + i)*byte + 0];
				m_Image[(m_ImgWidth*j + i + width) * 4 + 1] = image_gray[(width*j + i)*byte + 1];
				m_Image[(m_ImgWidth*j + i + width) * 4 + 2] = image_gray[(width*j + i)*byte + 2];
			}
		}
	}
	
}

void CImageProcessingView::SetDrawImageWithHistogram(unsigned char* image, unsigned char* histogram,
	const int width, const int height, const int histo_width, const int histo_height)
{
	m_ImgWidth = width + histo_width;
	m_ImgHeight = height;

	if (m_Image)
		delete[] m_Image;

	// 화면을 histogram 너비 만큼 써야 하기 때문에 +histo_width 를 해준다.
	m_Image = new unsigned char[(width + histo_width) * height * 4];

	for (int i = 0; i<width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			m_Image[(m_ImgWidth*j + i) * 4 + 0] = image[(width*j + i) * 4 + 0];
			m_Image[(m_ImgWidth*j + i) * 4 + 1] = image[(width*j + i) * 4 + 1];
			m_Image[(m_ImgWidth*j + i) * 4 + 2] = image[(width*j + i) * 4 + 2];
		}
	}


	for (int i = 0; i<histo_width; i++)
	{
		for (int j = 0; j < histo_height; j++)
		{
			m_Image[(m_ImgWidth*j + i + width) * 4 + 0] = histogram[(histo_width*j + i)*4 + 0];
			m_Image[(m_ImgWidth*j + i + width) * 4 + 1] = histogram[(histo_width*j + i)*4 + 1];
			m_Image[(m_ImgWidth*j + i + width) * 4 + 2] = histogram[(histo_width*j + i)*4 + 2];
		}
	}
	
}

BOOL CImageProcessingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CImageProcessingView 그리기

void CImageProcessingView::OnDraw(CDC* pDC)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	if (m_Image)
	{
		printf("m_Image is exists\n");
		CDC MemDC;
		BITMAP bmpInfo;

		// 화면 DC와 호환되는 메모리 DC를 생성
		MemDC.CreateCompatibleDC(pDC);

		// 비트맵 리소스 로딩
		CBitmap cBitmap;
		cBitmap.CreateBitmap(m_ImgWidth, m_ImgHeight, 1, 32, m_Image);
		CBitmap* pOldBmp = NULL;

		// 로딩된 비트맵 정보 확인
		cBitmap.GetBitmap(&bmpInfo);

		printf("view image width %d, height %d\n", bmpInfo.bmWidth, bmpInfo.bmHeight);

		// 메모리 DC에 선택
		pOldBmp = MemDC.SelectObject(&cBitmap);

		// 메모리 DC에 들어 있는 비트맵을 화면 DC로 복사하여 출력
		pDC->BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &MemDC, 0, 0, SRCCOPY);

		MemDC.SelectObject(pOldBmp);
	}
}


// CImageProcessingView 인쇄


void CImageProcessingView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageProcessingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CImageProcessingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CImageProcessingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CImageProcessingView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageProcessingView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImageProcessingView 진단

#ifdef _DEBUG
void CImageProcessingView::AssertValid() const
{
	CView::AssertValid();
}

void CImageProcessingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageProcessingDoc* CImageProcessingView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessingDoc)));
	return (CImageProcessingDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageProcessingView 메시지 처리기
