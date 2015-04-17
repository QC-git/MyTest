
// xDemoView.cpp : CxDemoView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "xDemo.h"
#endif

#include "xDemoDoc.h"
#include "xDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MainFrm.h"

// CxDemoView

IMPLEMENT_DYNCREATE(CxDemoView, CView)

BEGIN_MESSAGE_MAP(CxDemoView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CxDemoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CxDemoView ����/����

CxDemoView::CxDemoView()
{
	// TODO: �ڴ˴���ӹ������

}

CxDemoView::~CxDemoView()
{
}

BOOL CxDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CxDemoView ����

void CxDemoView::OnDraw(CDC* /*pDC*/)
{
	CxDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CxDemoView ��ӡ


void CxDemoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CxDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CxDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CxDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CxDemoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CxDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CxDemoView ���

#ifdef _DEBUG
void CxDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CxDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CxDemoDoc* CxDemoView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CxDemoDoc)));
	return (CxDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CxDemoView ��Ϣ�������


void CxDemoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	HWND hWnd = this->GetSafeHwnd();
	hWnd = pMain->GetSafeHwnd();
	if (this->GetSafeHwnd() != NULL && pMain->GetSafeHwnd() != NULL)
	{
		CRect rt;
		GetWindowRect(&rt);
		pMain->ScreenToClient(&rt);

		if (pMain->m_cMainDlg.GetSafeHwnd()!=NULL)
		{
			pMain->m_cMainDlg.MoveWindow(&rt);
			{
				pMain->m_cMainDlg.ShowWindow(SW_SHOW);
				ShowWindow(SW_HIDE);
			}

		}
	}
}


void CxDemoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
}
