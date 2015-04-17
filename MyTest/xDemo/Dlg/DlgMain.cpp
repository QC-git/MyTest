// DlgMain.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgMain.h"
#include "afxdialogex.h"

#include "RkUtil.h"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

// class CAboutDlg : public CDialog
// {
// public:
// 	CAboutDlg();
// 
// 	// �Ի�������
// 	enum { IDD = IDD_ABOUTBOX };
// 
// protected:
// 	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
// 
// 	// ʵ��
// protected:
// 	DECLARE_MESSAGE_MAP()
// };
// 
// CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
// {
// }
// 
// void CAboutDlg::DoDataExchange(CDataExchange* pDX)
// {
// 	CDialog::DoDataExchange(pDX);
// }
// 
// BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
// END_MESSAGE_MAP()

// CDlgMain �Ի���

#define  WM_TRAY_CLICK   WM_USER+10001

IMPLEMENT_DYNAMIC(CDlgMain, CDialog)

CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMain::IDD, pParent)
{
	TRACE("\n CDlgMain()");
}

CDlgMain::~CDlgMain()
{
	TRACE("\n ~CDlgMain()");
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_cBtn1);
	DDX_Control(pDX, IDC_BUTTON2, m_cBtn2);
	DDX_Control(pDX, IDC_BUTTON3, m_cBtn3);
	DDX_Control(pDX, IDC_BUTTON4, m_cBtn4);
	DDX_Control(pDX, IDC_BUTTON5, m_cBtn5);
	DDX_Control(pDX, IDC_TEXT1, m_cStaText1);
	DDX_Control(pDX, IDC_TEXT2, m_cStaText2);
	DDX_Control(pDX, IDC_STATIC_CHART, m_cStaPicture);
}


BEGIN_MESSAGE_MAP(CDlgMain, CDialog)
	ON_WM_TIMER()
	//ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMain::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgMain::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgMain::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgMain::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgMain::OnBnClickedButton5)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgMain ��Ϣ�������


void CDlgMain::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if ( 1 == nIDEvent )
	{

		int nValue = m_CpuEx.GetStatusValue();

// 		static bool s_bIsFirst = true;
// 		if (s_bIsFirst)
// 		{
// 			nValue = 100;
// 			s_bIsFirst = false;
// 		}

		if ( nValue > 100 )
		{
			nValue = 100;
		}
		
		m_pLineSerie->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray,m_c_arrayLength, (double)nValue);
		LeftMoveArray(m_X,m_c_arrayLength,m_count);
		m_pLineSerie->AddPoints(m_X,m_HightSpeedChartArray,m_c_arrayLength);

		m_count++;

		//TRACE("\n nValue = %d", nValue);
	}

	if ( 2 == nIDEvent )
	{
		g_cService.OnMonitor();
	}

	CDialog::OnTimer(nIDEvent);
}


// LRESULT CDlgMain::OnNcHitTest(CPoint point)
// {
// 	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
// 
// 	return CDialog::OnNcHitTest(point);
// }

void CDlgMain::LeftMoveArray(double* ptr,size_t length,double data)
{
	for (size_t i=1;i<length;++i)
	{
		ptr[i-1] = ptr[i];
	}
	ptr[length-1] = data;
}

