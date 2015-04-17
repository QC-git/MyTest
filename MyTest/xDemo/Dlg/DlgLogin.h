#pragma once


// CLoginDlg 对话框
#include "MainService.h"
#include "afxwin.h"

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogin();

// 对话框数据
	enum { IDD = IDD_CHILD_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonLogout();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CProgressCtrl m_cProgressCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	CString m_edtUserName;
	CString m_edtUserPassword;

protected:

	//@Override
	void OnLogionStatus(bool bIsLogionSuc, void* dummy);

	bool m_bIsSetBtnPress;

	void SetBtnModeUpData();

	void DoLogin();

	void DoLogout();

	bool m_bIsHaveLogin;

public:
	afx_msg void OnPaint();
	CButton m_cBtn1;
};
