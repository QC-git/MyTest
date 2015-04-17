#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "RF_Position_API_v2.h"
#include "RfServece.h"
#include "ListAdapter.h"

// CZuoBiaoInfoDlg 对话框

class CDlgRfExportInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfExportInfo)

public:
	CDlgRfExportInfo(RfService* pRfService, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRfExportInfo();

// 对话框数据
	enum { IDD = IDD_CHILD5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck1();
	BOOL m_bAutoShow;
	CListCtrl m_cListContrl;

	static int __stdcall AfterGetTagInfo(RF_INFO_TAG* tagList, int tagListNum, DWORD dwHandle)
	{
		return ((CDlgRfExportInfo*)dwHandle)->AfterGetTagInfo2(tagList, tagListNum);
	}
	int AfterGetTagInfo2(RF_INFO_TAG * tagList, int tagListNum);

	void UpdataTagList();

public:
	static int __stdcall AfterGetReaderInfo(RF_INFO_READER* readerList, int readerListNum, DWORD dwHandle)
	{
		if (dwHandle) return ((CDlgRfExportInfo*)dwHandle)->AfterGetReaderInfo1(readerList, readerListNum);
		return -1;
	}

	int AfterGetReaderInfo1(RF_INFO_READER* readerList, int readerListNum);

private:
	RfService* m_pRfService;

// 	typedef ValueMap<long, RF_INFO_TAG*> TagInfoMap_T;
// 	typedef ValueSet<long> TagIdSet_T;

	ExportTagInfoMap_T m_cTagInfoMap;
	ExportTagIdSet_T m_cTagIdSet;  // 用于排序
	ExportReaderInfoMap_T m_cReaderMap;
	
	X::CLock m_lock;

	long m_LastListUpdataTime;

	//HMUTEX_D m_Lock;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
