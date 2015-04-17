// RfAlgorihmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgRfAlgInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"


// CRfAlgorihmDlg 对话框

IMPLEMENT_DYNAMIC(CDlgRfAlgInfo, CDialog)

CDlgRfAlgInfo::CDlgRfAlgInfo(RfService* pRfService, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRfAlgInfo::IDD, pParent)
	, m_pRfService(pRfService)
	, m_edtRegionId(0)
{

}

CDlgRfAlgInfo::~CDlgRfAlgInfo()
{
}

void CDlgRfAlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cListControl);
	DDX_Text(pDX, IDC_EDIT1, m_edtRegionId);
	DDV_MinMaxUInt(pDX, m_edtRegionId, 1, 100);
	DDX_Control(pDX, IDC_COMBO1, m_cComboBox);
}

BOOL CDlgRfAlgInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CListCtrl& cListCtrl = m_cListControl;
	CString str;

	cListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	cListCtrl.InsertColumn(0,str,LVCFMT_LEFT,50);
	for(int n=0; n<=30 ; n++)
	{
		str.Format("%d",n);
		cListCtrl.InsertColumn(n+1,str,LVCFMT_LEFT,50);
		//str.Format("%d",30-n);
		//cListCtrl.InsertItem(n,str);
	}
	cListCtrl.SetTextColor(RGB(128,128,255));

	m_cComboBox.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CDlgRfAlgInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgRfAlgInfo::OnBnClickedButton1)
END_MESSAGE_MAP()

// CRfAlgorihmDlg 消息处理程序


void CDlgRfAlgInfo::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码\

	if ( false == UpdateData(TRUE) ) return;

	UINT uiRegionIndexId = m_edtRegionId;

	LOG_F("\n CRfAlgorihmDlg::OnBnClickedButton1(), uiRegionIndexId = %d", uiRegionIndexId);

	int nRet = apiGetRegionsVssInfo(uiRegionIndexId, CDlgRfAlgInfo::AfterGetVssInfo, (DWORD)this);

	if ( 0 == nRet )
	{
		MY_MSGBOX_F("获取区域VSS数据 成功");
	}
	else
	{
		CString str;
		str.Format("获取区域VSS数据 失败, 错误码(%d)", nRet);
		MY_MSGBOX_F(str);
	}

}


int CDlgRfAlgInfo::AfterGetVssInfo2(UINT uiRegionId, RF_REGION_STYLE emStyle, float* pVss, int nVssLen, float fMaxValue, float fMinValue)
{
	UINT uiReaderPoint = m_cComboBox.GetCurSel();

	LOG_F("\n CRfAlgorihmDlg::AfterGetVssInfo2(), uiReaderPoint = %d, uiRegionId = %d, emStyle = %d",
		uiReaderPoint, uiRegionId, emStyle);

	CListCtrl& cListCtrl = m_cListControl;
	cListCtrl.DeleteAllItems();

	

	CString str;
	switch(emStyle)
	{
	case RF_REGION_STYLE_RECT:
	case RF_REGION_STYLE_RECT_MIRROR:
		{
			if ( uiReaderPoint > 3 ) break;

			for(int n=0; n<=30 ; n++)
			{
				str.Format("%d",30-n);
				cListCtrl.InsertItem(n,str);
			}

			int nMul  = 31;
			int nMul2 = 31*31;
			int nMul3 = nMul2*uiReaderPoint;
			for (int j=30; j>=0; j--){
				for (int i=0; i<=30; i++){
					if ( j<5 || j> 25 || i<5 || i>25 )
					{
						str.Format("~%.2f",pVss[nMul3+j*nMul+i]);
						cListCtrl.SetItemText(30-j,i+1,str);
					}
					else
					{
						str.Format("[%.2f]",pVss[nMul3+j*nMul+i]);
						cListCtrl.SetItemText(30-j,i+1,str);
					}
					
					//cListCtrl.
				}
			}


		}break;
	case RF_REGION_STYLE_LINE:
		{
			if ( uiReaderPoint > 1 ) break;

			str.Format("0");
			cListCtrl.InsertItem(0,str);

			int nMul  = 31;
			int nMul3 = nMul*uiReaderPoint;
			for (int i=0; i<=30; i++){
				if ( i<5 || i>25 )
				{
					str.Format("~%.2f",pVss[nMul3+i]);
					cListCtrl.SetItemText(0,i+1,str);
				}
				else
				{
					str.Format("*%.2f",pVss[nMul3+i]);
					cListCtrl.SetItemText(0,i+1,str);
				}
				
				//cListCtrl.
			}


		}break;
	default:break;
	}

	//

	return 0;
}



