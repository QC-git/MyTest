
#ifndef _ROPEOK_TRANS_H_
#define _ROPEOK_TRANS_H_


#include <winsock2.h>
#include <windows.h>
#include "GlobalDef.h"


#define NOBLOKING_SOCKET SWITCH_OFF

#define IP_LENGTH  64
#define TRANS_PACKAGE_SIZE 1024
#define TRANS_COMPLETE_BUF 1024*2

#define TRANS_CHECK_CODE_A 0X77BE7DEE// 0111 0111 1011 1110  0111 1101 1110 1110
#define TRANS_CHECK_CODE_B 0X86411486// 1000 0100 0010 0001 0001 0010 0100 1000

enum TRANS_METHOD
{
	TRANS_TCP = 0,
	TRANS_UDP = 1,
};

enum NODE_TYPE
{
	NODE_NORMAL = 0,
	NODE_EMERGENT = 1,
};

enum EXCEPTION_STATE
{
	EX_REMOTE_SHUTDOWN = 10054,
	EX_UNDEF = 0XFFFFFFFF,
};

enum CALLBACK_SWITCH
{
	CALLBACK_DISABLE = 0,	// 回调模式关闭
	CALLBACK_ENABLE  = 1,	// 回调模式开启
};
 
/*
 * Each buffer used to communicate must content 8byte at beginning: int nSize, int nType.
 * The Size item equal hold buffer's length.
 */
typedef struct tagTRANSHEADER
{
	int nSize;
	int nType;
	int nCheckA;
	int nCheckB;
}TRANSHEADER, *PTRANSHEADER;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CallBack Functions. 
 */
typedef VOID (*AcceptProc)(void);

/*
 * CallBack Functions: Process receiving data.
 * void *pUserData: The user translate data.
 * SOCKET sockCon: The socket which communicate with.
 * char *pDataBuf, int nLen: The data you received.
 */
typedef VOID (*ReceiveProc)(void *pUserData, SOCKET sockCon, char *pDataBuf, int nLen);

/*
 * CallBack Functions: Check the emergent data type.
 * void *pUserData: The user translate data.
 * int nType: the type of data packet, refer to NODE_TYPE.
 */
typedef int (*CheckPacketType)(void *pUserData, int nType);

/*
 * CallBack Functions: Return exceptional states.
 * void *pUserData: The user translate data.
 * int nState: the exception state.
 */
typedef int (*ExceptionProc)(void *pUserData, int nState);

/*
 * Description: The Receive thread callback function.
 *              This function can be called by Server point and client point.
 * Author: lutf@20110720
 */
DWORD WINAPI Trans_ReceiveThreadFunc( LPVOID lpParam );

/*
 * Description: Start up socket services before uses network.
 *              This function only be called once in a process.
 * Author: lutf@20110718
 */
int Ropeok_SocketStartup();

/*
 * Description: If needn't use network any more, calls Socket clean up operation.
 *              This function only be called once in a process.
 * Author: lutf@20110718
 */
int Ropeok_SocketCleanup();

#ifdef __cplusplus
};
#endif

class CRopeokTrans;

typedef struct tagPARAMTHD
{
	CRopeokTrans *pServerObj;
	SOCKET        sockCon;
}PARAMTHD;

typedef struct tagTHDINFO
{
	HANDLE hThd;
	BOOLEAN bToExit;
	BOOLEAN bBeenExit;
}THDINFO, *PTHDINFO;



class CRopeokTrans
{
public:
	CRopeokTrans();
	virtual ~CRopeokTrans();

public:
	int Initialize(TRANS_METHOD transMethod, char* pszIPOwner, unsigned short nPortOwner, char* pszIPDest, unsigned short nPortDest, CALLBACK_SWITCH nCallBackSwitch = CALLBACK_DISABLE);		
	int InitBufferQueue(int nQLen, int nNodeSize, int nQLenEm, int nNodeSizeEm);
	int SetAcceptCallBack(AcceptProc pAcceptProc, void *pUserData);
	int SetReceCallBack(ReceiveProc pReceiveProc, void *pUserData);	
	int SetExceptionCallBack(ExceptionProc pExceptionProc, void *pUserData);
	int SetCheckPacketType(CheckPacketType pCheckPacketType, void *pUserData);
	int GetLastError();
	int DoTransData(SOCKET SocketCon, char *pTransData, int nDataLen);	
	virtual int CheckSocketValid(SOCKET hSocket) = 0;
	virtual int CloseTheSocket(SOCKET SocketCon) = 0;
	virtual int EndOfRecvThd(SOCKET SocketCon) = 0;
	
