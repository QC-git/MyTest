#include "StdAfx.h"
#include "RF_Position_API_v2.h"
#include "RF_Position_API2_v2.h"

#include "RF_Positon.h"


//////////////////////////////////////////////////////////////////////////




RF_CALLBACK_TAG     g_TagInfo_CallBack=NULL;
RF_CALLBACK_READER  g_ReaderInfo_CallBack=NULL;
DWORD g_dwTagInfo_Handle=NULL;
DWORD g_dwReaderInfo_Handle=NULL;


/************************************************************************/
/*                            RF_API                                    */
/************************************************************************/

RF_CReaderManager* g_pRF_Device=NULL;
RF_CAlgManager* g_pRF_ALgManager=NULL;
bool g_bRF_IsInit=false;

//////////////////////////////////////////////////////////////////////////

/*
	func�� RFID ��ʼ�����ڴ�ķ��䣬��Ҫ����ʼ��
	return��true���ɹ�
*/
int __stdcall apiRF_Init()  
{
	TRACE("\n apiRF_Init");

	int nStatus = 0;
	do
	{
		nStatus--;
		if ( NULL != g_pRF_Device || NULL != g_pRF_ALgManager ) break;

		nStatus--;
		g_pRF_Device = RF_CReaderManager::CreateNew(READERMANAGER_NORMALMODE); //���� ���������� �������ݣ�������һ������ش洢����
		g_pRF_ALgManager = new RF_CAlgManager( (HANDLE)g_pRF_Device ); // �㷨�����࣬��ȡ������е��豸�������㣬 ���ص����ݣ���Ҫ���豸
		if ( NULL == g_pRF_Device || NULL == g_pRF_ALgManager ) break;

		g_bRF_IsInit=true;  //��ʼ�� �ɹ� ��־λ
		return 0;
	}while(false);
	
	apiRF_UnInit();
	return nStatus;
}

/*
	func������ʼ���� �ͷ���Դ
	return��true���ɹ�
*/
void __stdcall apiRF_UnInit()  
{
	g_bRF_IsInit=false;

	if (g_pRF_ALgManager)
	{
		delete g_pRF_ALgManager;
		g_pRF_ALgManager=NULL;
	}
	if (g_pRF_Device)
	{
		delete g_pRF_Device;
		g_pRF_Device=NULL;
	}

}

/*
	func������ ��Ҫ������
	return��true���ɹ�
*/
bool __stdcall apiRF_AddReader(const RF_INFO_READER_IN& ReaderInfo)  
{
	if (!g_bRF_IsInit)
	{
		return FALSE;
	}

	return g_pRF_Device->Add_ReaderInfo(
		ReaderInfo.ID,     
		ReaderInfo.IP,
		ReaderInfo.Port,
		ReaderInfo.x,    
		ReaderInfo.y);
}

/*
	func�����������������������������ڣ���
	return����0,�����������id�ţ�0������ʧ��	
*/
UINT __stdcall apiRF_CreateRectRegion(const RF_INFO_REGION_RECT_IN* pAttr)   
{
	if (!g_bRF_IsInit)
	{
		return 0;
	}

	RF_RectRegionAttr_s ttPar;
	ttPar.left=pAttr->left;
	ttPar.top=pAttr->top;
	ttPar.right=pAttr->right;
	ttPar.bottom=pAttr->bottom;

	ttPar.ltReaderID=pAttr->ReaID[3];
	ttPar.lbReaderID=pAttr->ReaID[0];
	ttPar.rtReaderID=pAttr->ReaID[2];
	ttPar.rbReaderID=pAttr->ReaID[1];

	ttPar.ltTagID=pAttr->TagID[3];
	ttPar.lbTagID=pAttr->TagID[0];
	ttPar.rtTagID=pAttr->TagID[2];
	ttPar.rbTagID=pAttr->TagID[1];

	return g_pRF_ALgManager->CreateRectRegion(&ttPar);
}

UINT __stdcall apiRF_CreateMirrorRectRegion(const RF_INFO_REGION_RECT_IN* pAttr)
{
	if (!g_bRF_IsInit)
	{
		return 0;
	}

	RF_RectRegionAttr_s ttPar;
	ttPar.left=pAttr->left;
	ttPar.top=pAttr->top;
	ttPar.right=pAttr->right;
	ttPar.bottom=pAttr->bottom;

	ttPar.ltReaderID=pAttr->ReaID[3];
	ttPar.lbReaderID=pAttr->ReaID[0];
	ttPar.rtReaderID=pAttr->ReaID[2];
	ttPar.rbReaderID=pAttr->ReaID[1];

	ttPar.ltTagID=pAttr->TagID[3];
	ttPar.lbTagID=pAttr->TagID[0];
	ttPar.rtTagID=pAttr->TagID[2];
	ttPar.rbTagID=pAttr->TagID[1];

	return g_pRF_ALgManager->CreateMirrorRectRegion(&ttPar);
}

UINT __stdcall apiRF_CreateLineRegion(const RF_INFO_REGION_LINE_IN& attr)
{
	if (!g_bRF_IsInit)
	{
		return 0;
	}

	RF_LineRegionAttr_s ttPar;

	ttPar.P1.x = attr.P1x;
	ttPar.P1.y = attr.P1y;

	ttPar.P2.x = attr.P2x;
	ttPar.P2.y = attr.P2y;

	ttPar.P1_ReaderID = attr.P1_ReaderID;
	ttPar.P1_TagID = attr.P1_TagID;

	ttPar.P2_ReaderID = attr.P2_ReaderID;
	ttPar.P2_TagID = attr.P2_TagID;


	return g_pRF_ALgManager->CreateLineRegion(ttPar);
}

