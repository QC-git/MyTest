#include "stdafx.h"
#include "RKGUI_DrawImage.h"


#ifndef WIDTHBYTES
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#endif


void  SetBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
   if ( ! (bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32)) )
   {
	   return;
   }

   BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

   memset( bmih, 0, sizeof(*bmih));
   bmih->biSize = sizeof(BITMAPINFOHEADER);
   bmih->biWidth = width;
   bmih->biHeight = origin ? abs(height) : -abs(height);
   bmih->biPlanes = 1;
   bmih->biBitCount = (unsigned short)bpp;
   bmih->biCompression = BI_RGB;
   //bmih->biSizeImage = WIDTHBYTES(width * bpp) *height;
   if( bpp == 8 )
   {
      RGBQUAD* palette = bmi->bmiColors;
      int i;
      for( i = 0; i < 256; i++ )
      {
         palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
         palette[i].rgbReserved = 0;
      }
   }
}

//yuv ת RGB
typedef   unsigned char     uint8_t;
typedef   ULONGLONG         uint64_t;
void YUV411_TO_RGB24(uint8_t *pYUV_Y, int nLines_Y, uint8_t *pYUV_U,
					 uint8_t *pYUV_V, int nLines_UV, uint8_t *pRGB,
				     int nWidth, int nHeight,int nLineBytes);
//How to use:
//YUV_TO_RGB24(pY,width,pU,pV,width>>1,pRGBBuf,width,(int)0-height,width*3);
static uint64_t mmw_mult_Y    = 0x2568256825682568;
static uint64_t mmw_mult_U_G  = 0xf36ef36ef36ef36e;
static uint64_t mmw_mult_U_B  = 0x40cf40cf40cf40cf;
static uint64_t mmw_mult_V_R  = 0x3343334333433343;
static uint64_t mmw_mult_V_G  = 0xe5e2e5e2e5e2e5e2;

static uint64_t mmb_0x10      = 0x1010101010101010;
static uint64_t mmw_0x0080    = 0x0080008000800080;
static uint64_t mmw_0x00ff    = 0x00ff00ff00ff00ff;

static uint64_t mmw_cut_red   = 0x7c007c007c007c00;
static uint64_t mmw_cut_green = 0x03e003e003e003e0;
static uint64_t mmw_cut_blue  = 0x001f001f001f001f;

