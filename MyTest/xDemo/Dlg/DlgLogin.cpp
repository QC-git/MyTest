// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgLogin.h"
#include "afxdialogex.h"

/*#include "Test.h"*/

// CLoginDlg �Ի���

#define  WM_SERVER_CONNECT_SUCCESS			WM_USER+10001
#define  WM_SERVER_CONNECT_FAIL				WM_USER+10002

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)

CDlgLogin::CDlgLogin(CWnd* pParent)
	: CDialog(CDlgLogin::IDD, pParent)
	//, m_pLogin(pDataBaseOpter)
	, m_bIsSetBtnPress(false)
	, m_bIsHaveLogin(false)
	, m_edtUserName(_T(""))
	, m_edtUserPassword(_T(""))
{
	//m_pLogin = new DataBaseOperater;
	//ASSERT(m_pLogin);
	//m_pLogin->SetLoginCallBack(this);
}



CDlgLogin::~CDlgLogin()
{
	//delete m_pLogin;

}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_LOGIN, m_cProgressCtrl);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_edtUserName);
	DDV_MaxChars(pDX, m_edtUserName, 16);
	DDX_Text(pDX, IDC_EDIT_PASS_WORD, m_edtUserPassword);
	DDV_MaxChars(pDX, m_edtUserPassword, 16);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_cBtn1);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CDlgLogin::OnBnClickedButtonLogin)
	//ON_BN_CLICKED(IDC_BUTTON_SAVE, &CLoginDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CDlgLogin::OnBnClickedButtonSet)
	//ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CLoginDlg::OnBnClickedButtonLogout)
	//ON_COMMAND(WM_SERVER_CONNECT_SUCCESS, &CLoginDlg::OnCmdLoginSuc)
	//ON_COMMAND(WM_SERVER_CONNECT_FAIL, &CLoginDlg::OnCmdLoginSuc)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������


void CDlgLogin::OnBnClickedButtonLogin()
{
	TRACE("\n CLoginDlg::OnBnClickedButtonLogin()");
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString str1, str2;
	str1.Format("��¼");
	m_cBtn1.GetWindowTextA(str2);

	if ( str1 == str2 )
	{
		DoLogin();
	}
	else
	{
		DoLogout();
	}

}


void CDlgLogin::OnBnClickedButtonSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}


void CDlgLogin::OnBnClickedButtonSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//this->SetTimer(1, 200, NULL);
	m_bIsSetBtnPress = !m_bIsSetBtnPress;
	SetBtnModeUpData();
}


void CDlgLogin::OnBnClickedButtonLogout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CDlgLogin::OnLogionStatus(bool bIsLogionSuc, void* dummy)
{
	if (bIsLogionSuc)
		::PostMessage(m_hWnd, WM_SERVER_CONNECT_SUCCESS, 0, 0);
	else
		::PostMessage(m_hWnd, WM_SERVER_CONNECT_FAIL, (WPARAM)dummy, 0);
}


BOOL CDlgLogin::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
// 	if ( WM_SERVER_CONNECT_SUCCESS == pMsg->message )
// 	{
// 		m_cProgressCtrl.SetPos(100);
// 		MY_MSGBOX_F(_T("��¼�ɹ�"));
// 
// 
// 		GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowText("ע��");  m_bIsHaveLogin = true; // �ı䰴������
// 		GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(TRUE);  // �ָ�����״̬
// 
// 	}
// 	if ( WM_SERVER_CONNECT_FAIL == pMsg->message )
// 	{
// 		m_cProgressCtrl.SetPos(0);
// 
// 		CString str;
// 		str.Format("��¼ʧ��, ������(%d)" , (int)pMsg->wParam);
// 		MY_MSGBOX_F(str);
// 
// 		GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(TRUE);  // �ָ�����״̬
// 	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgLogin::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	CRect rect;
	CString strT;
	GetWindowRect(&rect);
	SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height() - 270, SWP_NOZORDER);

	m_cProgressCtrl.SetRange(0, 100);
	m_cProgressCtrl.SetStep(15);
	m_cProgressCtrl.SetPos(0);

// 	m_pDetailDlg = new CLoginDetailInfoDlg(m_pLogin->DataBaseInfo());
// 	m_pDetailDlg->Create(IDD_CHILD_LOGIN_SERVERINFO, GetDlgItem(IDC_STA_CHILD_DETAILINFO));

	SetBtnModeUpData();

	m_edtUserName.Format("ROPEOK"); //Ĭ���û���
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgLogin::OnDestroy()
{
	CDialog::OnDestroy();

// 	if (m_pDetailDlg)
// 	{
// 		m_pDetailDlg->DestroyWindow();
// 		delete m_pDetailDlg;
// 		m_pDetailDlg = NULL;
// 	}

}

void CDlgLogin::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if ( 1 == nIDEvent )
	{
		m_cProgressCtrl.StepIt();
	}

	__super::OnTimer(nIDEvent);
}

void CDlgLogin::SetBtnModeUpData()
{
// 	if (!m_bIsSetBtnPress)
// 	{
// 		GetDlgItem(IDC_STATIC_BKPICTURE)->ShowWindow(SW_SHOW);
// 		m_pDetailDlg->ShowWindow(SW_HIDE);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_STATIC_BKPICTURE)->ShowWindow(SW_HIDE);
// 		m_pDetailDlg->ShowWindow(SW_SHOW);
// 	}

}

void CDlgLogin::DoLogin()
{
	UpdateData(TRUE);
	CString strRet;

	CMainService* pSer = &g_cService;

	m_cProgressCtrl.SetPos(0);

	if ( m_edtUserName.GetLength() < 1 || m_edtUserPassword.GetLength() < 1 )
	{
		MY_MSGBOX_F("�û���������Ϊ��");
		return;
	}

	int nRet = pSer->m_pLogin->Login2(m_edtUserName, m_edtUserPassword);
	if ( 0 != nRet )
	{
		strRet.Format("��¼ʧ�� ������(%d)", nRet);
		MY_MSGBOX_F(strRet);
		return;
	}

	pSer->m_pDog->SaveLoginStatus(1);
	DB_LOG_F("��¼�ɹ�");
	MY_MSGBOX_F("��¼�ɹ�");

	// �ı�ͼ��״̬
	m_cProgressCtrl.SetPos(100);
	m_cBtn1.SetWindowTextA("ע��");
}

void CDlgLogin::DoLogout()
{
	UpdateData(TRUE);
	CString strRet;

	CMainService* pSer = &g_cService;

	if ( m_edtUserName.GetLength() < 1 || m_edtUserPassword.GetLength() < 1 )
	{
		MY_MSGBOX_F("�û���������Ϊ��");
		return;
	}

	// ע������
	int nErr = pSer->m_pLogin->Logout2(m_edtUserName, m_edtUserPassword);
	if ( 0 != nErr )
	{
		strRet.Format("ע��ʧ�� ������(%d)", nErr);
		MY_MSGBOX_F(strRet);
		return;
	}

	pSer->m_pDog->SaveLoginStatus(0);
	DB_LOG_F("ע���ɹ�");
	MY_MSGBOX_F("ע���ɹ�");
	
	// �ı�ͼ��״̬
	m_cProgressCtrl.SetPos(0);
	m_cBtn1.SetWindowTextA("��¼");
}

void CDlgLogin::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

// 	CRect rect;
// 	GetClientRect(&rect);
// 	dc.FillSolidRect(&rect, RGB(171, 200, 238));

}