	virtual BOOLEAN GetRecvThdRunCondition(SOCKET SocketCon)
	{		
		return (m_bReceiveThdExit);
	}
	virtual VOID SetRecvThdRunCondition(SOCKET SocketCon, BOOLEAN bNewCondtion)
	{
		m_bReceiveThdExit = bNewCondtion;
	}
	virtual BOOLEAN GetRecvThdState(SOCKET SocketCon)
	{
		return (m_bIsReceiveThdExit);
	}
	virtual VOID SetRecvThdState(SOCKET SocketCon, BOOLEAN bNewState)
	{
		m_bIsReceiveThdExit = bNewState;
	}

	int ReadyForNode(char **ppNodeBuf, int *pnNodeLen, int nNodeType);
	int ReleaseTheNode(char *pNodeBuf, int nNodeType);
	int TakeNode(char* pNodeBuf, int *pnNodeLen, int nNodeType);

private:
	virtual int StartService() = 0;
	int Release();

public:
	SOCKET			m_socketS;
	int					m_nLastError;
	TRANS_METHOD	m_nTransMethod;
	CALLBACK_SWITCH m_nCallBackSwitch;
	unsigned short	m_nportOwner;
	unsigned short  m_nportDest;	
	AcceptProc		m_pfunAcceptProc;
	ReceiveProc		m_pfunReceiveProc;
	CheckPacketType	m_pfunCheckDataType;
	ExceptionProc	m_pfunExceptionProc;
	void*			m_pCbAcceptUserData;
	void*			m_pCbRecvUserData;
	void*			m_pCbCheckUserData;
	void*			m_pCbExceptionUserData;
	void*			m_pQRecvData; // point to CBufferQueue object.
	void*			m_pQRecvEmergent; // point to CBufferQueue object.
	BOOLEAN			m_bAcceptThdExit;
	BOOLEAN			m_bIsAcceptThdExit;
	BOOLEAN			m_bReceiveThdExit;
	BOOLEAN			m_bIsReceiveThdExit;
	BOOLEAN			m_bReleased; // Had been DoRelease.
	BOOLEAN			m_bBeginRelease;
	char			m_szIPOwner[IP_LENGTH+1];	
	char			m_szIPDest[IP_LENGTH+1];
};

