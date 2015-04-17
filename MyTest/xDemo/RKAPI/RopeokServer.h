
#pragma warning(disable:4786)     //   Disable   warning   messages

#include <map>
#include <vector>
#include <utility>

#include "GlobalDef.h"
#include "RopeokTrans.h"

using namespace std;


#ifndef _ROPEOKSERVER_H_
#define _ROPEOKSERVER_H_

// Server Part
class CRopeokServer : public CRopeokTrans
{
public:	
	CRopeokServer();
	~CRopeokServer();

	int DoRelease();
	int DoReception(PARAMTHD paramThd, HANDLE *phThdReceive);
	int CheckSocketValid(SOCKET hSocket);
	int CloseTheSocket(SOCKET SocketCon);
	virtual BOOLEAN GetRecvThdRunCondition(SOCKET SocketCon);
	virtual VOID SetRecvThdRunCondition(SOCKET SocketCon, BOOLEAN bNewCondtion);
	virtual BOOLEAN GetRecvThdState(SOCKET SocketCon);
	virtual VOID SetRecvThdState(SOCKET SocketCon, BOOLEAN bNewState);
	int EndOfRecvThd(SOCKET SocketCon);

private:
	int StartService();
	int DoReadySocket();
	int DoAcception();
	
public:
	map<SOCKET, THDINFO>	m_mapSocketThd;
	CRITICAL_SECTION				m_CriticalMapSocketThd;	// For m_mapSocketThd's accesses.

private:
	HANDLE			m_hThdAccept;
	HANDLE			m_hThdReceive;	
	//BOOL				m_bReleased; // Move this define to base class.
};

/* ================================================================== */
/*
Class Description:
	This class is inherited from CRopeokTrans. Be activated as socket's server point.
int DoRelease();
    需要在父类的析构前调用DoRelease函数释放CRopeokServer对象的资源。CRopeokServer类中的回调函数有可能继续使用父类的资源。
*/

#endif//_ROPEOKSERVER_H_