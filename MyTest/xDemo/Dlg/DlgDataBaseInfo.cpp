// RfDataBaseInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgDataBaseInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"


// CRfDataBaseInfoDlg 对话框

IMPLEMENT_DYNAMIC(CDlgDataBaseInfo, CDialog)

CDlgDataBaseInfo::CDlgDataBaseInfo(CMainService* pService, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataBaseInfo::IDD, pParent)
	, m_pService(pService) 
{
	m_pService->m_pDataBaseReader->SetReadDataCallBack(this);
	m_pService->m_pFileDevInfoReader->SetReadDataCallBack(this);
	m_pService->m_pDataBaseReader->SetReadDataCallBack2(this);
}

CDlgDataBaseInfo::~CDlgDataBaseInfo()
{
}

void CDlgDataBaseInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cReaderList);
	DDX_Control(pDX, IDC_LIST2, m_cRegionList);
}


BEGIN_MESSAGE_MAP(CDlgDataBaseInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgDataBaseInfo::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgDataBaseInfo::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgDataBaseInfo::OnBnClickedButton3)
END_MESSAGE_MAP()


// CRfDataBaseInfoDlg 消息处理程序

BOOL CDlgDataBaseInfo::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  在此添加额外的初始化

	{
		CListCtrl& cListCtrl = m_cReaderList;

		cListCtrl.InsertColumn(0, "SN",    LVCFMT_LEFT, 30);
		cListCtrl.InsertColumn(1, "ID",    LVCFMT_LEFT, 50);
		cListCtrl.InsertColumn(2, "IP",    LVCFMT_LEFT, 120);
		cListCtrl.InsertColumn(3, "PORT",  LVCFMT_LEFT, 50);
		cListCtrl.InsertColumn(4, "POS_x", LVCFMT_LEFT, 50);
		cListCtrl.InsertColumn(5, "POS_y", LVCFMT_LEFT, 50);

	}
	
	{
		CListCtrl& cListCtrl = m_cRegionList;

		cListCtrl.InsertColumn(0, "SN",    LVCFMT_LEFT,30);
		cListCtrl.InsertColumn(1, "STYLE", LVCFMT_LEFT,80);

		cListCtrl.InsertColumn(2, "P1x(left)",   LVCFMT_LEFT,80);   // 如果是矩形， 则点是逆时针方向
		cListCtrl.InsertColumn(3, "P1y(bottom)", LVCFMT_LEFT,80);
		cListCtrl.InsertColumn(4, "P2x(right)",  LVCFMT_LEFT,80);   // 如果是矩形， 则点是逆时针方向
		cListCtrl.InsertColumn(5, "P2y(top)",    LVCFMT_LEFT,80);

		cListCtrl.InsertColumn(6,  "P1_rea", LVCFMT_LEFT,50);
		cListCtrl.InsertColumn(7,  "P1_tag", LVCFMT_LEFT,100);
		cListCtrl.InsertColumn(8,  "P2_rea", LVCFMT_LEFT,50);
		cListCtrl.InsertColumn(9,  "P2_tag", LVCFMT_LEFT,100);
		cListCtrl.InsertColumn(10, "P3_rea", LVCFMT_LEFT,50);
		cListCtrl.InsertColumn(11, "P3_tag", LVCFMT_LEFT,100);
		cListCtrl.InsertColumn(12, "P4_rea", LVCFMT_LEFT,50);
		cListCtrl.InsertColumn(13, "P4_tag", LVCFMT_LEFT,100);

		cListCtrl.Update(0);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgDataBaseInfo::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

// 	DataBaseReader* pReader = m_pDataBaseReader;
// 
// 	int nRet = pReader->ReadFromDataBase();
// 	if ( 0 != nRet )
// 	{
// 		CString str;
// 		str.Format("读取失败, 错误码(%d)", nRet);
// 		MY_MSGBOX_F(str);
// 	}
// 	else
// 	{
// 		MY_MSGBOX_F("读取成功");
// 	}

}


void CDlgDataBaseInfo::OnReadReaderData(int nCount, const VEC_ITER_PREADER_T& vecReaderInfos)
{
	m_cReaderList.DeleteAllItems();

	RfReader_RW* pReader = NULL;
	CString str;

	for (int n=0; n<nCount; n++)
	{
		pReader = vecReaderInfos[n];
	    const RF_INFO_READER_IN& data = pReader->Info();

		str.Format("%d",   n);         m_cReaderList.InsertItem(n, str);
		str.Format("%d",   data.ID);   m_cReaderList.SetItemText(n, 1, str);
		str.Format("%s",   data.IP);   m_cReaderList.SetItemText(n, 2, str);
		str.Format("%d",   data.Port); m_cReaderList.SetItemText(n, 3, str);
		str.Format("%.2f", data.x);    m_cReaderList.SetItemText(n, 4, str);
		str.Format("%.2f", data.y);    m_cReaderList.SetItemText(n, 5, str);

	}
}

