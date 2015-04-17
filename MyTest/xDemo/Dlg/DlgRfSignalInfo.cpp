// RfInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgRfSignalInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"

// CRfInfoDlg 对话框


IMPLEMENT_DYNAMIC(CDlgRfSignalInfo, CDialog)

CDlgRfSignalInfo::CDlgRfSignalInfo(RfService* pRfService, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRfSignalInfo::IDD, pParent)
	, m_bIsCheck(FALSE)
	, m_pRfService(pRfService)
	, m_pListAdapter(NULL)
	, m_bIsAutoUpdataInfo(FALSE)
	, m_cComboValue(_T(""))
{

}

CDlgRfSignalInfo::~CDlgRfSignalInfo()
{
}

void CDlgRfSignalInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Check(pDX, IDC_CHECK1, m_bIsCheck);
	DDX_Check(pDX, IDC_CHECK2, m_bIsAutoUpdataInfo);
	DDX_CBString(pDX, IDC_COMBO1, m_cComboValue);
	DDX_Control(pDX, IDC_COMBO1, m_cComboControl);
}


BEGIN_MESSAGE_MAP(CDlgRfSignalInfo, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgRfSignalInfo::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgRfSignalInfo::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgRfSignalInfo::OnBnClickedCheck2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgRfSignalInfo::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


BOOL CDlgRfSignalInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT);// | LVS_EX_GRIDLINES);
	CString str;
	m_cList.InsertColumn(0,"",LVCFMT_LEFT,80);
	for (int n=0; n<100; n++)
	{
		str.Format("%d",n+1);
		m_cList.InsertColumn(n+1,str,LVCFMT_LEFT,70);
	}
	m_cList.Update(0);

	
	m_pListAdapter = new CRfListAdapter(this->m_hWnd, &m_cList);

	m_cComboControl.InsertString(0, MD_STRING_RSSI);
	m_cComboControl.InsertString(1, MD_STRING_COUNT);
	m_cComboControl.InsertString(2, MD_STRING_TIME);
	m_cComboControl.InsertString(3, MD_STRING_POWER);
	m_cComboControl.InsertString(4, MD_STRING_ANT);
	m_cComboControl.InsertString(5, MD_STRING_ANTRSSI);
	m_cComboControl.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgRfSignalInfo::OnDestroy()
{
	CDialog::OnDestroy();

	delete m_pListAdapter;
	m_pListAdapter = NULL;

	// TODO: 在此处添加消息处理程序代码
}

BOOL CDlgRfSignalInfo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( WM_REQUEST_LIST_UPDATA == pMsg->message )
	{
		UpdateData(TRUE);
		//LOG_F("\n CRfInfoDlg::PreTranslateMessage(), m_cComboValue = %s", m_cComboValue);

		if (m_bIsAutoUpdataInfo)
			m_pListAdapter->UpDataList(m_cComboValue);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// CRfInfoDlg 消息处理程序


void CDlgRfSignalInfo::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);
	//LOG_F("\n CRfInfoDlg::OnBnClickedButton1(), m_cComboValue = %s", m_cComboValue);

	m_pListAdapter->UpDataList(m_cComboValue);
}


void CDlgRfSignalInfo::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);
	//LOG_F("\n CRfInfoDlg::OnBnClickedButton1(), m_cComboValue = %s", m_cComboValue);

	m_pListAdapter->UpDataList(m_cComboValue);
}


void CDlgRfSignalInfo::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	LOG_F("CRfInfoDlg::OnBnClickedCheck1(), m_bIsCheck = %d", m_bIsCheck);

	if (m_bIsCheck)
	{
		apiSetRssiInfoCallBack(&CDlgRfSignalInfo::AfterGetRssiInfo, (DWORD)this);
	}
	else
	{
		apiSetRssiInfoCallBack(NULL, NULL);
	}

}


void CDlgRfSignalInfo::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


int CDlgRfSignalInfo::AfterGetRssiInfo1(RF_INFO_RSSI_OUT* pInfoList, int nInfoNum)
{
	if ( nInfoNum <= 0 || NULL == m_pListAdapter ) return -1;

	RF_INFO_RSSI_OUT* p;
	for (int n=0; n<nInfoNum; n++)
	{
		p = &pInfoList[n];

		m_pListAdapter->AddNewUpData(p->lReaderId, p->lTagId, p);

	}

	return nInfoNum;
}














