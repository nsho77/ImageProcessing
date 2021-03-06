
// ImageProcessingDoc.cpp: CImageProcessingDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif

#include "ImageProcessingDoc.h"
#include "ImageProcessingView.h"
#include "MainFrm.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LARGE_INTEGER Frequency;
LARGE_INTEGER BeginTime;
LARGE_INTEGER Endtime;

// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CImageProcessingDoc::OnFileOpen)
	ON_COMMAND(ID_PIXELPROCESSING_ADJUSTBRIGHT, &CImageProcessingDoc::OnPixelprocessingAdjustbright)
	ON_COMMAND(ID_PIXELPROCESSING_ADJUSTCONTRAST, &CImageProcessingDoc::OnPixelprocessingAdjustcontrast)
	ON_COMMAND(ID_PIXELPROCESSING_BINARIZATION, &CImageProcessingDoc::OnPixelprocessingBinarization)
	ON_COMMAND(ID_AREAPROCESSING_BINARYEROSION, &CImageProcessingDoc::OnAreaprocessingBinaryerosion)
	ON_COMMAND(ID_AREAPROCESSING_BINARYDILATION, &CImageProcessingDoc::OnAreaprocessingBinarydilation)
	ON_COMMAND(ID_32778, &CImageProcessingDoc::OnNextImage)
	ON_COMMAND(ID_MASKING_USERMASKING1, &CImageProcessingDoc::OnMaskingUsermasking1)
	ON_COMMAND(ID_MASKING_BINARYMASKING, &CImageProcessingDoc::OnMaskingBinarymasking)
	ON_COMMAND(ID_USERMASKING2_CIRCLE, &CImageProcessingDoc::OnUsermasking2Circle)
	ON_COMMAND(ID_USERMASKING2_UPDOWN, &CImageProcessingDoc::OnUsermasking2Updown)
	ON_COMMAND(ID_USERMASKING2_DIVIDE, &CImageProcessingDoc::OnUsermasking2Divide)
	ON_COMMAND(ID_USERMASKING2_CUD, &CImageProcessingDoc::OnUsermasking2Cud)
	ON_COMMAND(ID_AREAPROCESSING_ADAPTIVEBINARIZATION, &CImageProcessingDoc::OnAreaprocessingAdaptivebinarization)
	ON_COMMAND(ID_WINDOWMASKING_AVERAGING, &CImageProcessingDoc::OnWindowmaskingAveraging)
	ON_COMMAND(ID_WINDOWMASKING_SOBEL, &CImageProcessingDoc::OnWindowmaskingSobel)
	ON_COMMAND(ID_WINDOWMASKING_LAPLACIAN, &CImageProcessingDoc::OnWindowmaskingLaplacian)
	ON_COMMAND(ID_WINDOWMASKING_GAUSIAN, &CImageProcessingDoc::OnWindowmaskingGausian)
	ON_COMMAND(ID_MEDIANFILTER_RADIXSORT, &CImageProcessingDoc::OnMedianfilterRadixsort)
	ON_COMMAND(ID_MEDIANFILTER_MEDIANOFMEDIANS, &CImageProcessingDoc::OnMedianfilterMedianofmedians)
	ON_COMMAND(ID_HISTOGRAM_CREATEHISTOGRAM, &CImageProcessingDoc::OnHistogramCreatehistogram)
	ON_COMMAND(ID_HISTOGRAM_HISTOGRAMEQUALIZATION, &CImageProcessingDoc::OnHistogramHistogramequalization)
	ON_COMMAND(ID_PRACTICE_COUNTREDBLOODCELL, &CImageProcessingDoc::OnPracticeCountredbloodcell)
	ON_COMMAND(ID_IMAGEPROCESSINGGPU_ADAPTIVEBINARIZATION, &CImageProcessingDoc::OnImageprocessinggpuAdaptivebinarization)
	ON_COMMAND(ID_IMAGEPROCESSINGGPU_BINARYDILATION, &CImageProcessingDoc::OnImageprocessinggpuBinarydilation)
	ON_COMMAND(ID_IMAGEPROCESSINGGPU_BINARYEROSION, &CImageProcessingDoc::OnImageprocessinggpuBinaryerosion)
END_MESSAGE_MAP()


// CImageProcessingDoc 생성/소멸