void CDlgDataBaseInfo::OnReadRegionData(int nCount, const VEC_ITER_PREGION_T& vecRegionInfos)
{
	m_cRegionList.DeleteAllItems();

	RfRegion_RW* pRegion = NULL;
	CString str;
	CListCtrl& cList = m_cRegionList;

	for (int n=0; n<nCount; n++)
	{
		pRegion = vecRegionInfos[n];
		const RfRegion_RW::SData& data = pRegion->Info();

		str.Format("%d",   n);              cList.InsertItem(n, str);

		switch(data.nRectType)
		{
		case 0:
			str.Format("矩形(%d)", data.nRectType); 
			cList.SetItemText(n, 1, str);
			break;
		case 1:
			str.Format("矩形2(%d)", data.nRectType); 
			cList.SetItemText(n, 1, str);
			break;
		case 2:
			str.Format("线性(%d)", data.nRectType); 
			cList.SetItemText(n, 1, str);
			break;
		case 3:
			str.Format("点(%d)", data.nRectType); 
			cList.SetItemText(n, 1, str);
			break;
		case 4:
			str.Format("125K(%d)", data.nRectType); 
			cList.SetItemText(n, 1, str);
			break;
		default:
			break;
		}


		switch(data.nRectType)
		{
		case 0:
		case 1:
			{
				const RF_INFO_REGION_RECT_IN* p = &data.stRect;

				str.Format("%d", p->left);   cList.SetItemText(n, 2, str);
				str.Format("%d", p->bottom); cList.SetItemText(n, 3, str);
				str.Format("%d", p->right);  cList.SetItemText(n, 4, str);
				str.Format("%d", p->top);    cList.SetItemText(n, 5, str);

				str.Format("%d", p->ReaID[0]); cList.SetItemText(n, 6, str);
				str.Format("%x", p->TagID[0]); cList.SetItemText(n, 7, str);
				str.Format("%d", p->ReaID[1]); cList.SetItemText(n, 8, str);
				str.Format("%x", p->TagID[1]); cList.SetItemText(n, 9, str);
				str.Format("%d", p->ReaID[2]); cList.SetItemText(n, 10, str);
				str.Format("%x", p->TagID[2]); cList.SetItemText(n, 11, str);
				str.Format("%d", p->ReaID[3]); cList.SetItemText(n, 12, str);
				str.Format("%x", p->TagID[3]); cList.SetItemText(n, 13, str);
			}
			break;
		case 2:
			{
				const RF_INFO_REGION_LINE_IN* p = &data.stLine;

				str.Format("%.2f", p->P1x); cList.SetItemText(n, 2, str);
				str.Format("%.2f", p->P1y); cList.SetItemText(n, 3, str);
				str.Format("%.2f", p->P2x); cList.SetItemText(n, 4, str);
				str.Format("%.2f", p->P2y); cList.SetItemText(n, 5, str);

				str.Format("%d", p->P1_ReaderID); cList.SetItemText(n, 6, str);
				str.Format("%x", p->P1_TagID);    cList.SetItemText(n, 7, str);
				str.Format("%d", p->P2_ReaderID); cList.SetItemText(n, 8, str);
				str.Format("%x", p->P2_TagID);    cList.SetItemText(n, 9, str);
			}
			break;
		case 3:
		case 4:
			{
				const RF_INFO_REGION_POINT_IN* p = &data.stPoint;

				str.Format("%.2f", p->x); cList.SetItemText(n, 2, str);
				str.Format("%.2f", p->y); cList.SetItemText(n, 3, str);

				str.Format("%d", p->uiReaderID); cList.SetItemText(n, 6, str);
			}
			break;
		default:
			break;
		}

	}
}

void CDlgDataBaseInfo::OnReadReaderData(const RfDevVarsInFile_RW::SData* pData)
{
	m_cReaderList.DeleteAllItems();

	if ( pData->nReaderNum <=0 )
	{
		return;
	}

	RF_INFO_READER_IN* pReader = NULL;
	CString str;

	for (int n=0; n<pData->nReaderNum; n++)
	{
		RF_INFO_READER_IN& data = pData->pReaderList[n];

		str.Format("%d",   n);         m_cReaderList.InsertItem(n, str);
		str.Format("%d",   data.ID);   m_cReaderList.SetItemText(n, 1, str);
		str.Format("%s",   data.IP);   m_cReaderList.SetItemText(n, 2, str);
		str.Format("%d",   data.Port); m_cReaderList.SetItemText(n, 3, str);
		str.Format("%.2f", data.x);    m_cReaderList.SetItemText(n, 4, str);
		str.Format("%.2f", data.y);    m_cReaderList.SetItemText(n, 5, str);

	}
}

