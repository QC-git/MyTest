#include "stdafx.h"

#include "RfServiceClientTest.h"


#ifndef MY_RFSERVER_DEBUG
#define MY_RFSERVER_DEBUG
#endif

CRfServiceClient::CRfServiceClient()
	: m_nFd(-1)
{
	;
}

CRfServiceClient::~CRfServiceClient()
{
	StopTest();
}

bool CRfServiceClient::StartTest()
{
	return TStartup();
}

void CRfServiceClient::StopTest()
{
	TClose();

	if ( -1 != m_nFd )
	{
		closesocket(m_nFd);
		m_nFd = -1;
	}

}

void CRfServiceClient::DoRoundPro()
{
	if ( -1 == m_nFd )
	{
		// 创建套接字
		m_nFd = socket(AF_INET, SOCK_STREAM, 0);
		if ( m_nFd < 0 )
		{
			m_nFd = -1;
			Sleep(100);
			return;
		}

		// 设置参数
		int nBuffSize = 1024*512;
		setsockopt(m_nFd, SOL_SOCKET,SO_RCVBUF, (char *)&nBuffSize, sizeof(int));

		timeval tmOutOfTime;
		tmOutOfTime.tv_sec = 2;
		tmOutOfTime.tv_usec=0;
		setsockopt(m_nFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tmOutOfTime, sizeof(timeval));

		// 设置地址

		const char* sAddr = "127.0.0.1";
		unsigned short uiPort = 8111;

		struct sockaddr_in stAddr;
		stAddr.sin_family = AF_INET;
		stAddr.sin_addr.s_addr = inet_addr(sAddr);
		stAddr.sin_port = htons(uiPort);

		// 连接

		unsigned long ulFlag;
		int bConnected = 0;

		ulFlag = 1;
		//ioctlsocket(m_nFd, FIONBIO, (unsigned long*)&ulFlag);


		if ( connect(m_nFd, (const struct sockaddr*)&stAddr, sizeof(stAddr)) != 0 ) 
		{

			// 设置超时时间
// 			fd_set stFdSet;
// 			struct timeval tmTimeVal;
// 
// 			tmTimeVal.tv_sec = 1;
// 			tmTimeVal.tv_usec = 0;
// 			FD_ZERO(&stFdSet);
// 			FD_SET((unsigned int)m_nFd, &stFdSet);
// 
// 			int nSelRet = select(m_nFd, 0, &stFdSet, 0, &tmTimeVal);
// 			if (nSelRet > 0)
// 			{
// 				int nSelError = -1;
// 				int nSelErrorLen = sizeof(int);
// 
// 				getsockopt(m_nFd, SOL_SOCKET, SO_ERROR, (char*)&nSelError, &nSelErrorLen);
// 				if ( 0 != nSelError )
// 				{
// 					bConnected = 0;
// 				}else
// 				{
// 					bConnected = 1;
// 				}
// 
// 			}
// 			else
// 			{
// 				bConnected = 0;
// 			} 
			int nRecvErr = GetLastError();
			bConnected = 0;

		}
		else
		{
			bConnected = 1;
		}

		ulFlag = 0;
		ioctlsocket(m_nFd, FIONBIO, (unsigned long*)&ulFlag);

		if ( 0 == bConnected )
		{
			closesocket(m_nFd);
			m_nFd = -1;

			Sleep(100);
			return;
		}
		else
		{
			SPack ObjsInfo = {0};

			ObjsInfo.nLen = sizeof(SPack);
			ObjsInfo.nType = TRANS_INITIAL_REQUEST;
			ObjsInfo.nCheckA = TRANS_CHECK_CODE_A;
			ObjsInfo.nCheckB = TRANS_CHECK_CODE_B;
			ObjsInfo.lTime = time(NULL);

			int nSendRet = send(m_nFd, (const char*)&ObjsInfo, sizeof(SPack), 0);

			LOG_F("Client connect service success !, nSendRet = %d", nSendRet);
		}
	}
	
	if ( -1 == m_nFd )
	{
		return;
	}

	// 开始接收

	const int nRevBuffLen = 1024 * 10;
	char pRevBuff[nRevBuffLen];

	int nRevLen = recv(m_nFd, pRevBuff, nRevBuffLen-1,0);
	if ( nRevLen <= 0 )
	{
		int nRecvErr = GetLastError();

		Sleep(50);
		return;
	}
	else
	{
		// 开始解析

		pRevBuff[nRevLen] = '\0';

		TiXmlDocument* pDoc = new TiXmlDocument;
		if ( NULL == pDoc->Parse(pRevBuff, 0) )
		{
			LOGERR_F("CRfServiceClient::DoRoundPro(),  pDoc->Parse() fail");
			delete pDoc;
			return;
		}

		TiXmlPrinter* pXmlData = new TiXmlPrinter;
		if ( false == pDoc->Accept(pXmlData) )
		{
			LOGERR_F("CRfServiceClient::DoRoundPro(),  pDoc->Accept() fail");
			delete pXmlData;
			return;
		}
		else
		{
			// 打印数据

#ifdef MY_RFSERVER_DEBUG

			int nSize = pXmlData->Size();
			char* pStr = (char*)pXmlData->CStr();
			int nCurP = 0;
			char chOld;

			LOG_F("CRfServiceClient::DoRoundPro(), size = %d, xml = \n", nSize);
			while( nCurP < nSize)
			{
				if ( nCurP + 500 < nSize )
				{
					chOld = pStr[500];
					pStr[500] = '\0';
				}


				TRACE("%s", pStr);

				if ( nCurP + 500 < nSize )
				{
					pStr[500] = chOld;

					nCurP+=500;
					pStr += 500;
				}
				else
				{
					break;
				}

			}

			TRACE("\n");

#endif	

		}

		delete pDoc;
		delete pXmlData;

	}


}