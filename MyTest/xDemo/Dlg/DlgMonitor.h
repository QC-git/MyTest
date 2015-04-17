#pragma once

#include "DlgDataBaseInfo.h"
#include "DlgRfSignalInfo.h"
#include "DlgRfImportInfo.h"
#include "DlgRfAlgInfo.h"
#include "DlgRfExportInfo.h"
#include "DlgLogInfo.h"

#include "MainService.h"
#include "afxwin.h"

#include "PictureEx.h"

// CDlgMonitor �Ի���

class CDlgMonitor : public CDialog
{
	DECLARE_DYNAMIC(CDlgMonitor)

public:
	CDlgMonitor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMonitor();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();

	CDlgDataBaseInfo*   m_pDlg1;
	CDlgRfSignalInfo*   m_pDlg2;
	CDlgRfImportInfo*   m_pDlg3;
	CDlgRfAlgInfo*      m_pDlg4;
	CDlgRfExportInfo*   m_pDlg5;
	CDlgLogInfo*        m_pDlg6;

	CMainService* m_pService;

	void ShowSubWindow(int nIndex);

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	//CStatic m_cSta1;
	CPictureEx m_cSta1;
};