// void CDlgMain::SetUpRegon()
// {
// 	CDC*      pDC;
// 	CDC	      memDC;
// 	CBitmap	  cBitmap;
// 	CBitmap*  pOldMemBmp = NULL;
// 	COLORREF  col;
// 	CRect     cRect;
// 	int	      x, y;
// 	CRgn      wndRgn, rgnTemp;
// 
// 	pDC = GetDC();
// 	GetClientRect(&cRect);
// 
// 
// 	//��ģ��λͼѡ���豸������
// 	cBitmap.LoadBitmap(IDB_BITMAP1);
// 	memDC.CreateCompatibleDC(pDC);
// 	pOldMemBmp = memDC.SelectObject(&cBitmap);
// 
// 	//����ģ����״�Ĳ���������
// 	wndRgn.CreateRectRgn(0, 0, cRect.Width(), cRect.Height());
// 	for(x=0; x<=cRect.Width(); x++)
// 	{
// 		for(y=0; y<=cRect.Height(); y++)
// 		{
// 			col = memDC.GetPixel(x, y);
// 			UCHAR by1 = (UCHAR)(col>>8);
// 			UCHAR by2 = (UCHAR)(col>>16);
// 			UCHAR by3 = (UCHAR)(col>>24);
// 			UCHAR byLimit = 15;
// 			if( by1 <= byLimit && 
// 				by2 <= byLimit &&
// 				by3 <= byLimit)
// 			{
// 				rgnTemp.CreateRectRgn(x, y, x+1, y+1);
// 				wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_XOR);
// 				rgnTemp.DeleteObject();	
// 			}
// 		}
// 	}
// 
// 	if(pOldMemBmp)
// 	{
// 		memDC.SelectObject(pOldMemBmp);
// 	}
// 
// 	//�������������������壬Ҳ���Ǵ���������Ĵ���
// 	SetWindowRgn((HRGN)wndRgn, TRUE);
// 	ReleaseDC(pDC);
// }

void CDlgMain::InitChart()
{
	CRect cRect;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(cRect);
	GetDlgItem(IDC_STATIC_CHART)->ShowWindow(SW_HIDE);
	ScreenToClient(cRect);

	// ƫ�������ʵ�����
	cRect.left-=18;
	cRect.right+=18;
	cRect.top-=18;
	cRect.bottom+=18;

	m_cTab.Create(CMFCTabCtrl::STYLE_3D_ROUNDED,//�ؼ���ʽ��������������ʾ 
		cRect,//�ؼ�����
		this,//�ؼ��ĸ�����ָ��
		1,//�ؼ�ID
		CMFCTabCtrl::LOCATION_TOP);//��ǩλ��	
	m_cTab.AutoSizeWindow(TRUE);//�����öԻ�����tab��ʾ��������ţ�ͬʱ�����Ի����OnSize��Ϣ
	//m_cTab.ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(cRect);
	m_cHSChartCtrl.Create(&m_cTab,cRect,2);

	//��ʼ������
	CChartAxis *pAxis = NULL;
	pAxis = m_cHSChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_cHSChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetMinMax(0.0f, 100);
	pAxis->SetAutomatic(true);

	m_pLineSerie = m_cHSChartCtrl.CreateLineSerie();

	UpdateData(FALSE);
	m_cTab.AddTab(&m_cHSChartCtrl,_T("������״̬"));  // tab ����
	m_cTab.SetActiveTab(0);//������ʾ��һҳ��     
	m_cTab.ShowWindow(SW_SHOWNORMAL);

	// 	CRect cTabBRect,cTabTRect;
	// 	m_cTab.GetWindowRect(cRect);
	// 	ScreenToClient(cRect);
	// 	m_cTab.GetTabArea(cTabTRect,cTabBRect);
	//	cRect.top+=TabTRect.Height();

	//
	m_c_arrayLength = 20;
	memset(m_HightSpeedChartArray, 0, sizeof(double)*m_c_arrayLength);
	memset(m_X, 0, sizeof(double)*m_c_arrayLength);
	m_count = 0;

	InitCpuStatus(true);

	SetTimer(1, 1000, NULL);
}

void CDlgMain::InitCpuStatus(bool bThread)
{
	int nRet = m_CpuEx.Init();
	m_CpuEx.SetWnd(this->m_hWnd);

	if (bThread)
	{
		m_CpuEx.Start();
	}
	
}

long CDlgMain::GetCpuStatue()
{
	return 0;
}

