
// xDemoView.h : CxDemoView 类的接口
//

#pragma once

#include "DlgMain.h"

class CxDemoView : public CView
{
protected: // 仅从序列化创建
	CxDemoView();
	DECLARE_DYNCREATE(CxDemoView)

// 特性
public:
	CxDemoDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CxDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//CDlgMain m_cMainDlg;

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // xDemoView.cpp 中的调试版本
inline CxDemoDoc* CxDemoView::GetDocument() const
   { return reinterpret_cast<CxDemoDoc*>(m_pDocument); }
#endif

