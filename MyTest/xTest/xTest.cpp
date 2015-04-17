// xTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include <windows.h>

#include "x_dummy.h"
#include "x_util.h"
#include "x_thread.h"
#include "x_container.h"
// 
 #pragma comment(lib, "xService.lib")


namespace hhhh
{

#include <stdio.h>
	typedef signed char             S8ss_T;
	void func()
	{
		;
	}
#define FUNC hhhh::func
}


// #define sas  #define AAA 111
// sas
int _tmain(int argc, _TCHAR* argv[])
{
	LOG_F("xTest main()");
	//CRITICAL_SECTION ds;
	//_SERVICE_HMUTEX d;
	//HMUTEX_D s;
//	X::DummyTest();
//	X::UtilTest();
//	X::ThreadTest();
	X::ContainerTest();

	//PRINT_F();

// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
 	
// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
// 	OUT_F<<1+10000<<2<<"hello"<<11.23<<"a";
	
	return 0;
}