void CDlgDataBaseInfo::OnReadData(
	DataBaseReader::READER_MAP_T& cMap1, 
	DataBaseReader::REGION_POINT_MAP_T& cMap2, 
	DataBaseReader::REGION_LINE_MAP_T& cMap3, 
	DataBaseReader::REGION_RECT_MAP_T& cMap4)
{
	m_cReaderList.DeleteAllItems();
	m_cRegionList.DeleteAllItems();
	CString str;

	int nNum = cMap1.Size();   
	if ( nNum > 0 )
	{
		CListCtrl& cList = m_cReaderList;
		RF_INFO_READER_IN data;
		DataBaseReader::READER_MAP_T::Iter cIter(cMap1);

		int nKey;
		int n = 0;
		while( cIter.Next(nKey, data) )
		{
			str.Format("%d",   n);         cList.InsertItem(n, str);
			str.Format("%d",   data.ID);   cList.SetItemText(n, 1, str);
			str.Format("%s",   data.IP);   cList.SetItemText(n, 2, str);
			str.Format("%d",   data.Port); cList.SetItemText(n, 3, str);
			str.Format("%.2f", data.x);    cList.SetItemText(n, 4, str);
			str.Format("%.2f", data.y);    cList.SetItemText(n, 5, str);

			n++;
		}

	}


	nNum = cMap2.Size();   
	if ( nNum > 0 )
	{
		CListCtrl& cList = m_cRegionList;
		RF_INFO_REGION_POINT_IN data;
		DataBaseReader::REGION_POINT_MAP_T::Iter cIter(cMap2);

		int nKey;
		int n = 0;
		while( cIter.Next(nKey, data) )
		{
		
			if ( 0 != data.uiReaderID )
			{
				str.Format("%d", n);      cList.InsertItem(n, str);
				str.Format("点(%d)", 3);  cList.SetItemText(n, 1, str);
			}
			else if ( 0 != data.uiTriggerID )
			{
				str.Format("%d",   n);     cList.InsertItem(n, str);
				str.Format("125K(%d)", 4); cList.SetItemText(n, 1, str);
			}
			else
			{
				continue;
			}

			str.Format("%.2f", data.x);          cList.SetItemText(n, 2, str);
			str.Format("%.2f", data.y);          cList.SetItemText(n, 3, str);
			str.Format("%d",   data.uiReaderID); cList.SetItemText(n, 6, str);

			n++;
		}

	}


}

void CDlgDataBaseInfo::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

// 	DataBaseLogin*		pLogin = m_pService->m_pLogin;
// 	DataBaseReader*		pData  = m_pService->m_pDataBaseReader;
// 	RfService*			pServe = m_pService->m_pRfService;
// 	RfDevVarsInFile_RW* pData2 = m_pService->m_pFileDevInfoReader;
// 
// 	int nRet;
// 
// 	if ( false == pLogin->IsLogin() )
// 	{
// 		MY_MSGBOX_F("请先登录");
// 		return;
// 	}
// 
// 	{  // 读数据库启动
// // 		nRet = pData->ReadFromDataBase();
// // 		if ( 0 != nRet )
// // 		{
// // 			DB_LOG_F("读取数据库失败, nRet = %d", nRet);
// // 			MY_MSGBOX_F("读取数据库失败");
// // 			return;
// // 		}
// // 
// // 		if ( 0 != pServe->StartCheck(pData) )
// // 		{
// // 			DB_LOG_F("启动服务失败, nRet = %d", nRet);
// // 			MY_MSGBOX_F("启动服务失败");
// // 			return;
// // 		}
// 	}
// 
// 	{  // 读文件启动
// 		nRet = pData2->Read();
// 		if ( 0 != nRet )
// 		{
// 			DB_LOG_F("读取RF配置文件失败, nRet = %d", nRet);
// 			MY_MSGBOX_F("读取数据库失败");
// 			return;
// 		}
// 
// 		if ( 0 != pServe->StartCheck(pData2) )
// 		{
// 			DB_LOG_F("启动服务失败, nRet = %d", nRet);
// 			MY_MSGBOX_F("启动服务失败");
// 			return;
// 		}
// 	}
// 
// 	m_pService->m_pDog->SaveServiceStatus(1);
// 	MY_MSGBOX_F("启动服务成功");
// 
// 	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
}


void CDlgDataBaseInfo::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

// 	DataBaseLogin*  pLogin = m_pService->m_pLogin;
// 	DataBaseReader* pData  = m_pService->m_pDataBaseReader;
// 	RfService*      pServe = m_pService->m_pRfService;
// 
// 	if ( true == pLogin->IsLogin() )
// 	{
// 		MY_MSGBOX_F("请先注销");
// 		return;
// 	}
// 
// 	pServe->StopCheck();
// 	m_pService->m_pDog->SaveServiceStatus(0);
// 	MY_MSGBOX_F("关闭服务成功");
// 
// 	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
// 	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
}
