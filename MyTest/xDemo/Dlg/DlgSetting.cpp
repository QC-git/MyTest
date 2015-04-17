// DlgSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgSetting.h"
#include "afxdialogex.h"


// CDlgSetting 对话框

IMPLEMENT_DYNAMIC(CDlgSetting, CDialog)

CDlgSetting::CDlgSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetting::IDD, pParent)
	, m_bIdcCheck1(FALSE)
{

}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bIdcCheck1);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetting::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgSetting::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDlgSetting 消息处理程序


void CDlgSetting::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}


void CDlgSetting::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void CDlgSetting::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	if (m_bIdcCheck1)
	{
		;
	}
	else
	{
		;
	}

}
