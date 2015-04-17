// ZuoBiaoInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgRfExportInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"

// CZuoBiaoInfoDlg 对话框

IMPLEMENT_DYNAMIC(CDlgRfExportInfo, CDialog)

CDlgRfExportInfo::CDlgRfExportInfo(RfService* pRfService, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRfExportInfo::IDD, pParent)
	, m_bAutoShow(FALSE)
	, m_pRfService(pRfService)
	, m_LastListUpdataTime(0)
{

	m_cTagInfoMap.Clear();
	m_cTagIdSet.Clear();

	m_pRfService->SetTagInfoCallBack(CDlgRfExportInfo::AfterGetTagInfo, (DWORD)this);

	
}

CDlgRfExportInfo::~CDlgRfExportInfo()
{
	int nTagNum = m_cTagInfoMap.Size();
	ExportTagInfoMap_T::Iter iter(m_cTagInfoMap);

	long lKey;
	RF_INFO_TAG* pTagInfo;
	for (int n=0; n<nTagNum; n++)
	{
		if ( false == iter.Next(lKey, pTagInfo) )
		{
			LOGERR_F("CZuoBiaoInfoDlg::~CZuoBiaoInfoDlg(), false == iter.Next(lKey, pTagInfo)");
			break;
		}

		delete pTagInfo;
	}

}

void CDlgRfExportInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bAutoShow);
	DDX_Control(pDX, IDC_LIST1, m_cListContrl);
}

BOOL CDlgRfExportInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CListCtrl& cListCtrl = m_cListContrl;

	cListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	cListCtrl.InsertColumn(0, "标签ID", LVCFMT_LEFT,80);
	cListCtrl.InsertColumn(1, "Alive",  LVCFMT_LEFT,50);
	cListCtrl.InsertColumn(2, "SSR",    LVCFMT_LEFT,50);  //largest signal reciever
	cListCtrl.InsertColumn(3, "SSRssi", LVCFMT_LEFT,50); 
	cListCtrl.InsertColumn(4, "DaTime", LVCFMT_LEFT,100); 
	cListCtrl.InsertColumn(5, "AlTime", LVCFMT_LEFT,100); 

	cListCtrl.InsertColumn(6, "RR",       LVCFMT_LEFT,50);    //Recent regional
	cListCtrl.InsertColumn(7, "RA",       LVCFMT_LEFT,50);
	cListCtrl.InsertColumn(8, "世界坐标", LVCFMT_LEFT,100);

	cListCtrl.Update(0);

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgRfExportInfo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( WM_REQUEST_LIST_UPDATA == pMsg->message )
	{
		UpdataTagList();
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CDlgRfExportInfo, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgRfExportInfo::OnBnClickedCheck1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CZuoBiaoInfoDlg 消息处理程序


void CDlgRfExportInfo::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CListCtrl& cListCtrl = m_cListContrl;

	if (m_bAutoShow)
	{
		cListCtrl.SetRedraw(TRUE);
	}
	else
	{
		cListCtrl.SetRedraw(FALSE);
	}

}

int CDlgRfExportInfo::AfterGetTagInfo2(RF_INFO_TAG* tagList, int tagListNum)
{
	//LOG_F("\n CDlgRfExportInfo::AfterGetTagInfo2(), tagListNum = %d", tagListNum);

	RF_INFO_TAG* pTag;
	RF_INFO_TAG* pOldTag;

	m_lock.Lock();
	for (int n=0; n<tagListNum; n++)
	{
// 		pTag = new RF_INFO_TAG;
// 		if ( NULL == pTag ) 
// 		{
// 			LOGW_F("CDlgRfExportInfo::AfterGetTagInfo2(), NULL == pTag");
// 			break;
// 		}
// 		memcpy(pTag, &tagList[n], sizeof(RF_INFO_TAG));

		m_cTagIdSet.Add(tagList[n].lTagID);
// 		if ( true == m_TagInfoMap.Find(pTag->lTagID, pOldTag) )
// 		{
// 			delete pOldTag;
// 		}
// 		if ( false == m_TagInfoMap.Add(pTag->lTagID, pTag) )
// 		{
// 			LOGW_F("CDlgRfExportInfo::AfterGetTagInfo2(), false == m_TagInfoMap.Add()");
// 			delete pTag;
// 			break;
// 		}
	}

	RfExportTagFilter* pFilter = m_pRfService->GetTagFilter();
	if ( pFilter )
	{
		pFilter->Filter(tagList, tagListNum, m_cTagInfoMap);
	}

	m_lock.UnLock();

	
	ASSERT(m_pRfService);
	if ( m_pRfService )
	{
		// 发送
		RfTransport* pTrans = m_pRfService->GetTransport();
		if (pTrans)
		{
			pTrans->InsertTagInfoToSendQueueByXml(tagList, tagListNum);
		}

		// 更新数据库
		RfDataBaseWriter* pWriter = m_pRfService->GetDbWriter();
		if (pWriter)
		{
			pWriter->UpdataTagInfoToDb(tagList, tagListNum, m_cTagInfoMap);
		}

	}

	long lCurTime = time(NULL);
	if ( lCurTime != m_LastListUpdataTime )
	{
		m_LastListUpdataTime = lCurTime;

		// 发消息给对话框以更新列表
		::PostMessage(m_hWnd, WM_REQUEST_LIST_UPDATA, 0, 0);  // 发送消息给窗口用于更新列表

	}

	return 0;
}


