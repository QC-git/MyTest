//DJWStatic.cppʵ���ļ�

#include "stdafx.h"
#include "DJWStatic.h"


BEGIN_MESSAGE_MAP(CDJWStatic, CStatic)
	//{{AFX_MSG_MAP(CDJWStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//**************************************************************************
//
//Function name:  CDJWStatic 
//
//Description: ���캯��
//
//Author:  Time:[2012-12-30]
//
//Parameters: ��
//
//Return Value: ��
//
//Remarks
//
//**************************************************************************
CDJWStatic::CDJWStatic()
{
	m_nTimerID          = 100;
    m_bOverControl      = FALSE;                         
	m_bCheck			= FALSE;
	m_crSideColor		= RGB(255,100,255);			//�߿�
	m_unSideBmpID		= 0;			
	m_unCenterBmpID		= 0;
	m_unCenterSelBmpID  = 0;
	//m_crCheckedColour	= RGB(73,41,62);		//ѡ��������ɫ
	m_crCheckedColour	= RGB(255,255,255);		//ѡ��������ɫ
	m_crHotColour		= RGB(73,41,62);//RGB(92,95,78);		//����ڿؼ���������ɫ
	m_crNormalColour	= RGB(176,114,152);		//��ͨ״̬��������ɫ
}

//**************************************************************************
//
//Function name:  ~CDJWStatic
//
//Description: ��������
//
//Author:  Time:[2012-12-30]
//
//Parameters: ��
//
//Return Value: ��
//
//Remarks
//
//**************************************************************************
CDJWStatic::~CDJWStatic()
{
	m_fntText.DeleteObject();
}

//**************************************************************************
//
//Function name:  DestroyWindow 
//
//Description: �ؼ�������Ϣ
//
//Author:  Time:[2012-12-30]
//
//Parameters: ��
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
BOOL CDJWStatic::DestroyWindow() 
{
    KillTimer(m_nTimerID);
	return CStatic::DestroyWindow();
}


//**************************************************************************
//
//Function name:  PreSubclassWindow 
//
//Description: �ı�ؼ���� �ɽ���֪ͨ��Ϣ
//
//Author:  Time:[2012-12-30]
//
//Parameters: ��
//
//Return Value: ��
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::PreSubclassWindow() 
{
	//����Static�ؼ�������굥����Ϣ
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle|SS_NOTIFY);
    CStatic::PreSubclassWindow();
}

//**************************************************************************
//
//Function name:  OnClicked 
//
//Description: ����������
//
//Author:  Time:[2012-12-30]
//
//Parameters: ��
//
//Return Value: ��
//
//Remarks��
//
//**************************************************************************
//void CDJWStatic::OnClicked()
//{
//	m_bCheck       = TRUE;
//    m_bOverControl = FALSE;
//	Invalidate();//���¿ؼ�
//}

//**************************************************************************
//
//Function name:  CtlColor 
//
//Description: CtlColor
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
HBRUSH CDJWStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

	if (m_bCheck == TRUE)//����ڿؼ����
	{
		pDC->SetTextColor(m_crCheckedColour);
	}else if (m_bOverControl == TRUE)//����ڿؼ���
	{
		pDC->SetTextColor(m_crHotColour);

	}else//��ͨ״̬
	{
		pDC->SetTextColor(m_crNormalColour);
	}
    //͸��
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}


//**************************************************************************
//
//Function name:  OnMouseMove 
//
//Description: ����ڿؼ����ƶ�
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (!m_bOverControl)        
    {
        m_bOverControl = TRUE;
        Invalidate();//���¿ؼ�
        SetTimer(m_nTimerID, 100, NULL);
    }
    CStatic::OnMouseMove(nFlags, point);
}

//**************************************************************************
//
//Function name:  OnTimer 
//
//Description: ��ʱ��
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: ��
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::OnTimer(UINT nIDEvent) 
{
    CPoint p(GetMessagePos());
    ScreenToClient(&p);

    CRect rect;
    GetClientRect(rect);

	//����ڿؼ���
    if (!rect.PtInRect(p))
    {
        m_bOverControl = FALSE;
        KillTimer(m_nTimerID);
        rect.bottom += 10;
        InvalidateRect(rect);
    }
    
	CStatic::OnTimer(nIDEvent);
}