/* ================================================================== */
/*
CRopeokTrans: It's a base class of network translate library.
CRopeokTrans be extended by CRopeokClient and CRopeokServer.

  ------------------------------
  int Initialize(TRANS_METHOD transMethod, char* pszIPOwner, unsigned short nPortOwner, char* pszIPDest, unsigned short nPortDest)
  Description:  
	Parameters:
  TRANS_METHOD transMethod: socket trans format( TCP/UDP ), at present, only support TCP connection.
	char* pszIPOwner, unsigned short nPortOwner: The IP and PORT of current point.
	char* pszIPDest, unsigned short nPortDest: The IP and PORT of point which you want to connect to.	
  Return:
	RPKEFAULT\RPKSUCCESS
  Note:
	About the IP and PORT,
	For Client point, only care about: char* pszIPDest, unsigned short nPortDest
	For Server Point, only care about: char* pszIPOwner, unsigned short nPortOwner

  -------------------------------
  int InitBufferQueue(int nQLen, int nNodeSize, int nQLenEm, int nNodeSizeEm);
  Description:
	To create buffer queues for store receiving data.
	There are two buffer queues, one is for normal receiving data buffer, another is for receiving emergent data.
  Parameters:
	int nQLen, int nNodeSize: For normal data, nQLen: the node count, nNodeSize: each node's space.
	int nQLenEm, int nNodeSizeEm: For emergent data.
  Return:
	RPKEFAULT\RPKSUCCESS

  ------------------------------
  int SetAcceptCallBack(AcceptProc pAcceptProc);
	No refer

  ------------------------------
  int SetReceCallBack(ReceiveProc pReceiveProc, void *pUserData);	
  Description:
	After receive a complete data, will call pReceiveProc function to user to process.
  Return:
	RPKEFAULT\RPKSUCCESS

  ------------------------------
  int GetLastError();
  Description:
	If error happen, call GetLastError for detail information. The return error code means need to refer to 

  ------------------------------
  int DoTransData(SOCKET SocketCon, char *pTransData, int nDataLen);
  Description:
	Translate data to destination.
  Parameters:
	SOCKET SocketCon: The socket which you want to send to.
	char *pTransData: The data buffer, can't b NULL.
	int nDataLen:	  The data buffer's length.
  Return:
	RPKEFAULT\RPKSUCCESS
  Note:
	If SocketCon is invalid, function return RPKEFAULT.

  ------------------------------
  virtual int CheckSocketValid(SOCKET hSocket) = 0;
  Description:
	Check current socket is usable.
  Parameters:
	SOCKET hSocket: The socket handle.
  Return:
	RPKEFAULT\RPKSUCCESS

  ------------------------------
  virtual int CloseTheSocket(SOCKET SocketCon) = 0;
  Description:
	Close the special socket handle.
	

  ------------------------------
  virtual int StartService() = 0;
  Description:
	Create socket services. Be implemented by inherited class.

  ------------------------------
  Description:
	The next four functions are created for control threads,needn't to care about them.
	virtual BOOLEAN GetRecvThdRunCondition(SOCKET SocketCon)
	virtual VOID SetRecvThdRunCondition(SOCKET SocketCon, BOOLEAN bNewCondtion)
	virtual BOOLEAN GetRecvThdState(SOCKET SocketCon)
	virtual VOID SetRecvThdState(SOCKET SocketCon, BOOLEAN bNewState)

  ------------------------------
  int Release();
*/




class CTransData
{
public:
	CTransData()
	{
		m_nTotalLen = 0;
		m_nRemainLen = 0;
		m_pDataBuf = NULL;
		m_pCurBuf  = NULL;
		
	}
	~CTransData()
	{
	}
	
	int Initialize(char *pDataBuf, int nDataLen)
	{
		if ((NULL==pDataBuf) || (nDataLen <= 0))
		{
			return RPKEFAULT;
		}
		//
		m_pDataBuf = pDataBuf;
		m_pCurBuf  = pDataBuf;
		m_nTotalLen = nDataLen;
		m_nRemainLen= nDataLen;
		//
		return RPKSUCCESS;
	}
	
	int UpdateProcessed(int nDoneLen)
	{
		if (nDoneLen < 0)
		{
			return RPKEFAULT;
		}
		//		
		m_nRemainLen -= nDoneLen;
		if (m_nRemainLen <= 0)
		{
			m_pCurBuf = NULL;
			m_nRemainLen = 0;
		}
		//
		return RPKSUCCESS;
	}
	
	const char* GetRemainBuf()
	{
		if (m_nRemainLen > 0)
		{
			return (m_pCurBuf);
		}
		else
		{
			return NULL;
		}
	}
	
	int GetRemainLen()
	{
		return m_nRemainLen;
	}
	
	int RemainData(char** ppRemainBuf, int *nRemainLen)
	{
		if (m_nRemainLen <= 0)
			return RPKEFAULT;
		(*ppRemainBuf) = m_pCurBuf;
		(*nRemainLen)  = m_nRemainLen;
		return RPKSUCCESS;
	}
	
private:
	int m_nTotalLen;
	int m_nRemainLen;
	char* m_pCurBuf;
	char* m_pDataBuf;
};




#endif//_ROPEOK_TRANS_H_