void CDlgRfExportInfo::UpdataTagList()
{
	

	CListCtrl& cListCtrl = m_cListContrl;

	//int nPos = cListCtrl.GetScrollPos(0);
	//TRACE("\n nPos = %d", nPos);

	//cListCtrl.DeleteAllItems(); 

	m_lock.Lock();

	int nItemCount = cListCtrl.GetItemCount();
	ExportTagIdSet_T::Iter iter(m_cTagIdSet);
	int nTagNum = m_cTagIdSet.Size();
	CString str;

	if ( nItemCount < nTagNum )
	{
		int nAddCnt = nTagNum - nItemCount;
		for (int n=0; n<nAddCnt; n++)
		{
			str.Format("");
			cListCtrl.InsertItem(0, str);
		}
	}


	long lTagId;
	RF_INFO_TAG* pTagInfo;
	for (int n=0; n<nTagNum; n++)
	{
		if ( false == iter.Next(lTagId) )
		{
			LOGERR_F("CDlgRfExportInfo::UpdataTagList(), false == iter.Next()");
			break;
		}

		if ( false == m_cTagInfoMap.Find(lTagId, &pTagInfo) || NULL == pTagInfo )
		{
			LOGERR_F("CDlgRfExportInfo::UpdataTagList(), false == m_TagInfoMap.Find() || NULL == pTagInfo ");
			break;
		}

		str.Format("%x", pTagInfo->lTagID);
		cListCtrl.SetItemText(n, 0, str);

		str.Format("%d", pTagInfo->bAlive);
		cListCtrl.SetItemText(n,1,str);

		str.Format("%d", pTagInfo->uiLocateReader); 
		cListCtrl.SetItemText(n,2,str);

		str.Format("%.2f",pTagInfo->fLocateRssi);
		cListCtrl.SetItemText(n,3,str);

		CTime cTime(pTagInfo->lTime);
		str.Format("%d",cTime.GetMinute());
		cListCtrl.SetItemText(n,4,str);

		//CTime tTime2=p->AlgTime;
		str = cTime.Format("%H:%M:%S");
		cListCtrl.SetItemText(n,5,str);

		str.Format("%d", pTagInfo->nLocateRegion);
		cListCtrl.SetItemText(n,6,str);

		str.Format("%d", pTagInfo->nLocateAnt);
		cListCtrl.SetItemText(n,7,str);

		str.Format("%.2f, %.2f", pTagInfo->fWorld_X, pTagInfo->fWorld_Y);
		cListCtrl.SetItemText(n,8,str);

	}

	//cListCtrl.SetScrollPos(0, nPos);

	m_lock.UnLock();
	
	//cListCtrl.SetRedraw(TRUE);
	//cListCtrl.Update(0);
}


int CDlgRfExportInfo::AfterGetReaderInfo1(RF_INFO_READER* readerList, int readerListNum)
{
	// 预处理数据

	// 更新至数据库
	RfDataBaseWriter* pWriter = m_pRfService->GetDbWriter();
	if (pWriter)
	{
		pWriter->UpdataReaderInfoToDb(readerList, readerListNum, m_cReaderMap);
	}

	return 0;
}


void CDlgRfExportInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ( 1 == nIDEvent )
	{
		ASSERT(m_pRfService);
		if ( m_pRfService )
		{
			if ( true == m_pRfService->IsStart() )
			{
				int nReaderNum = apiGetReadersInfo(CDlgRfExportInfo::AfterGetReaderInfo, (DWORD)this);
			}

		}
	}

	CDialog::OnTimer(nIDEvent);
}
