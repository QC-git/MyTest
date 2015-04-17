#include "StdAfx.h"

#include "RF_NetTransmit.h"



/************************************************************************/
/*                        CTCP_ServerToListen_M                         */
/*                               Public                                 */
/************************************************************************/

#define HELLOWORD "welcome %s to RFID"

CTCP_ServerToListen_M::CTCP_ServerToListen_M()
{
	//TRACE("\n CTCP_ServerToListen_M");

	m_pUser_List=new TCP_UserPar_M[MAX_LINK_NUM];
	if ( m_pUser_List==NULL )
	{
		return;
	}
	memset(m_pUser_List, 0, sizeof(TCP_UserPar_M)*MAX_LINK_NUM);

	InitializeCriticalSection(&m_cs);

	m_bIsClassInit=true ;
	TRACE("\n CTCP_ServerToListen_M end");
}

CTCP_ServerToListen_M::~CTCP_ServerToListen_M()
{
	//TRACE("\n ~CTCP_ServerToListen_M");

	if (m_pUser_List)
	{
		delete[] m_pUser_List;
	}

	DeleteCriticalSection(&m_cs);

	TRACE("\n ~CTCP_ServerToListen_M end ");
}

bool CTCP_ServerToListen_M::IsClassInit() const
{
	return m_bIsClassInit;
}

UINT CTCP_ServerToListen_M::Get_AllsockComm(SOCKET* psockComm_out)
{
	if (!IsClassInit())
	{
		return 0;
	}

	UINT uiCount=0;
	EnterCriticalSection(&m_cs);
	for (int n=0; n<MAX_LINK_NUM; n++)
	{
		if ( m_pUser_List[n].IsHavUser && m_pUser_List[n].IsConected )
		{
			psockComm_out[uiCount]=m_pUser_List[n].sockComm;
			uiCount++;
		}
	}
	LeaveCriticalSection(&m_cs);

	return uiCount;
}

