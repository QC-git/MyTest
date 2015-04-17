#pragma once

#include "RfServece.h"
#include "afxcmn.h"

// CRfParameterDlg �Ի���

class CDlgRfImportInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfImportInfo)

public:
	CDlgRfImportInfo(RfService* pRfService, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRfImportInfo();

// �Ի�������
	enum { IDD = IDD_CHILD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
