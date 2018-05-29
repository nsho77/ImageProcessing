
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

// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CImageProcessingDoc::OnFileOpen)
	ON_COMMAND(ID_PIXELPROCESSING_ADJUSTBRIGHT, &CImageProcessingDoc::OnPixelprocessingAdjustbright)
	ON_COMMAND(ID_PIXELPROCESSING_ADJUSTCONTRAST, &CImageProcessingDoc::OnPixelprocessingAdjustcontrast)
	ON_COMMAND(ID_PIXELPROCESSING_BINARIZATION, &CImageProcessingDoc::OnPixelprocessingBinarization)
END_MESSAGE_MAP()


// CImageProcessingDoc 생성/소멸

CImageProcessingDoc::CImageProcessingDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CImageProcessingDoc::~CImageProcessingDoc()
{
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

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
