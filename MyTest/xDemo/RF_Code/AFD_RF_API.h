#include "StdAfx.h"
//#include "vld.h"

#pragma once

int _stdcall  AFD_RF_Reader_Open(LPCTSTR strCfg);

BOOL _stdcall AFD_RF_Reader_Close(UINT uiCommSN);

void _stdcall AFD_RF_Reader_CloseAll(void);

void _stdcall AFD_RF_Reader_LinkRecover(UINT uiTimeout_ms);

