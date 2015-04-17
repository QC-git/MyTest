#pragma once 

#include "SQLAPI.h"

#include "x_thread.h"
// class CMySqlConnect;
//
// class CMySqlCmd
// {
// public:
// 	CMySqlCmd(CMySqlConnect* pCon);
// 	virtual ~CMySqlCmd();
// 
// 	bool Execute(SAString& str);
// 
// protected:
// 	CMySqlConnect* m_pCon;
// 
// };



class CMyDataBase
{
public:
	CMyDataBase() {;}
	virtual ~CMyDataBase();

	bool IsConnected();

	bool IsAlive();

	bool Connect(SAString& saDb, SAString& saUser, SAString& saPwd);

	bool Excute(SACommand& saCmd, SAString& saStr);

protected:
	SAConnection m_cCon;
	X::CLock m_cLock;

};

extern CMyDataBase g_cDB;
extern CMyDataBase g_cDB2;