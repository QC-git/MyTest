#pragma once
#include "afxcmn.h"


// CDlgLogInfo �Ի���

class CDlgLogInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogInfo)

public:
	CDlgLogInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogInfo();

// �Ի�������
	enum { IDD = IDD_CHILD6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_cListCtrl;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
};
