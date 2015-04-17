#pragma once
#ifndef __RKGUI_DREAIMAGE_H_
#define __RKGUI_DREAIMAGE_H_
#include <Windows.h>



/* CRKDrawImageBase class definition */
class  CRKDrawImageBase
{
public:
   CRKDrawImageBase();
   virtual ~CRKDrawImageBase();
   /* Create image (BGR or grayscale) */
   virtual bool  Create( int width, int height, int biBitCount, int origin=1);

   /* Copy Image data */
   virtual bool CopyImageBits( unsigned char *pBits, int width, int height, int biBitCount);

   /* Convert YUV21 data to RGB*/
   virtual bool CopyYUV21DataToRGB( char *pY, char *pU, char *pV, 
	                                int stridey, int strideuv, 
									int width, int height);



   /* Destroy data */
   virtual void  Destroy(void);

   /* Get image Information */
   int Width(){return m_stBitMapHead.biWidth;};
   int Height() { return m_stBitMapHead.biHeight;};
   int BitCount(){return m_stBitMapHead.biBitCount;};
   /*virtual bool  Save( const char* filename );*/
   unsigned char* GetImageData() { return m_lpBits; };


#if defined WIN32 || defined _WIN32
   /* draw the current image ROI to the specified rectangle of the destination DC */
   virtual void  DrawToHDC( HDC hDCDst, RECT* pDstRect);
   virtual void  DrawToHDCByWnd( HWND hPlayWnd);
   virtual void  DrawToHDCByWndEx( HWND hPlayWnd, double szfArea[4]);
#endif

protected:
   BITMAPINFOHEADER m_stBitMapHead;
   unsigned char*	  m_lpBits;	// Pointer to actual bitmap bits
   unsigned char m_pBitMapInfo[sizeof(BITMAPINFOHEADER)+1024];

   BOOL m_bLock;
};

typedef CRKDrawImageBase CRKDrawBase;
#endif                                         