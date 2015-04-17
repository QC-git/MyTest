// DlgLogInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "xDemo.h"
#include "DlgLogInfo.h"
#include "afxdialogex.h"

#include "RkUtil.h"

// CDlgLogInfo �Ի���

IMPLEMENT_DYNAMIC(CDlgLogInfo, CDialog)

CDlgLogInfo::CDlgLogInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogInfo::IDD, pParent)
{

}

CDlgLogInfo::~CDlgLogInfo()
{
}

void CDlgLogInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLogInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgLogInfo::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgLogInfo ��Ϣ�������


void CDlgLogInfo::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	const TCHAR* pPath = DBGetLogFilePath();
	LOG_F("CDlgLogInfo::OnBnClickedButton1(), pPath = %s", pPath);

	CStdioFile cFile;
	CString cLine;
	CString cStr;

	cFile.SetFilePath(pPath);
	if ( TRUE == cFile.Open(pPath, 0) )
	{
		CListCtrl& cListCtrl = m_cListCtrl;
		cListCtrl.DeleteAllItems();
		int nCurLine = 0;

		while ( cFile.ReadString(cLine) )
		{
			if ( 0 == cLine.GetLength() )
			{
				continue;
			}

			cStr.Format("%d", nCurLine+1);
			cListCtrl.InsertItem(nCurLine, cStr);

			cListCtrl.SetItemText(nCurLine, 1, cLine);

			nCurLine++;
			if ( nCurLine > 3000 )
			{
				break;
			}
		}
		
	}	

}


BOOL CDlgLogInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	CListCtrl& cListCtrl = m_cListCtrl;

	cListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	cListCtrl.InsertColumn(0, "���",    LVCFMT_LEFT, 50);
	cListCtrl.InsertColumn(1, "����",    LVCFMT_LEFT, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
