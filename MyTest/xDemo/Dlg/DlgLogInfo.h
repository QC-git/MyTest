#pragma once
#include "afxcmn.h"


// CDlgLogInfo 对话框

class CDlgLogInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogInfo)

public:
	CDlgLogInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogInfo();

// 对话框数据
	enum { IDD = IDD_CHILD6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_cListCtrl;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
};