UINT __stdcall apiRF_CreatePointRegion(int readerId, float pointX, float pointY)
{
	if (!g_bRF_IsInit)
	{
		return 0;
	}

	RF_PointRegionAttr_s ttPar;

	ttPar.readerId = readerId;
	ttPar.readerPoint.x = pointX;
	ttPar.readerPoint.y = pointY;

	return g_pRF_ALgManager->CreatePointRegion(ttPar);
}

int __stdcall apiRF_CreateAntPoint(int antId, float pointX, float pointY)
{
	if ( 0 == antId || NULL == g_pRF_ALgManager )
	{
		return -1;
	}

	RF_Ant125Attr_s attr;
	attr.nAntId = antId;
	attr.antPonit.x = pointX;
	attr.antPonit.y = pointY;
	return g_pRF_ALgManager->CreateAnt(antId, attr);
}

/*
	func�����ûص�ʱ�����ص��û�����
	return���ɹ�����0
*/
int __stdcall apiSetUserHandle(DWORD hUser)
{
	if ( NULL == g_pRF_ALgManager || NULL == g_pRF_Device )
	{
		return -1;
	}

	g_pRF_Device->SetUserHandle(hUser);

	return g_pRF_ALgManager->SetUserHandle(hUser);
}

/*
	func�����ûص�
	return���ɹ�����0
*/
int __stdcall apiSetSingleTagCallBack(RF_CALLBACK_TAG_SINGLE cb)
{
	if ( NULL == g_pRF_ALgManager )
	{
		return -1;
	}

	return g_pRF_ALgManager->SetSingleTagInfoCallBack(cb);
}

/*
	func�����ûص�
	return���ɹ�����0
*/
int __stdcall apiSetTagCallBack(RF_CALLBACK_TAG cb)
{
	if ( NULL == g_pRF_ALgManager )
	{
		return -1;
	}

	g_pRF_ALgManager->SetAllTagInfoCallBack(cb);
	return 0;
}

int __stdcall apiSetReaderCallBack(RF_CALLBACK_READER cb)
{
	if ( NULL == g_pRF_Device )
	{
		return -1;
	}

	g_pRF_Device->SetAllReaderInfoCallBack(cb);
	return 0;
}

/*
	func�������ڲ����߳�
	return��	
*/
void __stdcall apiRF_TStartup()
{
	if (!g_bRF_IsInit)
	{
		return ;
	}

	g_pRF_Device->StartThread();

	g_pRF_ALgManager->Startup_TRfAlgorithm();

}

/*
	func���ر��ڲ����߳�
	return��	
*/
void __stdcall apiRF_TClose()
{
	if (!g_bRF_IsInit)
	{
		return ;
	}

	g_pRF_ALgManager->Close_TRfAlgorithm();

	g_pRF_Device->CloseThread();
}

void __stdcall apiRF_Clear()
{
	if (!g_bRF_IsInit)
	{
		return ;
	}

	g_pRF_ALgManager->Del_AllRegion();
	g_pRF_Device->ClearAll();
}

/*
	func��
	return��	
*/
int  apiGetTagInfo(PVOID pTagInfoBuff, PVOID pTagInfoOutput)
{

	if (g_TagInfo_CallBack!=NULL)
	{
		(*g_TagInfo_CallBack)(NULL, 0, g_dwTagInfo_Handle);
	}

	return 0;
}

int  apiGetReaderInfo(PVOID pReaderInfoBuff, PVOID pReaderInfoOutput)
{

	if (g_ReaderInfo_CallBack!=NULL)
	{
		(*g_ReaderInfo_CallBack)(NULL, 0, g_dwReaderInfo_Handle);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////

PVOID __stdcall sapiGetRegionDBMaddr()
{
	return g_pRF_ALgManager;
}


/////////////////////////////////////////////////////////////////////////

int __stdcall apiGetReadersInfo(RF_CALLBACK_READER pAfterGetFunc, DWORD hUser)
{
	if ( !g_bRF_IsInit || NULL == pAfterGetFunc )
	{
		return -1;
	}

	return g_pRF_Device->GetReadersInfo(pAfterGetFunc, hUser);
}

int __stdcall apiGetRegionsInfo(RF_GET_REGION_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	if ( !g_bRF_IsInit || NULL == pAfterGetFunc )
	{
		return -1;
	}

	return g_pRF_ALgManager->GetRegionsInfo(pAfterGetFunc, hUser);
}

int __stdcall apiSetRssiInfoCallBack(RF_GET_RSSI_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	if ( !g_bRF_IsInit )
	{
		return -1;
	}

	return g_pRF_Device->SetRssiInfoCallBack(pAfterGetFunc, hUser);
}

int __stdcall apiGetRegionsVssInfo(UINT uiRegionId, RF_GET_VSS_INFO_FUNC pAfterGetFunc, DWORD hUser)
{
	if ( !g_bRF_IsInit )
	{
		return -1;
	}

	return g_pRF_ALgManager->GetRegionsVssInfo(uiRegionId, pAfterGetFunc, hUser);
}


