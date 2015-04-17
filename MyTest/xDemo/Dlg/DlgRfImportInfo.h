#pragma once

#include "RfServece.h"
#include "afxcmn.h"

// CRfParameterDlg 对话框

class CDlgRfImportInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfImportInfo)

public:
	CDlgRfImportInfo(RfService* pRfService, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRfImportInfo();

// 对话框数据
	enum { IDD = IDD_CHILD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CListCtrl m_ctlReaderList;
	CListCtrl m_ctlRegionList;

public:
	RfService* m_pRfService;

	afx_msg void OnBnClickedButton1();

public:
	static int __stdcall AfterGetReaderInfo(RF_INFO_READER* readerList, int readerListNum, DWORD dwHandle)
	{
		if (dwHandle) return ((CDlgRfImportInfo*)dwHandle)->AfterGetReaderInfo1(readerList, readerListNum);
		return -1;
	}
	
	int AfterGetReaderInfo1(RF_INFO_READER* readerList, int readerListNum);

	static int __stdcall AfterGetRegionInfo(RF_INFO_REGION* regionList, int regionNum, DWORD dwHandle)
	{
		if (dwHandle) return ((CDlgRfImportInfo*)dwHandle)->AfterGetRegionInfo1(regionList, regionNum);
		return -1;
	}

	int AfterGetRegionInfo1(RF_INFO_REGION* regionList, int regionNum);

};