BOOL CDlgMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	g_cService.OnCreate();
	{
		CRkWatchDog* pDog = g_cService.m_pDog;

		if ( NULL != pDog && pDog->IsLastCloseError() )
		{
			DB_LOG_F("��⵽�����������쳣");
			DB_LOG_F("	�ϴε�¼״̬��%d", pDog->IsEverLogin());
			DB_LOG_F("	�ϴη���״̬��%d", pDog->IsEverStartSevice());
		}
	}

	m_cStaText2.SetWindowTextA("����");

	m_cBtn1.LoadBitmaps(IDB_BITMAP11, IDB_BITMAP12);
	m_cBtn2.LoadBitmaps(IDB_BITMAP3, IDB_BITMAP4);
	m_cBtn4.LoadBitmaps(IDB_BITMAP5, IDB_BITMAP6);
	m_cBtn3.LoadBitmaps(IDB_BITMAP7, IDB_BITMAP8);
	m_cBtn5.LoadBitmaps(IDB_BITMAP9, IDB_BITMAP10);
	//m_cBtn1.SizeToContent();

	m_cLoginDlg.Create(CDlgLogin::IDD, this);
	m_cLoginDlg.ShowWindow(SW_HIDE);

	m_cMonitorDlg.Create(CDlgMonitor::IDD, this);
	m_cMonitorDlg.ShowWindow(SW_HIDE);

	m_cSettingDlg.Create(CDlgSetting::IDD, this);
	m_cSettingDlg.ShowWindow(SW_HIDE);

	if ( true )
	{
		InitChart();
	}
	else
	{
		InitGifPicture();
	}
	

// 	m_pService = new CMyProService;
// 	ASSERT(m_pService);
	

	SetTimer(2, 500, NULL); // ���ں�̨����

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgMain::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("\n CDlgMain::OnBnClickedButton1()");

	CMainService* pService = &g_cService;
	DataBaseLogin*		pLogin = pService->m_pLogin;
	DataBaseReader*		pData  = pService->m_pDataBaseReader;
	RfService*			pServe = pService->m_pRfService;
	RfDevVarsInFile_RW* pData2 = pService->m_pFileDevInfoReader;

	CString str1, str2;
	str1.Format("����");
	m_cStaText1.GetWindowTextA(str2);

	if ( str1 == str2 )
	{

		if ( false == pLogin->IsLogin() )
		{
			MY_MSGBOX_F("���ȵ�¼");
			return;
		}

		{  // �����ݿ�����
			int nRet = pData->ReadFromDataBase2();
			if ( 0 != nRet )
			{
				DB_LOG_F("��ȡ���ݿ�ʧ��, nRet = %d", nRet);
				MY_MSGBOX_F("��ȡ���ݿ�ʧ��");
				return;
			}

			if ( 0 != pServe->StartCheck2(pData) )
			{
				DB_LOG_F("��������ʧ��, nRet = %d", nRet);
				MY_MSGBOX_F("��������ʧ��");
				return;
			}
		}

		{  // ���ļ�����
// 			nRet = pData2->Read();
// 			if ( 0 != nRet )
// 			{
// 				DB_LOG_F("��ȡRF�����ļ�ʧ��, nRet = %d", nRet);
// 				MY_MSGBOX_F("��ȡ���ݿ�ʧ��");
// 				return;
// 			}
// 
//			nRet = pServe->StartCheck(pData2);
// 			if ( 0 != nRet )
// 			{
// 				DB_LOG_F("��������ʧ��, nRet = %d", nRet);
// 				MY_MSGBOX_F("��������ʧ��");
// 				return;
// 			}
		}

		{ // ��������������

// 			nRet = pServe->StartTestCheck3();
// 			if ( 0 != nRet )
// 			{
// 				DB_LOG_F("��������ʧ��, nRet = %d", nRet);
// 				MY_MSGBOX_F("��������ʧ��");
// 				return;
// 			}

		}

		pService->m_pDog->SaveServiceStatus(1);
		DB_LOG_F("��������ɹ�");
		m_cStaText2.SetWindowTextA("���ڹ���...");
		MY_MSGBOX_F("��������ɹ�");

		// �ı�ͼ��״̬
		m_cStaText1.SetWindowTextA("ֹͣ");
		m_cBtn1.LoadBitmaps(IDB_BITMAP13, IDB_BITMAP14);
		m_cBtn1.Invalidate(TRUE);
	}
	else
	{

		if ( true == pLogin->IsLogin() )
		{
			MY_MSGBOX_F("����ע��");
			return;
		}

		pServe->StopCheck();
		pService->m_pDog->SaveServiceStatus(0);
		DB_LOG_F("�رշ���ɹ�");
		m_cStaText2.SetWindowTextA("����");
		MY_MSGBOX_F("�رշ���ɹ�")

		// �ı�ͼ��״̬
		m_cStaText1.SetWindowTextA("����");
		m_cBtn1.LoadBitmaps(IDB_BITMAP11, IDB_BITMAP12);
		m_cBtn1.Invalidate(TRUE);
	}

}


