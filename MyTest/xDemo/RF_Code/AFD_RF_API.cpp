#include "StdAfx.h"
#include "AFD_RF_API.h"

#include "AFD_ReaderComm.h"
//#include "RF_RssiDatBase.h"

#pragma comment(lib,"AFD_ReaderComm.lib")

int _stdcall  AFD_RF_Reader_Open(LPCTSTR strCfg)
{
	return AFD_ReaderComm_Open(strCfg);
}

BOOL _stdcall AFD_RF_Reader_Close(UINT uiCommSN)
{
	return AFD_ReaderComm_Close(uiCommSN);
}

void _stdcall AFD_RF_Reader_CloseAll(void)
{
	AFD_RF_Reader_LinkRecover(0);

	AFD_ReaderComm_CloseAll();
}

void _stdcall AFD_RF_Reader_LinkRecover(UINT uiTimeout_ms)
{
	AFD_ReaderComm_LinkRecover(uiTimeout_ms);
}