//**************************************************************************
//
//Function name:  OnEraseBkgnd 
//
//Description: �ؼ�����
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
BOOL CDJWStatic::OnEraseBkgnd(CDC* pDC) 
{
	CDC dcMemory;
	CBitmap bmpTemp, *pOldBitmap;
	CRect rect;
	GetClientRect(rect);
	int nWidth  = rect.Width();		//��
	int nHeight = rect.Height();	//��

	if (m_bCheck == TRUE)//����ڿؼ����
	{
		//ˢ�±���
		if (m_unSideBmpID != 0)//��ͼ
		{
			bmpTemp.LoadBitmap(m_unSideBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//��ͼ
		{

			//CRect rcTemp(rect);
			//ClientToScreen(&rcTemp);
			//CPoint pointTemp(rcTemp.left, rcTemp.top);
			//::ScreenToClient(GetParent()->GetSafeHwnd(), &pointTemp);
			//rcTemp.SetRect(pointTemp.x, pointTemp.y, pointTemp.x+rect.Width(), pointTemp.y+rect.Height());
			//GetParent()->InvalidateRect(rcTemp, TRUE);

			dcMemory.CreateCompatibleDC(pDC);
			bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			dcMemory.FillSolidRect(0, 0, nWidth, nHeight, ::GetSysColor(COLOR_BTNFACE));
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = 0;
			pDC->AlphaBlend(0, 0, nWidth, nHeight, &dcMemory, 0, 0, nWidth, nHeight, bf);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();

		}

		//ˢ�¿ؼ�
		CPen pen(PS_SOLID, 1, RGB(255,255,255));
		CBrush* brush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CPen * pOldPen = pDC->SelectObject(&pen);
		CBrush* pOldBrush =pDC->SelectObject(brush);
		pDC->Rectangle(rect);

		if(pOldBrush != NULL)
			pDC->SelectObject(pOldPen);
		if(pOldBrush != NULL)
			pDC->SelectObject(pOldBrush);


		dcMemory.CreateCompatibleDC(pDC);
		bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
		pOldBitmap = dcMemory.SelectObject(&bmpTemp);
		dcMemory.FillSolidRect(0, 0, nWidth, nHeight, m_crSideColor);
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 80;//͸����
		bf.AlphaFormat = 0;
		pDC->AlphaBlend(0, 0, nWidth, nHeight, &dcMemory, 0, 0, nWidth, nHeight, bf);
		dcMemory.SelectObject(pOldBitmap);
		bmpTemp.DeleteObject();
		dcMemory.DeleteDC();


		//����
		if (m_unCenterSelBmpID != 0)
		{
			bmpTemp.LoadBitmap(m_unCenterSelBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/5, nHeight/5);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}
		else//��ͼ
		{
			dcMemory.CreateCompatibleDC(pDC);
			bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/5, nHeight/5);
			dcMemory.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), ::GetSysColor(CTLCOLOR_BTN));
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = 0;
			pDC->AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, rect.left, rect.top, rect.Width(), rect.Height(), bf);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}

	}else if (m_bOverControl == TRUE)//����ڿؼ���
	{

		CPen pen(PS_SOLID, 1, RGB(255,255,255));
		CBrush* brush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CPen * pOldPen = pDC->SelectObject(&pen);
		CBrush* pOldBrush =pDC->SelectObject(brush);
		pDC->Rectangle(rect);

		if(pOldBrush != NULL)
			pDC->SelectObject(pOldPen);
		if(pOldBrush != NULL)
			pDC->SelectObject(pOldBrush);


		//�߿�
		dcMemory.CreateCompatibleDC(pDC);
		bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
		pOldBitmap = dcMemory.SelectObject(&bmpTemp);
		dcMemory.FillSolidRect(0, 0, nWidth, nHeight, m_crSideColor);
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 38;
		bf.AlphaFormat = 0;
		pDC->AlphaBlend(0, 0, nWidth, nHeight, &dcMemory, 0, 0, nWidth, nHeight, bf);
		dcMemory.SelectObject(pOldBitmap);
		bmpTemp.DeleteObject();
		dcMemory.DeleteDC();

		//����
		if (m_unCenterSelBmpID != 0)
		{
			bmpTemp.LoadBitmap(m_unCenterSelBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/5, nHeight/5);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//��ͼ
		{
			dcMemory.CreateCompatibleDC(pDC);
			bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/5, nHeight/5);
			dcMemory.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), ::GetSysColor(CTLCOLOR_BTN));
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = 0;
			pDC->AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, rect.left, rect.top, rect.Width(), rect.Height(), bf);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}


	}else//��ͨ״̬
	{

		//�߿�
		if (m_unSideBmpID != 0)//��ͼ
		{
			bmpTemp.LoadBitmap(m_unSideBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//��ͼ
		{
			dcMemory.CreateCompatibleDC(pDC);
			bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			dcMemory.FillSolidRect(0, 0, nWidth, nHeight, ::GetSysColor(COLOR_BTNFACE));
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = 0;
			pDC->AlphaBlend(0, 0, nWidth, nHeight, &dcMemory, 0, 0, nWidth, nHeight, bf);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();

		}

		//����
		if (m_unCenterBmpID != 0)
		{
			bmpTemp.LoadBitmap(m_unCenterBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/4, nHeight/4);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//��ͼ
		{
			dcMemory.CreateCompatibleDC(pDC);
			bmpTemp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			rect.DeflateRect(nWidth/4, nHeight/4);
			dcMemory.FillSolidRect(rect.left, rect.top, rect.Width(), rect.Height(), ::GetSysColor(CTLCOLOR_BTN));
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = 0;
			pDC->AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, rect.left, rect.top, rect.Width(), rect.Height(), bf);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}

	}
	return TRUE;
}

//**************************************************************************
//
//Function name:  SetCheck 
//
//Description: ���ÿؼ���ѡ״̬
//
//Author:  Time:[2012-12-30]
//
//Parameters: bCheck��TRUE��ѡ��
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::SetCheck(BOOL bCheck)
{
	m_bCheck = bCheck;
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect);
}

