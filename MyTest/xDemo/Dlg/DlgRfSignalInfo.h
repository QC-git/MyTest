#pragma once
#include "afxcmn.h"

#include "RfServece.h"
#include "ListAdapter.h"
#include "afxwin.h"

// CRfInfoDlg �Ի���


class CDlgRfSignalInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRfSignalInfo)

public:
	CDlgRfSignalInfo(RfService* pRfService, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRfSignalInfo();

// �Ի�������
	enum { IDD = IDD_CHILD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CListCtrl m_cList;

public:
	afx_msg void OnBnClickedButton1();  //��ȡ��Ϣ
	//afx_msg void OnBnClickedButton2();  //��Ϣ�л�

	afx_msg void OnBnClickedCheck1();   //�Զ���ȡ
	BOOL m_bIsCheck;

	afx_msg void OnBnClickedCheck2();   //�Զ�ˢ��
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
