#pragma once

#include "RkUtil.h"

#include "RF_Position_API_v2.h"

#define MY_CLIENT_TEST_ENABLE 1

class CRfServiceClient : public X::CRoundThread
{
public:
	CRfServiceClient();
	~CRfServiceClient();

	struct SPack 
	{
		int nLen;
		int nType;      //信息类型,TRANS_READER_NUMS
		int nCheckA;
		int nCheckB;

		long lTime;
	};

	bool StartTest();
	void StopTest();

public:
	//Override
	void DoRoundPro();

private:
	int m_nFd;
};