/*
============================================================================
 @Description	: YUV411תRGB24
 @param1	: 
 @Return	:
 @Author	: renjie.li
 @Date		: 2009-09-01
 @Version	: V1.0
 @Remark	: 
============================================================================
*/
void YUV411_TO_RGB24(uint8_t *pYUV_Y, int nLines_Y, uint8_t *pYUV_U,
                     uint8_t *pYUV_V, int nLines_UV, uint8_t *pRGB,
                     int nWidth, int nHeight,int nLineBytes) 
{
	int y, horiz_count;
    
	if (nHeight < 0) {
		//we are flipping our output upside-down
		nHeight  = -nHeight;
		pYUV_Y     += (nHeight   - 1) * nLines_Y ;
		pYUV_U     += (nHeight/2 - 1) * nLines_UV;
		pYUV_V     += (nHeight/2 - 1) * nLines_UV;
		nLines_Y  = -nLines_Y;
		nLines_UV = -nLines_UV;
	}

	horiz_count = -(nWidth >> 3);

	for (y=0; y<nHeight; y++) {
		_asm {
			push eax
			push ebx
			push ecx
			push edx
			push edi

			mov eax, pRGB       
			mov ebx, pYUV_Y       
			mov ecx, pYUV_U       
			mov edx, pYUV_V
			mov edi, horiz_count

			horiz_loop:

			movd mm2, [ecx]
			pxor mm7, mm7

			movd mm3, [edx]
			punpcklbw mm2, mm7       

			movq mm0, [ebx]          
			punpcklbw mm3, mm7       

			movq mm1, mmw_0x00ff     

			psubusb mm0, mmb_0x10    

			psubw mm2, mmw_0x0080    
			pand mm1, mm0            

			psubw mm3, mmw_0x0080    
			psllw mm1, 3             

			psrlw mm0, 8             
			psllw mm2, 3             

			pmulhw mm1, mmw_mult_Y   
			psllw mm0, 3             

			psllw mm3, 3             
			movq mm5, mm3            

			pmulhw mm5, mmw_mult_V_R 
			movq mm4, mm2            

			pmulhw mm0, mmw_mult_Y   
			movq mm7, mm1            

			pmulhw mm2, mmw_mult_U_G 
			paddsw mm7, mm5

			pmulhw mm3, mmw_mult_V_G
			packuswb mm7, mm7

			pmulhw mm4, mmw_mult_U_B
			paddsw mm5, mm0      

			packuswb mm5, mm5
			paddsw mm2, mm3          

			movq mm3, mm1            
			movq mm6, mm1            

			paddsw mm3, mm4
			paddsw mm6, mm2

			punpcklbw mm7, mm5
			paddsw mm2, mm0

			packuswb mm6, mm6
			packuswb mm2, mm2

			packuswb mm3, mm3
			paddsw mm4, mm0

			packuswb mm4, mm4
			punpcklbw mm6, mm2

			punpcklbw mm3, mm4

			// 32-bit shuffle.
			pxor mm0, mm0

			movq mm1, mm6
			punpcklbw mm1, mm0

			movq mm0, mm3
			punpcklbw mm0, mm7

			movq mm2, mm0

			punpcklbw mm0, mm1
			punpckhbw mm2, mm1

			// 24-bit shuffle and sav
			movd   [eax], mm0
			psrlq mm0, 32

			movd  3[eax], mm0

			movd  6[eax], mm2


			psrlq mm2, 32            

			movd  9[eax], mm2        

			// 32-bit shuffle.
			pxor mm0, mm0            

			movq mm1, mm6            
			punpckhbw mm1, mm0       

			movq mm0, mm3            
			punpckhbw mm0, mm7       

			movq mm2, mm0            

			punpcklbw mm0, mm1       
			punpckhbw mm2, mm1       

			// 24-bit shuffle and sav
			movd 12[eax], mm0        
			psrlq mm0, 32            

			movd 15[eax], mm0        
			add ebx, 8               

			movd 18[eax], mm2        
			psrlq mm2, 32            

			add ecx, 4               
			add edx, 4               
			
			movd 21[eax], mm2        
			add eax, 24              

			inc edi
			jne horiz_loop

			pop edi
			pop edx
			pop ecx
			pop ebx
			pop eax

			emms
		}

		pYUV_Y   += nLines_Y;
		if (y%2)
		{
			pYUV_U   += nLines_UV;
			pYUV_V   += nLines_UV;
		}
		pRGB += nLineBytes; 
	}
}

CRKDrawImageBase::CRKDrawImageBase()
{
	//m_pBitMapInfo =0;
	m_lpBits =0;
	memset(&m_stBitMapHead, 0, sizeof(m_stBitMapHead));

	m_bLock = FALSE;
}

void CRKDrawImageBase::Destroy()
{
	//m_pBitMapInfo =0;
	memset(&m_stBitMapHead, 0, sizeof(m_stBitMapHead));

	if (m_pBitMapInfo)
	{
		memset(m_pBitMapInfo, 0, sizeof(m_pBitMapInfo));
// 		delete []m_pBitMapInfo;
// 		m_pBitMapInfo = 0;
	}

	if (m_lpBits)
	{
		delete []m_lpBits;
		m_lpBits =0;
	}
	
}

CRKDrawImageBase::~CRKDrawImageBase()
{
   Destroy();
}


