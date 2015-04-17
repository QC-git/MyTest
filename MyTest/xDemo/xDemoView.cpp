
// xDemoView.cpp : CxDemoView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CxDemoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CxDemoView 构造/析构

CxDemoView::CxDemoView()
{
	// TODO: 在此处添加构造代码

}

CxDemoView::~CxDemoView()
{
}

BOOL CxDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CxDemoView 绘制

void CxDemoView::OnDraw(CDC* /*pDC*/)
{
	CxDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CxDemoView 打印


void CxDemoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CxDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CxDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CxDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CxDemoView 诊断

#ifdef _DEBUG
void CxDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CxDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CxDemoDoc* CxDemoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CxDemoDoc)));
	return (CxDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CxDemoView 消息处理程序


void CxDemoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

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

	// TODO: 在此添加专用代码和/或调用基类
}