//**************************************************************************
//
//Function name:  InitControl 
//
//Description: ��ʼ���ؼ�
//
//Author:  Time:[2012-12-30]
//
//Parameters: unSideBmpID���߿��ͼ, unCenterBmpID������С��ͼ unCenterBmpSelID���Ĵ��ͼ
//
//Return Value: ��
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::InitControl(UINT unSideBmpID, UINT unCenterBmpID, UINT unCenterBmpSelID)
{
	m_unSideBmpID = unSideBmpID;
	m_unCenterBmpID = unCenterBmpID;
	m_unCenterSelBmpID = unCenterBmpSelID;
}

//**************************************************************************
//
//Function name:  SetSideColor 
//
//Description: ���ñ߿���ɫ
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
void CDJWStatic::SetSideColor(COLORREF clSideColor)
{
	m_crSideColor = clSideColor;
	m_crNormalColour = m_crSideColor;
}

//**************************************************************************
//
//Function name:  SetTextFont 
//
//Description: ��������
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
bool CDJWStatic::SetTextFont( const LOGFONT& lgfnt )
{
	m_fntText.DeleteObject();
	m_fntText.CreateFontIndirect( &lgfnt );
	SetFont( &m_fntText, TRUE );

	return true;
}


//**************************************************************************
//
//Function name:  SetTextFont 
//
//Description: ��������
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
bool CDJWStatic::SetTextFont( LONG nHeight, bool bBold, bool bItalic, const CString& sFaceName )
{
	LOGFONT lgfnt;

	lgfnt.lfHeight			= -MulDiv(nHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
	lgfnt.lfWidth			= 0;
	lgfnt.lfEscapement		= 0;
	lgfnt.lfOrientation		= 0;
	lgfnt.lfWeight			= bBold?FW_BOLD:FW_DONTCARE;
	lgfnt.lfItalic			= bItalic?TRUE:FALSE;
	lgfnt.lfUnderline		= FALSE;
	lgfnt.lfStrikeOut		= FALSE;
	lgfnt.lfCharSet			= DEFAULT_CHARSET;
	lgfnt.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lgfnt.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lgfnt.lfQuality			= DEFAULT_QUALITY;
	lgfnt.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;

	strcpy_s( lgfnt.lfFaceName, (char*)(LPCTSTR)sFaceName );
	return SetTextFont( lgfnt );
}


//**************************************************************************
//
//Function name:  ChangeAfterClicked 
//
//Description: ��갴�¿ؼ��ı仯
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks��
//
//**************************************************************************
void  CDJWStatic::ChangeAfterClicked()
{
	m_bCheck       = TRUE;
	m_bOverControl = FALSE;
	Invalidate();//���¿ؼ�
}




