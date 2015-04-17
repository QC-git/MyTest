#pragma once


// CDlgSetting 对话框

class CDlgSetting : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetting();

// 对话框数据
	enum { IDD = IDD_CHILD_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheck1();
	BOOL m_bIdcCheck1;
};
