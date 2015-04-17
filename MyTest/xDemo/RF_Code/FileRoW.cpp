#include "StdAfx.h"
#include "FileRoW.h"

//#include "vld.h"

#define ReaderInfo_FilePath          "../../ReaderInfo.dat"
#define RectRegionInfo_FilePath      "../../RectRegionInfo.dat"

int TestSaveFileData()
{
	RF_INFO_READER_IN p;
	p.ID=2;
	strcpy_s(p.IP,"192.168.8.192");
	p.Port=5000;

	CString str;
	str.Format("%d,%s,%d;", p.ID, p.IP, p.Port);

	FILE* pFile=NULL;
	pFile=fopen("C:/Users/RPK-QCY/Desktop/TempFiles/RFdata.txt", "wb");
	if (pFile!=NULL)
	{
		fwrite(str, sizeof(RF_INFO_READER_IN), 1, pFile );

		fwrite(str, sizeof(RF_INFO_READER_IN), 1, pFile );
		fclose(pFile);
		pFile=NULL;
	}

	return 1;
}

int ReadInfo_FromFile(FILE* pFile, PStrCT_M* pHead_Out)   //  Rools:单个条目头为‘@’，结束为‘;’  ,输出链表的头地址
{
	if (pFile==NULL)
	{
		return 0;
	}

	StrCT_M* pHead=NULL;
	StrCT_M* p=NULL;

	char tChar;
	CString str;
	int nCount=0;
	bool bHavInfoSta=false;
	while( fread(&tChar,1,1,pFile) )
	{
		if ( tChar=='@' )
		{
			bHavInfoSta=true;
			nCount++;
			if (pHead==NULL)
			{
				pHead=new StrCT_M;
				p=pHead;
				p->pNext=NULL;
				continue;
			}
			p->pNext=(PVOID)(new StrCT_M);
			p=(StrCT_M*)p->pNext;
			p->pNext=NULL;
			continue;
		}
		if ( bHavInfoSta )
		{
			str+=tChar;
		}
		if ( tChar==';' )
		{
			strcpy_s(p->sInfo, str);
			str.Format("");
			bHavInfoSta=false;
			continue;
		}

	}
	fclose(pFile);
	pFile=NULL;

	*pHead_Out=pHead;
	return nCount;
}

bool ReleaseStrCT(PStrCT_M pHead)
{
	PStrCT_M p=NULL;
	while( pHead )
	{
		p=pHead;
		pHead=(PStrCT_M)(pHead->pNext);
		delete p;
	}
	return TRUE;
}

bool GenerateReaderInfo_In_M_FromStrCT(PStrCT_M pHead, int nCount, RF_INFO_READER_IN* pReaderInfo_Out)
{
	if (pHead==NULL || nCount==0 || pReaderInfo_Out==NULL)
	{
		return FALSE;
	}

	RF_INFO_READER_IN* pInfo=pReaderInfo_Out;
	PStrCT_M p=pHead;
	CString str;
	for(int n=0; n<nCount; n++)
	{
		int nSta=0;
		char* pC=p->sInfo;
		bool bEnd=false;
		bool bExChange=false;
		
		int nTick_Max=100;
		int nTick=nTick_Max;
		while(nTick--)
		{
			if ( (*pC)==';' )
			{
				bEnd=true;
				nSta++;
				bExChange=true;
			}
			else if ( (*pC)==',' || (*pC)=='-' )
			{
				nSta++;
				bExChange=true;
			}
			else if (  ((*pC)>='0' && (*pC<='9')) || (*pC=='.') )
			{
				str+=(*pC);
			}
			pC++;

			if ( bExChange )
			{
				bExChange=false;
				switch(nSta)
				{
					case 1: pInfo[n].ID=atoi(str);       str.Format(""); break;
					case 2: strcpy_s(pInfo[n].IP, str);  str.Format(""); break;
					case 3: pInfo[n].Port=atoi(str);     str.Format(""); break;
					case 4: pInfo[n].x=(float)atof(str); str.Format(""); break;
					case 5: pInfo[n].y=(float)atof(str); str.Format(""); break;
					default:break;
				}
			}
			if ( bEnd )
			{
				bEnd=false;
				break;
			}
		}
		p=(StrCT_M*)(p->pNext);

	}

	return TRUE;
}