bool CTCP_ServerToListen_M::Get_sockCommAddr(SOCKET sockComm, SOCKADDR_IN* pAddr_out)
{
	if (!IsClassInit())
	{
		return FALSE;
	}

	EnterCriticalSection(&m_cs);
	for (int n=0; n<MAX_LINK_NUM; n++)
	{
		if ( m_pUser_List[n].IsHavUser )
		{
			*pAddr_out=m_pUser_List[n].addrClient;
			break;
		}
	}
	LeaveCriticalSection(&m_cs);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

bool CTCP_ServerToListen_M::TStartup()
{
	if (!IsClassInit())
	{
		return FALSE;
	}

	return MSingelThread::TStartup();
}

bool CTCP_ServerToListen_M::TClose()
{
	if (!IsClassInit())
	{
		return FALSE;
	}

	MSingelThread::TClose();
	for (int n=0; n<MAX_LINK_NUM; n++)
	{
		if ( m_pUser_List[n].IsHavUser )
		{
			closesocket(m_pUser_List[n].sockComm);
		}
	}

	return TRUE;
}

/************************************************************************/
/*                              Private                                 */
/************************************************************************/

bool CTCP_ServerToListen_M::brPro()
{
	TRACE("\n CTCP_ServerToListen_M::brPro() \n");

	if (!SocketInit())
	{
		TRACE("\n CTCP_ServerToListen_M: SocketInit  Fail ! \n");
		return FALSE;
	}

	m_sockSrv = socket(AF_INET,SOCK_STREAM,0);  
	m_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	m_addrSrv.sin_family = AF_INET;   
	m_addrSrv.sin_port = htons(6000); 
	bind(m_sockSrv,(SOCKADDR*)&m_addrSrv,sizeof(SOCKADDR));

	if ( listen(m_sockSrv,5)==SOCKET_ERROR )
	{
		TRACE("\n CTCP_ServerToListen_M:error listen on socket ! \n");
		return FALSE;
	}

	return TRUE;
}

void CTCP_ServerToListen_M::roundPro()
{
	UINT uiRoom=GetRoom();
	if (uiRoom)
	{
		TRACE("\n CTCP_ServerToListen_M:Wait for connected \n");

		SOCKET sockConn;
		SOCKADDR_IN addrClient ;
		int len=sizeof(SOCKADDR);

		sockConn = accept(m_sockSrv,(SOCKADDR*)&addrClient,&len );
		TRACE("\n CTCP_ServerToListen_M: accept for %s,%d successed ! \n", inet_ntoa(addrClient.sin_addr), addrClient.sin_port);
		send(sockConn,HELLOWORD,strlen(HELLOWORD)+1,0);
		//recv(sockConn,recvBuf,100,0);
		//closesocket(sockConn); 

		SetUserConnected(uiRoom, sockConn, addrClient);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CTCP_ServerToListen_M::SocketInit()
{

	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 1, 1 );
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{                           
		return FALSE;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

UINT CTCP_ServerToListen_M::GetRoom()
{
	UINT uiRoom=0;

	EnterCriticalSection(&m_cs);
	for (int n=0; n<MAX_LINK_NUM; n++)
	{
		if ( m_pUser_List[n].IsHavUser==false )
		{
			uiRoom = n+1;
		}
	}
	LeaveCriticalSection(&m_cs);

	return uiRoom;
}

bool CTCP_ServerToListen_M::SetUserConnected(UINT uiRoom, SOCKET sockClient, SOCKADDR_IN addrClient)
{
	if (uiRoom)
	{
		uiRoom--;
		EnterCriticalSection(&m_cs);
		m_pUser_List[uiRoom].IsHavUser=true;
		m_pUser_List[uiRoom].IsConected=true;
		m_pUser_List[uiRoom].sockComm=sockClient;
		m_pUser_List[uiRoom].addrClient=addrClient;
		LeaveCriticalSection(&m_cs);

		return TRUE;
	}

	return FALSE;
}
















/************************************************************************/
/*                             CTCP_Client_M                            */
/*                                Public                                */
/************************************************************************/

#define HOST_IP         "127.0.0.1"
#define HOST_PORT             6000

CTCP_Client_M::CTCP_Client_M()
{
	//TRACE("\n CTCP_Client_M ");

	m_precvBuff=new char[ sizeof(RfPacketEx)*500+sizeof(UINT)*2+sizeof(char) ];
	if ( m_precvBuff==0 )
	{
		return;
	}

	m_bIsClassInit=true ;
	//TRACE("\n CTCP_Client_M end ");
}

CTCP_Client_M::~CTCP_Client_M()
{
	//TRACE("\n ~CTCP_Client_M ");

	if (m_precvBuff)
	{
		delete[] m_precvBuff;
	}

	//TRACE("\n ~CTCP_Client_M end ");
}

bool CTCP_Client_M::IsClassInit() const
{
	return m_bIsClassInit;
}

bool CTCP_Client_M::TStartup()
{
	if (!IsClassInit())
	{
		return FALSE;
	}

	return MSingelThread::TStartup();
}

bool CTCP_Client_M::TClose()
{
	if (!IsClassInit())
	{
		return FALSE;
	}

	MSingelThread::TClose();
	closesocket(m_sockComm);

	return TRUE;
}

/************************************************************************/
/*                             Private                                  */
/************************************************************************/

bool CTCP_Client_M::OnRecieved(
	UINT uiReaID, 
	long lTagID,
	UCHAR byRssi,
	UCHAR byPower)
{
	return TRUE;
}

bool CTCP_Client_M::brPro()
{
	TRACE("\n CTCP_Client_M::brPro() \n");

	m_sockComm=socket(AF_INET,SOCK_STREAM,0) ;

	m_addrSrv.sin_addr.S_un.S_addr=inet_addr(HOST_IP);  //获取服务器IP地址,inet_addr()将IP地址转为点分十进制的格式
	m_addrSrv.sin_family = AF_INET;
	m_addrSrv.sin_port = htons(HOST_PORT); 

	return TRUE;
}

void CTCP_Client_M::roundPro()
{
	memset(m_precvBuff, 0, sizeof(RfPacketEx)*500+sizeof(UINT)*2+sizeof(char) );
	int nIndex=recv(m_sockComm, m_precvBuff, sizeof(RfPacketEx)*500+sizeof(UINT)*2+sizeof(char), 0);
	if ( nIndex==SOCKET_ERROR )
	{
		if ( connect(m_sockComm,(SOCKADDR*)&m_addrSrv,sizeof(SOCKADDR))==0 )
		{
			TRACE("\n (CTCP_Client_M) connect with %s,%d  successed ! \n", HOST_IP, HOST_PORT);
		}
	}
	else if ( nIndex>0 )
	{
		if ( strcmp(m_precvBuff, HELLOWORD)==0 )
		{
			TRACE("\n (CTCP_Client_M)：%s \n", m_precvBuff);
		}
		else if ( m_precvBuff[0]='!' )
		{
			UINT uiReaderID=0;
			UINT uiReaderInfo_Num=0;
			RfPacketEx t_RfPack;

			char recvbuff[100];
			memcpy(recvbuff, m_precvBuff, 100);
			memcpy(&uiReaderID, m_precvBuff+sizeof(char), sizeof(UINT));
			memcpy(&uiReaderInfo_Num, m_precvBuff+sizeof(char)+sizeof(UINT), sizeof(UINT));

			char* addrBuff=m_precvBuff+sizeof(char)+sizeof(UINT)*2;
			UINT packLen=sizeof(RfPacketEx);
			for (int n=0; n<(int)uiReaderInfo_Num; n++)
			{		
				memcpy(&t_RfPack, addrBuff+packLen*n, packLen);
				OnRecieved(
					uiReaderID,
					*( (long*)(t_RfPack.pk.Tag_id) ),
					(int)t_RfPack.pk.Rssi,
					(int)t_RfPack.pk.TagPw);
			}

			TRACE("\n (CTCP_Client_M) ReaderID:%d , ReaderInfo_Num:%d \n", uiReaderID, uiReaderInfo_Num);
			TRACE("\n (CTCP_Client_M) lastTagID:%x , Rssi:%d , Power:%d \n",
				*( (long*)(t_RfPack.pk.Tag_id) ),
				(int)t_RfPack.pk.Rssi,
				(int)t_RfPack.pk.TagPw);



		}

	}

	// 	send(sockClient,"client send ",strlen("client send ")+1,0);


}