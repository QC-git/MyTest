#pragma once


// CRfDataBaseInfoDlg 对话框
#include "MainService.h"
#include "afxcmn.h"

class CDlgDataBaseInfo : public CDialog, 
	public DataBaseReader::ReadDataInfoCallBack, 
	public RfDevVarsInFile_RW::ReadDataInfoCallBack,
	public DataBaseReader::ReadDataInfoCallBack2
{
	DECLARE_DYNAMIC(CDlgDataBaseInfo)

public:
	CDlgDataBaseInfo(CMainService* pService, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDataBaseInfo();

// 对话框数据
	enum { IDD = IDD_CHILD1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	CListCtrl m_cReaderList;
	CListCtrl m_cRegionList;

	afx_msg void OnBnClickedButton1();

protected:
	//DataBaseReader* m_pDataBaseReader;
	CMainService* m_pService;

	//@Override
	virtual void OnReadReaderData(int nCount, const VEC_ITER_PREADER_T& vecReaderInfos);

	//@Override
	virtual void OnReadRegionData(int nCount, const VEC_ITER_PREGION_T& vecRegionInfos);

	//@Override
	virtual void OnReadReaderData(const RfDevVarsInFile_RW::SData* pDataList);

	//@Override
	virtual void OnReadData(
		DataBaseReader::READER_MAP_T& cMap1, 
		DataBaseReader::REGION_POINT_MAP_T& cMap2, 
		DataBaseReader::REGION_LINE_MAP_T& cMap3, 
		DataBaseReader::REGION_RECT_MAP_T& cMap4);

public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
