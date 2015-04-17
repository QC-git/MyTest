#pragma once

#include "ChartClass\ChartCtrl.h"
#include "ChartClass\ChartLineSerie.h"
#include "ChartClass\ChartAxis.h"

#include "MyCpuStatus.h"
#include "afxwin.h"

#include "DlgLogin.h"
#include "DlgMonitor.h"
#include "DlgSetting.h"

#include "MainService.h"

extern CMainService g_cService;

//#include "aboutDlg.h"

// CDlgMain 对话框

class CDlgMain : public CDialog
{
	DECLARE_DYNAMIC(CDlgMain)

public:
	CDlgMain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMain();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg LRESULT OnNcHitTest(CPoint point);


public:
	void SetUpRegon();

	void InitChart();

	void InitCpuStatus(bool bThread);

	long GetCpuStatue();

	void LeftMoveArray(double* ptr,size_t length,double data);

	CMFCTabCtrl      m_cTab;
	CChartCtrl       m_cHSChartCtrl;
	CChartLineSerie* m_pLineSerie;

	MyCpuStatus m_cCpu;

	MyCpuStatusEx m_CpuEx;

	CBrush m_brBk;

	CDlgLogin   m_cLoginDlg;
	CDlgMonitor m_cMonitorDlg;
	CDlgSetting m_cSettingDlg;

	double m_HightSpeedChartArray[2096];
	double m_X[2096];
	unsigned int m_count;
	size_t m_c_arrayLength;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	
	CBitmapButton m_cBtn1;
	CBitmapButton m_cBtn2;
	CBitmapButton m_cBtn3;
	CBitmapButton m_cBtn4;
	CBitmapButton m_cBtn5;
	CStatic m_cStaText1;
	CStatic m_cStaText2;

	//托盘
	void InitTray();
	void DestroyTray();
	void HideMainWindow();
	void ShowMainWindow();

	NOTIFYICONDATA m_Nid; // 托盘

	afx_msg void OnDestroy();

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void InitGifPicture();

	CPictureEx m_cGifPicture;

	CPictureEx m_cStaPicture;
};
