// RfParameterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgRfImportInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"

// CRfParameterDlg 对话框

IMPLEMENT_DYNAMIC(CDlgRfImportInfo, CDialog)

CDlgRfImportInfo::CDlgRfImportInfo(RfService* pRfService, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRfImportInfo::IDD, pParent)
	, m_pRfService(pRfService)
{

}

CDlgRfImportInfo::~CDlgRfImportInfo()
{
}

void CDlgRfImportInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctlReaderList);
	DDX_Control(pDX, IDC_LIST2, m_ctlRegionList);
}


BOOL CDlgRfImportInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	{
		CListCtrl& cListCtrl = m_ctlReaderList;

		cListCtrl.InsertColumn(0, "SN",    LVCFMT_LEFT, 30);
		cListCtrl.InsertColumn(1, "ID",    LVCFMT_LEFT, 50);
		cListCtrl.InsertColumn(2, "IP",    LVCFMT_LEFT, 120);
		cListCtrl.InsertColumn(3, "IS NOAML",  LVCFMT_LEFT, 80);
		//cListCtrl.InsertColumn(4, "POS_x", LVCFMT_LEFT, 50);
		//cListCtrl.InsertColumn(5, "POS_y", LVCFMT_LEFT, 50);

	}

	{
		CListCtrl& cListCtrl = m_ctlRegionList;

		cListCtrl.InsertColumn(0, "SN/INDEX",    LVCFMT_LEFT,80);
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

BEGIN_MESSAGE_MAP(CDlgRfImportInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgRfImportInfo::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRfParameterDlg 消息处理程序


void CDlgRfImportInfo::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	int nReaderNum = apiGetReadersInfo(CDlgRfImportInfo::AfterGetReaderInfo, (DWORD)this);

	int nRegionNum = apiGetRegionsInfo(CDlgRfImportInfo::AfterGetRegionInfo, (DWORD)this);

	CString str;
	str.Format("共获得 接收器数据 %d , 区域数据 %d 条 ", nReaderNum, nRegionNum);
	MY_MSGBOX_F(str);
}

int CDlgRfImportInfo::AfterGetReaderInfo1(RF_INFO_READER* readerList, int readerListNum)
{
	LOG_F("CRfParameterDlg::AfterGetReaderInfo(), readerListNum = %d", readerListNum);

	m_ctlReaderList.DeleteAllItems();

	RF_INFO_READER* pReader;
	CString str;
	for (int n=0; n<readerListNum; n++)
	{
		pReader = &readerList[n];

		str.Format("%d",   n);				   m_ctlReaderList.InsertItem(n, str);
		str.Format("%d",   pReader->ReaderID); m_ctlReaderList.SetItemText(n, 1, str);
		str.Format("%s",   pReader->Ip);       m_ctlReaderList.SetItemText(n, 2, str);
		str.Format("%d",   pReader->IsNomal);  m_ctlReaderList.SetItemText(n, 3, str);
	}

	delete[] readerList;
	return 0;
}

int CDlgRfImportInfo::AfterGetRegionInfo1(RF_INFO_REGION* regionList, int regionNum)
{
	LOG_F("CRfParameterDlg::AfterGetRegionInfo(), regionNum = %d", regionNum);

	m_ctlRegionList.DeleteAllItems();

	CString str;
	CListCtrl& cList = m_ctlRegionList;

	for (int n=0; n<regionNum; n++)
	{
		RF_INFO_REGION& data = regionList[n];

		str.Format("%d/%d", n, data.nIndex); 
		cList.InsertItem(n, str);

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
			{
				const RF_INFO_REGION_POINT_IN* p = &data.stPoint;

				str.Format("%.2f", p->x); cList.SetItemText(n, 2, str);
				str.Format("%.2f", p->y); cList.SetItemText(n, 3, str);

				str.Format("%d", p->uiReaderID); cList.SetItemText(n, 6, str);
			}
			break;
		case 4:
			{
				const RF_INFO_REGION_POINT_IN* p = &data.stPoint;

				str.Format("%.2f", p->x); cList.SetItemText(n, 2, str);
				str.Format("%.2f", p->y); cList.SetItemText(n, 3, str);

				str.Format("%d(T)", p->uiTriggerID); cList.SetItemText(n, 6, str);
			}
			break;
		default:
			break;
		}

	}

	delete[] regionList;
	return 0;
}