int ReadReaderInfo_In_FromFile(RF_INFO_READER_IN* pInfo)
{
	FILE* pFile=NULL;
	pFile=fopen(ReaderInfo_FilePath, "rb");

	PStrCT_M pHead;
	int nCount=ReadInfo_FromFile(pFile, &pHead);

	RF_INFO_READER_IN* pReaderInfo=new RF_INFO_READER_IN[nCount];
	if (pReaderInfo==NULL)
	{
		return FALSE;
	}
	GenerateReaderInfo_In_M_FromStrCT(pHead, nCount, pReaderInfo);
	ReleaseStrCT(pHead);

	for (int n=0; n<nCount; n++)
	{
		pInfo[n]=pReaderInfo[n];
	}

	delete[] pReaderInfo;
	return nCount;
}


bool GenerateRectRegionInfo_In_M_FromStrCT(PStrCT_M pHead, int nCount, RF_INFO_REGION_RECT_IN* pRectRegionInfo_Out)
{
	if (pHead==NULL || nCount==0 || pRectRegionInfo_Out==NULL)
	{
		return FALSE;
	}

	RF_INFO_REGION_RECT_IN* pInfo=pRectRegionInfo_Out;
	PStrCT_M p=pHead;
	CString str;
	for(int n=0; n<nCount; n++)
	{
		int nSta=0;
		char* pC=p->sInfo;
		bool bEnd=false;
		bool bExChange=false;

		int nTick_Max=100;
		int nTick=nTick_Max;
		while(nTick--)
		{
			if ( (*pC)==';' )
			{
				bEnd=true;
				nSta++;
				bExChange=true;
			}
			else if ( (*pC)==',' || (*pC)=='-' )
			{
				nSta++;
				bExChange=true;
			}
			else if (  ((*pC)>='0' && (*pC<='9')) || (*pC=='.') )
			{
				str+=(*pC);
			}
			pC++;

			if ( bExChange )
			{
				bExChange=false;
				switch(nSta)
				{
				case 1:                                             str.Format(""); break;

				case 2:  pInfo[n].TagID[0]=strtoul(str, NULL, 16);  str.Format(""); break;
				case 3:  pInfo[n].TagID[1]=strtoul(str, NULL, 16);  str.Format(""); break;
				case 4:  pInfo[n].TagID[2]=strtoul(str, NULL, 16);  str.Format(""); break;
				case 5:  pInfo[n].TagID[3]=strtoul(str, NULL, 16);  str.Format(""); break;

				case 6:  pInfo[n].ReaID[0]=atoi(str);               str.Format(""); break;
				case 7:  pInfo[n].ReaID[1]=atoi(str);               str.Format(""); break;
				case 8:  pInfo[n].ReaID[2]=atoi(str);               str.Format(""); break;
				case 9:  pInfo[n].ReaID[3]=atoi(str);               str.Format(""); break;

				case 10: pInfo[n].left  =atoi(str);            str.Format(""); break;
				case 11: pInfo[n].top   =atoi(str);            str.Format(""); break;
				case 12: pInfo[n].right =atoi(str);            str.Format(""); break;
				case 13: pInfo[n].bottom=atoi(str);            str.Format(""); break;

				default:break;
				}
			}
			if ( bEnd )
			{
				bEnd=false;
				break;
			}
		}
		p=(StrCT_M*)(p->pNext);

	}

	return TRUE;
}

int ReadRectReginInfo_In_FromFile(RF_INFO_REGION_RECT_IN* pInfo)
{
	FILE* pFile=NULL;
	pFile=fopen(RectRegionInfo_FilePath, "rb");

	PStrCT_M pHead;
	int nCount=ReadInfo_FromFile(pFile, &pHead);

	RF_INFO_REGION_RECT_IN* pRegionInfo=new RF_INFO_REGION_RECT_IN[nCount];
	if (pRegionInfo==NULL)
	{
		return FALSE;
	}
	GenerateRectRegionInfo_In_M_FromStrCT(pHead, nCount, pRegionInfo);
	ReleaseStrCT(pHead);

	for (int n=0; n<nCount; n++)
	{
		pInfo[n]=pRegionInfo[n];
	}

	delete[] pRegionInfo;
	return nCount;
}