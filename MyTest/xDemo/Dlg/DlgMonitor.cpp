// DlgMonitor.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgMonitor.h"
#include "afxdialogex.h"


// CDlgMonitor 对话框

IMPLEMENT_DYNAMIC(CDlgMonitor, CDialog)

CDlgMonitor::CDlgMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMonitor::IDD, pParent)
{

}

CDlgMonitor::~CDlgMonitor()
{
}

void CDlgMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_cSta1);
}


BEGIN_MESSAGE_MAP(CDlgMonitor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMonitor::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgMonitor::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgMonitor::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgMonitor::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgMonitor::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDlgMonitor::OnBnClickedButton6)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgMonitor 消息处理程序


void CDlgMonitor::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(1);
}


void CDlgMonitor::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(2);
}


void CDlgMonitor::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(3);
}


void CDlgMonitor::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(4);
}


void CDlgMonitor::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(5);
}


void CDlgMonitor::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSubWindow(6);
}


void CDlgMonitor::ShowSubWindow(int nIndex)
{
	m_pDlg1->ShowWindow(SW_HIDE);
	m_pDlg2->ShowWindow(SW_HIDE);
	m_pDlg3->ShowWindow(SW_HIDE);
	m_pDlg4->ShowWindow(SW_HIDE);
	m_pDlg5->ShowWindow(SW_HIDE);
	m_pDlg6->ShowWindow(SW_HIDE);

	switch(nIndex)
	{
	case 1:
		m_pDlg1->ShowWindow(SW_SHOW);
		break;
	case 2:
		m_pDlg2->ShowWindow(SW_SHOW);
		break;
	case 3:
		m_pDlg3->ShowWindow(SW_SHOW);
		break;
	case 4:
		m_pDlg4->ShowWindow(SW_SHOW);
		break;
	case 5:
		m_pDlg5->ShowWindow(SW_SHOW);
		break;
	case 6:
		m_pDlg6->ShowWindow(SW_SHOW);
		break;
	default:
		break;
	}

}


BOOL CDlgMonitor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_pDlg1 = new CDlgDataBaseInfo(&g_cService);
	ASSERT(m_pDlg1);
	m_pDlg1->Create(IDD_CHILD1, GetDlgItem(IDC_STATIC_CHILD));

	m_pDlg2 = new CDlgRfSignalInfo(g_cService.m_pRfService);
	ASSERT(m_pDlg2);
	m_pDlg2->Create(IDD_CHILD2, GetDlgItem(IDC_STATIC_CHILD));

	m_pDlg3 = new CDlgRfImportInfo(g_cService.m_pRfService);
	ASSERT(m_pDlg3);
	m_pDlg3->Create(IDD_CHILD3, GetDlgItem(IDC_STATIC_CHILD));

	m_pDlg4 = new CDlgRfAlgInfo(g_cService.m_pRfService);
	ASSERT(m_pDlg4);
	m_pDlg4->Create(IDD_CHILD4, GetDlgItem(IDC_STATIC_CHILD));

	m_pDlg5 = new CDlgRfExportInfo(g_cService.m_pRfService);
	ASSERT(m_pDlg5);
	m_pDlg5->Create(IDD_CHILD5, GetDlgItem(IDC_STATIC_CHILD));

	m_pDlg6 = new CDlgLogInfo();
	ASSERT(m_pDlg6);
	m_pDlg6->Create(IDD_CHILD6, GetDlgItem(IDC_STATIC_CHILD));

	ShowSubWindow(1);

	m_cSta1.ShowWindow(SW_SHOW);
	BOOL bLoadSuc = m_cSta1.Load(_T("gif3.gif"));
	m_cSta1.SetPaintRect(CRect(0,0,100,150));
	if ( bLoadSuc )
	{
		m_cSta1.Draw();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgMonitor::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	m_pDlg1->DestroyWindow();
	delete m_pDlg1;

	m_pDlg2->DestroyWindow();
	delete m_pDlg2;

	m_pDlg3->DestroyWindow();
	delete m_pDlg3;

	m_pDlg4->DestroyWindow();
	delete m_pDlg4;

	m_pDlg5->DestroyWindow();
	delete m_pDlg5;

	m_pDlg6->DestroyWindow();
	delete m_pDlg6;

}
