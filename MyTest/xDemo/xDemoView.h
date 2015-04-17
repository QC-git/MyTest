
// xDemoView.h : CxDemoView ��Ľӿ�
//

#pragma once

#include "DlgMain.h"

class CxDemoView : public CView
{
protected: // �������л�����
	CxDemoView();
	DECLARE_DYNCREATE(CxDemoView)

// ����
public:
	CxDemoDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CxDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//CDlgMain m_cMainDlg;

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // xDemoView.cpp �еĵ��԰汾
inline CxDemoDoc* CxDemoView::GetDocument() const
   { return reinterpret_cast<CxDemoDoc*>(m_pDocument); }
#endif

