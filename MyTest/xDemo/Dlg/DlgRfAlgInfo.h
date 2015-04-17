#pragma once

#include "RfServece.h"
#include "afxcmn.h"
#include "afxwin.h"

// CRfAlgorihmDlg 对话框

class CDlgRfAlgInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfAlgInfo)

public:
	CDlgRfAlgInfo(RfService* pRfService, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRfAlgInfo();

// 对话框数据
	enum { IDD = IDD_CHILD4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	//virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButton1();

public:
	static int __stdcall AfterGetVssInfo(UINT uiRegionId, RF_REGION_STYLE emStyle, float* pVss, int nVssLen, float fMaxValue, float fMinValue, DWORD dwHandle)
	{
		return ((CDlgRfAlgInfo*)dwHandle)->AfterGetVssInfo2(uiRegionId, emStyle, pVss, nVssLen, fMaxValue, fMinValue);
	}
	int AfterGetVssInfo2(UINT uiRegionId, RF_REGION_STYLE emStyle, float* pVss, int nVssLen, float fMaxValue, float fMinValue);

public:
	RfService* m_pRfService;


	CListCtrl m_cListControl;
	UINT m_edtRegionId;
	CComboBox m_cComboBox;
};
