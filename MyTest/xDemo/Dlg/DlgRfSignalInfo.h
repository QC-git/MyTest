#pragma once
#include "afxcmn.h"

#include "RfServece.h"
#include "ListAdapter.h"
#include "afxwin.h"

// CRfInfoDlg 对话框


class CDlgRfSignalInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfSignalInfo)

public:
	CDlgRfSignalInfo(RfService* pRfService, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRfSignalInfo();

// 对话框数据
	enum { IDD = IDD_CHILD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CListCtrl m_cList;

public:
	afx_msg void OnBnClickedButton1();  //读取信息
	//afx_msg void OnBnClickedButton2();  //信息切换

	afx_msg void OnBnClickedCheck1();   //自动读取
	BOOL m_bIsCheck;

	afx_msg void OnBnClickedCheck2();   //自动刷新
	BOOL m_bIsAutoUpdataInfo;

	RfService* m_pRfService;

	CRfListAdapter* m_pListAdapter;

public:
	static int __stdcall AfterGetRssiInfo(RF_INFO_RSSI_OUT* pInfoList, int nInfoNum, DWORD dwHandle)
	{
		return ((CDlgRfSignalInfo*)dwHandle)->AfterGetRssiInfo1(pInfoList, nInfoNum);
	}

	int AfterGetRssiInfo1(RF_INFO_RSSI_OUT* pInfoList, int nInfoNum);

	CString m_cComboValue;
	CComboBox m_cComboControl;
	afx_msg void OnCbnSelchangeCombo1();
};
