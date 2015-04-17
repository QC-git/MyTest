//DJWStatic.cpp实现文件

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
//Description: 构造函数
//
//Author:  Time:[2012-12-30]
//
//Parameters: 无
//
//Return Value: 无
//
//Remarks
//
//**************************************************************************
CDJWStatic::CDJWStatic()
{
	m_nTimerID          = 100;
    m_bOverControl      = FALSE;                         
	m_bCheck			= FALSE;
	m_crSideColor		= RGB(255,100,255);			//边框
	m_unSideBmpID		= 0;			
	m_unCenterBmpID		= 0;
	m_unCenterSelBmpID  = 0;
	//m_crCheckedColour	= RGB(73,41,62);		//选中字体颜色
	m_crCheckedColour	= RGB(255,255,255);		//选中字体颜色
	m_crHotColour		= RGB(73,41,62);//RGB(92,95,78);		//鼠标在控件上字体颜色
	m_crNormalColour	= RGB(176,114,152);		//普通状态下字体颜色
}

//**************************************************************************
//
//Function name:  ~CDJWStatic
//
//Description: 析构函数
//
//Author:  Time:[2012-12-30]
//
//Parameters: 无
//
//Return Value: 无
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
//Description: 控件销毁消息
//
//Author:  Time:[2012-12-30]
//
//Parameters: 无
//
//Return Value: 
//
//Remarks：
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
//Description: 改变控件风格 可接收通知消息
//
//Author:  Time:[2012-12-30]
//
//Parameters: 无
//
//Return Value: 无
//
//Remarks：
//
//**************************************************************************
void CDJWStatic::PreSubclassWindow() 
{
	//更改Static控件接受鼠标单击消息
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle|SS_NOTIFY);
    CStatic::PreSubclassWindow();
}

//**************************************************************************
//
//Function name:  OnClicked 
//
//Description: 鼠标左键按下
//
//Author:  Time:[2012-12-30]
//
//Parameters: 无
//
//Return Value: 无
//
//Remarks：
//
//**************************************************************************
//void CDJWStatic::OnClicked()
//{
//	m_bCheck       = TRUE;
//    m_bOverControl = FALSE;
//	Invalidate();//更新控件
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
//Remarks：
//
//**************************************************************************
HBRUSH CDJWStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

	if (m_bCheck == TRUE)//鼠标在控件点击
	{
		pDC->SetTextColor(m_crCheckedColour);
	}else if (m_bOverControl == TRUE)//鼠标在控件上
	{
		pDC->SetTextColor(m_crHotColour);

	}else//普通状态
	{
		pDC->SetTextColor(m_crNormalColour);
	}
    //透明
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}


//**************************************************************************
//
//Function name:  OnMouseMove 
//
//Description: 鼠标在控件里移动
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
//
//**************************************************************************
void CDJWStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (!m_bOverControl)        
    {
        m_bOverControl = TRUE;
        Invalidate();//更新控件
        SetTimer(m_nTimerID, 100, NULL);
    }
    CStatic::OnMouseMove(nFlags, point);
}

//**************************************************************************
//
//Function name:  OnTimer 
//
//Description: 计时器
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 无
//
//Remarks：
//
//**************************************************************************
void CDJWStatic::OnTimer(UINT nIDEvent) 
{
    CPoint p(GetMessagePos());
    ScreenToClient(&p);

    CRect rect;
    GetClientRect(rect);

	//鼠标在控件里
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
//Description: 控件背景
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
//
//**************************************************************************
BOOL CDJWStatic::OnEraseBkgnd(CDC* pDC) 
{
	CDC dcMemory;
	CBitmap bmpTemp, *pOldBitmap;
	CRect rect;
	GetClientRect(rect);
	int nWidth  = rect.Width();		//宽
	int nHeight = rect.Height();	//高

	if (m_bCheck == TRUE)//鼠标在控件点击
	{
		//刷新背景
		if (m_unSideBmpID != 0)//有图
		{
			bmpTemp.LoadBitmap(m_unSideBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//无图
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

		//刷新控件
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
		bf.SourceConstantAlpha = 80;//透明度
		bf.AlphaFormat = 0;
		pDC->AlphaBlend(0, 0, nWidth, nHeight, &dcMemory, 0, 0, nWidth, nHeight, bf);
		dcMemory.SelectObject(pOldBitmap);
		bmpTemp.DeleteObject();
		dcMemory.DeleteDC();


		//中心
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
		else//无图
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

	}else if (m_bOverControl == TRUE)//鼠标在控件上
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


		//边框
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

		//中心
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
		}else//无图
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


	}else//普通状态
	{

		//边框
		if (m_unSideBmpID != 0)//有图
		{
			bmpTemp.LoadBitmap(m_unSideBmpID);
			dcMemory.CreateCompatibleDC(pDC);
			pOldBitmap = dcMemory.SelectObject(&bmpTemp);
			pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, SRCCOPY);
			dcMemory.SelectObject(pOldBitmap);
			bmpTemp.DeleteObject();
			dcMemory.DeleteDC();
		}else//无图
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

		//中心
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
		}else//无图
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
//Description: 设置控件被选状态
//
//Author:  Time:[2012-12-30]
//
//Parameters: bCheck：TRUE被选中
//
//Return Value: 
//
//Remarks：
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
//Description: 初始化控件
//
//Author:  Time:[2012-12-30]
//
//Parameters: unSideBmpID：边框底图, unCenterBmpID：中心小底图 unCenterBmpSelID中心大底图
//
//Return Value: 无
//
//Remarks：
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
//Description: 设置边框颜色
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
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
//Description: 设置字体
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
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
//Description: 设置字体
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
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
//Description: 鼠标按下控件的变化
//
//Author:  Time:[2012-12-30]
//
//Parameters: 
//
//Return Value: 
//
//Remarks：
//
//**************************************************************************
void  CDJWStatic::ChangeAfterClicked()
{
	m_bCheck       = TRUE;
	m_bOverControl = FALSE;
	Invalidate();//更新控件
}