CImageProcessingDoc::CImageProcessingDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	histogram = nullptr;
	histogram_equalization_image = nullptr;
	obj_ImageProc = nullptr;
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	if (histogram)
		delete[] histogram;
	if (histogram_equalization_image)
		delete[] histogram_equalization_image;
	if (obj_ImageProc)
		delete obj_ImageProc;
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (obj_ImageProc)
		delete obj_ImageProc;
	obj_ImageProc = new ImageProc;
	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CImageProcessingDoc serialization

void CImageProcessingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImageProcessingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CImageProcessingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageProcessingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageProcessingDoc 진단

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessingDoc 명령


void CImageProcessingDoc::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TCHAR szFilter[] = _T("Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG; | All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal()) {

		CString strPathName = dlg.GetPathName();

		CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

		unsigned char* output = nullptr;
		int width, height, byte = 0;

		if (pView->ReadImageFile(strPathName, output, width, height, byte))
		{
			printf("open success\n");

			unsigned char* image_gray = new unsigned char[width*height];
			ImageProc::MergeChannels(output, image_gray, width, height);

			printf("mergeChannels success \n");

			printf("image_width : %d\n", width);
			printf("image_height : %d\n", height);

			// IMAGE 변수를 만들어 데이터를 저장하고 이를 배열에 저장한다.
			IMAGE image_info = IMAGE(output, image_gray, width, height, byte);

			printf("image_width : %d\n",image_info.width);
			printf("image_height : %d\n",image_info.height);

			m_Images.push_back(image_info);
			cur_index = static_cast<int>(m_Images.size() - 1);

			printf("array insert success \n");

			// view의 setDrawImage를 호출하고 화면을 update한다.
			pView->SetDrawImage(output, image_gray,
				m_Images[cur_index].width, m_Images[cur_index].height);

			pView->OnInitialUpdate();

			printf("m_Images size %zd \n", m_Images.size());
		}
		else
			printf("open fail\n");

		
	}
}


void CImageProcessingDoc::OnPixelprocessingAdjustbright()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	ImageProc::AdjustBright(m_Images[cur_index].image_color, m_Images[cur_index].width, m_Images[cur_index].height,34);
	ImageProc::MergeChannels(m_Images[cur_index].image_color, m_Images[cur_index].image_gray ,m_Images[cur_index].width, m_Images[cur_index].height);
	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnPixelprocessingAdjustcontrast()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::AdjustContrast(m_Images[cur_index].image_color, m_Images[cur_index].width, m_Images[cur_index].height, 3);
	ImageProc::MergeChannels(m_Images[cur_index].image_color, m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnPixelprocessingBinarization()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::Binarization(m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height, 127);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnAreaprocessingBinaryerosion()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 먼저 binarization 한다.
	ImageProc::Binarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 127);

	// 주변 1개의 픽셀을 바라본다.
	ImageProc::BinaryErosion(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 3);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnAreaprocessingBinarydilation()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 먼저 binarization 한다.
	ImageProc::Binarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 127);

	// 주변 1개의 픽셀을 바라본다.
	ImageProc::BinaryDilation(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 3);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnNextImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (m_Images.size() == 0)
	{
		printf("Load Image.. No Images.\n");
		return;
	}

	cur_index = (cur_index + 1) % static_cast<int>(m_Images.size());

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnMaskingUsermasking1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::UserMasking1(m_Images[cur_index].image_color, 
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray,m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}