void CDlgMain::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("\n CDlgMain::OnBnClickedButton2()");

	if ( m_cLoginDlg.IsWindowVisible() )
	{
		m_cLoginDlg.ShowWindow(SW_HIDE);
	}
	else
	{
		m_cLoginDlg.ShowWindow(SW_SHOW);
	}

}


void CDlgMain::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("\n CDlgMain::OnBnClickedButton3()");

	if (m_cMonitorDlg.IsWindowVisible())
	{
		m_cMonitorDlg.ShowWindow(SW_HIDE);
	}
	else
	{
		m_cMonitorDlg.ShowWindow(SW_SHOW);
	}

}


void CDlgMain::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("\n CDlgMain::OnBnClickedButton4()");

	if (m_cSettingDlg.IsWindowVisible())
	{
		m_cSettingDlg.ShowWindow(SW_HIDE);
	}
	else
	{
		m_cSettingDlg.ShowWindow(SW_SHOW);
	}

}


void CDlgMain::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("\n CDlgMain::OnBnClickedButton5()");

	CAboutDlg cAboutDlg;
	cAboutDlg.DoModal();

}

void CDlgMain::InitTray()
{
	// ��������ͼ��
	m_Nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_Nid.hWnd = m_hWnd;
	m_Nid.uID = IDR_MAINFRAME;
	m_Nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	m_Nid.uCallbackMessage = WM_TRAY_CLICK; // �Զ������̲˵���Ϣ
	m_Nid.hIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	CString str;
	str.Format("RFID�����");
	strcpy(m_Nid.szTip, str);
	Shell_NotifyIcon(NIM_ADD, &m_Nid); // �����������ͼ��
}

void CDlgMain::DestroyTray()
{
	Shell_NotifyIcon(NIM_DELETE, &m_Nid);
}

void CDlgMain::HideMainWindow()
{
	if(IsWindowVisible())
	{
		this->ShowWindow(SW_HIDE);
	}
}


void CDlgMain::ShowMainWindow()
{
	if(IsWindowVisible())
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, (LPARAM)m_hWnd);
	else
		ShowWindow(SW_SHOW);
}



void CDlgMain::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	m_CpuEx.Stop();
	g_cService.OnDestroy();
//	delete m_pService;

}


LRESULT CDlgMain::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���

	// ������С����ť
// 	if ( WM_SYSCOMMAND == message && SC_MINIMIZE == wParam )
// 	{
// 		//AfxMessageBox(_T("�Ƿ���С�����ڵ�����"));
// 		HideMainWindow();
// 		return 0;
// 	}

	// �����رհ�ť
	if ( WM_SYSCOMMAND == message && SC_CLOSE == wParam )
	{
		if( IDNO == ::MessageBox(this->m_hWnd, _T("ȷ���˳�ϵͳ��?"), _T("��ʾ"), MB_ICONQUESTION | MB_YESNO) )  
		{  
			return 0;  
		}
		else
		{
			DataBaseLogin* pLogin = g_cService.m_pLogin;
			if ( DataBaseLogin::LOGIN_STATUS_NO_CONNECTED != pLogin->LoginStatus() )
			{
				MY_MSGBOX_F(_T("ϵͳ��δע��������ע���ٹر�"));
				return 0; 
			}
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgMain::InitGifPicture()
{
	//m_cGifPicture.Create();

	BOOL bLoadSuc = m_cStaPicture.Load(_T("gif7.gif"));
	//CRect cRect;
	//m_cStaPicture.GetClientRect(cRect);
	m_cStaPicture.SetPaintRect( CRect(0,0,450,200));  //  CRect(0,0,100,100)
	if ( bLoadSuc )
	{
		m_cStaPicture.Draw();
	}

}