bool  CRKDrawImageBase::Create(  int width, int height, int biBitCount, int origin )
{
	bool bRet = 0;
	if ( (width != m_stBitMapHead.biWidth) || ( height != m_stBitMapHead.biHeight) || 
		 (biBitCount != m_stBitMapHead.biBitCount) ) 
	{
		Destroy();
		int nLineBytes = WIDTHBYTES( width * biBitCount);
		int nImageSize = nLineBytes * height;
		BITMAPINFO *bmi = (BITMAPINFO*)m_pBitMapInfo;

		SetBitmapInfo( bmi, width, height, biBitCount, origin );
		memcpy(&m_stBitMapHead, m_pBitMapInfo, sizeof(m_stBitMapHead));
		m_stBitMapHead.biHeight = abs(m_stBitMapHead.biHeight);
		m_stBitMapHead.biSizeImage = nImageSize;

		m_lpBits = new unsigned char[nImageSize+4];
	}

	if (m_lpBits)
	{
		bRet =1;
	}
	
	return bRet;
}

/* Copy Image data */
bool CRKDrawImageBase::CopyImageBits( unsigned char *pBits, int width, int height, int biBitCount)
{
	bool bRet = 0;

	if (m_bLock)
	{
		return false;
	}
	m_bLock = TRUE;

	if (Create(width, height, biBitCount, 0))
	{
		memcpy(m_lpBits, pBits, m_stBitMapHead.biSizeImage);
		bRet =1;
	}


	m_bLock = FALSE;
	return bRet;
}

/* Convert YUV21 data to RGB*/
bool CRKDrawImageBase::CopyYUV21DataToRGB( char *pY, char *pU, char *pV,
										   int stridey, int strideuv, 
										   int width, int height)
{
	bool bRet = 0;

	if (m_bLock)
	{
		return false;
	}
	m_bLock = TRUE;
	
	if (Create(width, height, 24, 0))
	{
		YUV411_TO_RGB24((unsigned char*)pY, stridey, (unsigned char*)pU, (unsigned char*)pV, strideuv,
			(unsigned char*)m_lpBits, width , height,  WIDTHBYTES(width * 24) );

		bRet =1;
	}


	m_bLock = FALSE;
	return bRet;

}

#if defined WIN32 || defined _WIN32
void  CRKDrawImageBase::DrawToHDC( HDC hDCDst, RECT* pDstRect ) 
{
	if (m_bLock)
	{
		return ;
	}
	m_bLock = TRUE;

	if( m_lpBits && pDstRect && hDCDst)
   {
	   if( m_stBitMapHead.biWidth > (pDstRect->right - pDstRect->left)  )
	   {
		   SetStretchBltMode(
			   hDCDst,           // handle to device context
			   HALFTONE );
	   }
	   else
	   {
		   SetStretchBltMode(
			   hDCDst,           // handle to device context
			   COLORONCOLOR );
	   }

	   BITMAPINFO* bmi = (BITMAPINFO*)m_pBitMapInfo;

      ::StretchDIBits(
         hDCDst,
         pDstRect->left, pDstRect->top, (pDstRect->right - pDstRect->left) ,(pDstRect->bottom - pDstRect->top),
         0, 0, m_stBitMapHead.biWidth, m_stBitMapHead.biHeight,
         m_lpBits, bmi, DIB_RGB_COLORS, SRCCOPY );
   }

	m_bLock = FALSE;
}
   
void  CRKDrawImageBase::DrawToHDCByWnd( HWND hPlayWnd)
{
	if (!hPlayWnd)
	{
		return ;
	}

	
	RECT rcShow ={0};
	HWND hWnd = (HWND) hPlayWnd;
	HDC hDC = ::GetDC(hWnd);
	if (NULL == hDC)
	{
		return;
	}

	::GetClientRect(hWnd, &rcShow);
	rcShow.left   += 2;
	rcShow.right  -= 2;
	rcShow.top    += 2;
	rcShow.bottom -= 2;


	if (m_bLock)
	{
		return ;
	}
	m_bLock = TRUE;

	if( m_lpBits && (rcShow.bottom > rcShow.top) )
	{
		if( m_stBitMapHead.biWidth > (rcShow.right - rcShow.left)  )
		{
			SetStretchBltMode(
				hDC,           // handle to device context
				HALFTONE );
		}
		else
		{
			SetStretchBltMode(
				hDC,           // handle to device context
				COLORONCOLOR );
		}

		BITMAPINFO* bmi = (BITMAPINFO*)m_pBitMapInfo;

		::StretchDIBits(
			hDC,
			rcShow.left, rcShow.top, (rcShow.right - rcShow.left) ,(rcShow.bottom - rcShow.top),
			0, 0, m_stBitMapHead.biWidth, m_stBitMapHead.biHeight,
			m_lpBits, bmi, DIB_RGB_COLORS, SRCCOPY );
	}

	::ReleaseDC(hWnd, hDC);

	m_bLock = FALSE;
}