void CImageProcessingDoc::OnMaskingBinarymasking()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::Binarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 127);

	ImageProc::BinaryMasking(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnUsermasking2Circle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::UserMaskingCircle(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnUsermasking2Updown()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::UserMaskingUpDown(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnUsermasking2Divide()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::UserMaskingDivide(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnUsermasking2Cud()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::UserMaskingCUD(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnAreaprocessingAdaptivebinarization()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	printf("CPU Adaptivebinarization\n");
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	ImageProc::AdaptiveBinarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 125);

	QueryPerformanceCounter(&Endtime);
	int elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	double duringtime = (double)elapsed / (double)Frequency.QuadPart;

	printf("CPU Adaptivebinarization time : %f\n", duringtime);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnWindowmaskingAveraging()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::AveragingImageUsingMask(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnWindowmaskingSobel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::SobelMasking(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnWindowmaskingLaplacian()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::LaplacianMasking(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnWindowmaskingGausian()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::GausianMasking(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnMedianfilterRadixsort()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	printf("MedianFilter RadixSort\n");

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	ImageProc::MedianFilter(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height, 5, "RadixSort");

	QueryPerformanceCounter(&Endtime);
	int elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	double duringtime = (double)elapsed / (double)Frequency.QuadPart;

	printf("MedianFilter time : %f\n", duringtime);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
	printf("MedianFilter RadixSort End \n");
}


void CImageProcessingDoc::OnMedianfilterMedianofmedians()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	printf("MedianFilter MedianOfMedians\n");

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	ImageProc::MedianFilter(m_Images[cur_index].image_color,
		m_Images[cur_index].width, m_Images[cur_index].height, 5, "MedianOfMedians");

	QueryPerformanceCounter(&Endtime);
	int elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	double duringtime = (double)elapsed / (double)Frequency.QuadPart;

	printf("MedianFilter time : %f\n", duringtime);
	ImageProc::MergeChannels(m_Images[cur_index].image_color,
		m_Images[cur_index].image_gray, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	pView->OnInitialUpdate();
	printf("MedianFilter MedianOfMedians End \n");
}


void CImageProcessingDoc::OnHistogramCreatehistogram()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	const int histo_width = 512;
	const int histo_height = m_Images[cur_index].height;

	histogram = new unsigned char[histo_width * histo_height * 4];
	memset(histogram, 0, sizeof(unsigned char)*histo_width*histo_height * 4);

	ImageProc::CreateHistogram(m_Images[cur_index].image_color, histogram,
		m_Images[cur_index].width, m_Images[cur_index].height, histo_width, histo_height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImageWithHistogram(m_Images[cur_index].image_color, histogram,
		m_Images[cur_index].width, m_Images[cur_index].height,histo_width, histo_height);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnHistogramHistogramequalization()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	histogram_equalization_image =
		new unsigned char[m_Images[cur_index].width * m_Images[cur_index].height *4];
	memset(histogram_equalization_image, 0, sizeof(unsigned char)
		*m_Images[cur_index].width * m_Images[cur_index].height * 4);
	/*memcpy(histogram_equalization_image, m_Images[cur_index].image_color,
		sizeof(unsigned char)*m_Images[cur_index].width*m_Images[cur_index].height * 4);*/

	ImageProc::CreateHistogramEqualization(m_Images[cur_index].image_color,
		histogram_equalization_image, m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, histogram_equalization_image,
		m_Images[cur_index].width, m_Images[cur_index].height, 4);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnPracticeCountredbloodcell()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ImageProc::Binarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height,185);
	/*ImageProc::BinaryErosion(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 3);*/


	ImageProc::CountRedBloodCell(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height);

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 1);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnImageprocessinggpuAdaptivebinarization()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	printf("OnImageprocessing GPU Adaptivebinarization\n");

	obj_ImageProc->AllocateGPUMemory(m_Images[cur_index].width, m_Images[cur_index].height);

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	obj_ImageProc->GPU_AdaptiveBinarization(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 75);

	QueryPerformanceCounter(&Endtime);
	int elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	double duringtime = (double)elapsed / (double)Frequency.QuadPart;
	printf("GPU AdaptiveBinarization time : %f\n", duringtime);

	// GPU Memory DeAllocation
	if (obj_ImageProc->DeAllocateGPUMemory())
	{
		printf("DeAllocation success!!\n");
	}


	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 1);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnImageprocessinggpuBinarydilation()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	obj_ImageProc->AllocateGPUMemory(m_Images[cur_index].width, m_Images[cur_index].height);

	obj_ImageProc->GPU_BinaryDilation(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 5);

	if (obj_ImageProc->DeAllocateGPUMemory())
		printf("DeAllocation success!!\n");

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 1);

	pView->OnInitialUpdate();
}


void CImageProcessingDoc::OnImageprocessinggpuBinaryerosion()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	obj_ImageProc->AllocateGPUMemory(m_Images[cur_index].width, m_Images[cur_index].height);

	obj_ImageProc->GPU_BinaryErosion(m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 5);

	if (obj_ImageProc->DeAllocateGPUMemory())
		printf("DeAllocation success!!\n");

	CImageProcessingView* pView = (CImageProcessingView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	pView->SetDrawImage(m_Images[cur_index].image_color, m_Images[cur_index].image_gray,
		m_Images[cur_index].width, m_Images[cur_index].height, 1);

	pView->OnInitialUpdate();
}
