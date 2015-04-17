#include "StdAfx.h"

#if !defined FILEROW
#define FILEROW

#include "RF_Position_API_v2.h"

typedef struct StrChainTable_Model
{
	char sInfo[100];
	PVOID pNext;
}StrCT_M,*PStrCT_M;

int TestSaveFileData();

int ReadReaderInfo_In_FromFile(RF_INFO_READER_IN* pInfo);

int ReadRectReginInfo_In_FromFile(RF_INFO_REGION_RECT_IN* pInfo);

//////////////////////////////////////////////////////////////////////////
int ReadInfo_FromFile(FILE* pFile, PStrCT_M* pHead_Out);

bool ReleaseStrCT(PStrCT_M pHead);

#endif