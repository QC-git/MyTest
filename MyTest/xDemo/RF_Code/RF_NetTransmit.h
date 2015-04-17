#pragma once

#include "RF_Constant.h"
#include "AFD_ReaderComm.h"
#include "AFD_RF_API.h"

struct TCP_UserPar_M
{
	bool IsHavUser;
	bool IsConected;

	SOCKET sockComm;
	SOCKADDR_IN addrClient;
};

/*     专门监听 连接请求     */
class CTCP_ServerToListen_M:MSingelThread
{
public:
	CTCP_ServerToListen_M();
	~CTCP_ServerToListen_M();
	bool IsClassInit() const;

	static const int MAX_LINK_NUM=5;

	UINT Get_AllsockComm(SOCKET* psockComm_out);  // return count
	bool Get_sockCommAddr(SOCKET sockComm, SOCKADDR_IN* pAddr_out);
	bool Set_sockCommUnconected(SOCKET sockComm);

	//------------------

	bool TStartup();
	bool TClose();

private:
	bool m_bIsClassInit;

	TCP_UserPar_M* m_pUser_List;
	CRITICAL_SECTION m_cs;

	SOCKET m_sockSrv;
	SOCKADDR_IN m_addrSrv; 

	bool SocketInit();

	UINT GetRoom();
	bool SetUserConnected(UINT uiRoom, SOCKET sockClient, SOCKADDR_IN addrClient);

	/*------- 函数重载---------*/

	bool brPro();
	void roundPro();

};

//////////////////////////////////////////////////////////////////////////

class CTCP_Client_M:MSingelThread
{
public:
	CTCP_Client_M();
	~CTCP_Client_M();
	bool IsClassInit() const;

	bool TStartup();
	bool TClose();

private:
	bool m_bIsClassInit;

	SOCKET m_sockComm;
	SOCKADDR_IN m_addrSrv;

	char* m_precvBuff;

	virtual bool OnRecieved(
		UINT uiReaID, 
		long lTagID,
		UCHAR byRssi,
		UCHAR byPower);

	/*------- 虚函数重写---------*/

	bool brPro();
	void roundPro();


};