void  CRKDrawImageBase::DrawToHDCByWndEx( HWND hPlayWnd, double szfArea[4])
{
	if (!hPlayWnd)
	{
		return ;
	}

	RECT rcShow ={0};
	HWND hWnd = (HWND) hPlayWnd;
	HDC hDC = ::GetDC(hWnd);
	if (NULL == hDC)
	{
		return;
	}

	::GetClientRect(hWnd, &rcShow);
	rcShow.left   += 2;
	rcShow.right  -= 2;
	rcShow.top    += 2;
	rcShow.bottom -= 2;

	if (m_bLock)
	{
		return ;
	}
	m_bLock = TRUE;

	if( m_lpBits && (rcShow.bottom > rcShow.top) )
	{
		if( m_stBitMapHead.biWidth > (rcShow.right - rcShow.left)  )
		{
			SetStretchBltMode(
				hDC,           // handle to device context
				HALFTONE );
		}
		else
		{
			SetStretchBltMode(
				hDC,           // handle to device context
				COLORONCOLOR );
		}

		BITMAPINFO* bmi = (BITMAPINFO*)m_pBitMapInfo;

		::StretchDIBits(
			hDC,
			rcShow.left, rcShow.top, (rcShow.right - rcShow.left) ,(rcShow.bottom - rcShow.top),
			0, 0, m_stBitMapHead.biWidth, m_stBitMapHead.biHeight,
			m_lpBits, bmi, DIB_RGB_COLORS, SRCCOPY );

		if ( (szfArea[0]>=0 && szfArea[0]<=1) && (szfArea[1]>0 && szfArea[1]<=1) &&
			 (szfArea[2]>=0 && szfArea[2]<=1) && (szfArea[3]>0 && szfArea[3]<=1))
		{
			RECT rcPointArea={0};
			int nWidth = rcShow.right - rcShow.left;
			int nHeight= rcShow.bottom - rcShow.top;

			rcPointArea.left = rcShow.left + (int)(nWidth * szfArea[0]);
			rcPointArea.top = rcShow.top + (int)(nHeight * szfArea[1]);
			rcPointArea.right = rcShow.left + (int)(nWidth * szfArea[2]);
			rcPointArea.bottom = rcShow.left + (int)(nHeight * szfArea[3]);

			CDC dc;
			CDC *pDC =&dc;
			if (pDC->Attach(hDC))
			{
				pDC->Draw3dRect(&rcPointArea, RGB(255 ,0, 0), RGB(255, 0, 0));
				pDC->Detach();
			}

			/*
			CPen penBtnHiLight(PS_DASHDOT, 2, RGB(255, 0, 0)); //White
			CPen*	pOldPen;
			pOldPen = (CPen*)::SelectObject(hDC, &penBtnHiLight);
			MoveToEx(hDC,rcPointArea.left, rcPointArea.top,0);
			LineTo(hDC, rcPointArea.left, rcPointArea.bottom);
			LineTo(hDC, rcPointArea.right, rcPointArea.bottom);
			LineTo(hDC, rcPointArea.right, rcPointArea.top);
			LineTo(hDC, rcPointArea.left, rcPointArea.top);
			::SelectObject(hDC, pOldPen);
			*/


		}
	}

	::ReleaseDC(hWnd, hDC);

	m_bLock = FALSE;
}
#endif