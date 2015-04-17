#ifndef __RKGUI_SYSTEM_H__
#define __RKGUI_SYSTEM_H__


#ifdef __cplusplus
extern "C"{
#endif


#include "RKGUI_Typedef.h"

#define RKGUI_LOG_PATH "\\RKGUI_Log.txt"

#define RKGUI_LOG(s)  RKGUI_PutStringToLog s

#define RKGUI_PROCESS_MAXNUMS 16              // CPU 最大核数


//////////////////////////////////////////////////////////////////////////
//文件信息结构体
typedef struct tagDDB_FileConfig
{
	unsigned long dwFileLen;                   //文件大小
	RKGUI_tchar tcFileName[RKGUI_MAX_PATH];    //文件名
}DDB_FileConfig, *PDDB_FileConfig;

//硬盘信息
typedef struct tagRKGUI_DiskInfo_st
{
	char szDiskSign[8];              // 盘符名
	unsigned int  uDiskType;         // 类型 0:未知设备  1:无根目录 2:移动硬盘或U盘等移动设备 3:本地硬盘
	unsigned long dwTotalMBSize;     // 硬盘总容量
	unsigned long dwFreeMBSize;      // 可用容量
}RKGUI_DiskInfo_st, *PRKGUI_DiskInfo_st;

//所有硬盘信息
typedef struct tagRKGUI_AllDisk_st
{
	int nDiskNums;                    // 硬盘数量
	RKGUI_DiskInfo_st *pDiskInfo;     // 硬盘信息指针
}RKGUI_AllDisk_st, *PRKGUI_AllDisk_st;

//yuv 转 RGB
typedef   unsigned char     uint8_t;
typedef   ULONGLONG         uint64_t;
void YUV_TO_RGB24(uint8_t *pYUV_Y, int nLines_Y, uint8_t *pYUV_U,
				  uint8_t *pYUV_V, int nLines_UV, uint8_t *pRGB,
				  int nWidth, int nHeight,int nLineBytes);


//////////////////////////////////////////////////////////////////////////
//系统操作
//获取系统CPU核数
RKGUI_int RKGUI_GetCPUNumbers(int* pProNums);

//获取系统CPU使用率 0-100%
//说明: 由于使用空闲时间计算因此最好能够每秒调用这样才能准确
//      仅仅一次调用不准确
RKGUI_int RKGUI_GetCPU_UseRate( int *pUsePer);

//获取系统时间
RKGUI_void RKGUI_GetSystemTime(int DateTime[6]);

//获取系统时间(包括星期)
RKGUI_void RKGUI_GetSystemWeekTime(int DateTime[7]);

//设置系统时间
RKGUI_void RKGUI_SetSystemTime(int DateTime[6]);

//获取当前系统时间的星期
RKGUI_int RKGUI_GetDayOfWeek();

//获取当前系统内存状态
RKGUI_void RKGUI_GetSystemMemoryStatus(int *iTotalMem, int *iAvailPhysMem, int *iAvailVirtualMem);

//显示当前系统内存状态
RKGUI_void RKGUI_DrawMemoryStatus(HDC hDc, RECT rcShow);

//写当前系统内存状态MemoeyStatus.txt
RKGUI_void RKGUI_PrintSystemMemoryStatus();

//获取当前磁盘剩余空间
RKGUI_int  RKGUI_GetDiskFreeSpace(RKGUI_char *pRootPath);

//获取当前所有硬盘数量
RKGUI_int  RKGUI_GetDiskNums(int *pDiskNum);

//获取所有硬盘信息
RKGUI_int  RKGUI_GetLocalDiskInfo(RKGUI_AllDisk_st* pDiskAllInfo);

//获取当前系统运行的时间
RKGUI_int  RKGUI_GetTickCount();

//获取程序路径
RKGUI_int RKGUI_GetGlobalModuleAppPath(RKGUI_tchar *pAppFile);

//获取程序路径
RKGUI_int RKGUI_GetModuleAppPath(RKGUI_tchar *tcAppPath);

//获取程序名称(不带路径)
RKGUI_int RKGUI_GetModuleAppName(RKGUI_tchar *tcAppName);

//获取文件所处的目录
RKGUI_int RKGUI_GetFilePath(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFilePath);

//获取文件名
RKGUI_int RKGUI_GetFileName(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFileName);

//用explorer打开文件所在目录
RKGUI_int RKGUI_ExplorerDirectory(const RKGUI_tchar *pFileName, int  nType);

//获取程序路径宽字符
RKGUI_int RKGUI_GetModuleAppPath_W(RKGUI_wchar_t *wcAppPath);

//设置log路径
RKGUI_int RKGUI_SetLogFilePath();

//写记录到log文件
RKGUI_int RKGUI_SaveLogData(RKGUI_uchar *pData, RKGUI_int dwLength, RKGUI_tchar* pFileName);

//写log文件
RKGUI_void RKGUI_PutStringToLog(RKGUI_tchar* pLogBuffer, ...);

//写log文件(log时间)
RKGUI_void RKGUI_PutStringToLogWidthTime(RKGUI_tchar* pLogBuffer, ...);

//Utf-8转多字节字符
RKGUI_int RKGUI_UtfToMultiByte(const RKGUI_uchar* utf8, RKGUI_char *ascii, int asciilen);



//////////////////////////////////////////////////////////////////////////
//文件操作
//获取文件大小
RKGUI_ulong RKGUI_GetFileSize(RKGUI_tchar *pFileName);

//遍历指定路径和指定扩展名的文件
RKGUI_void RKGUI_FindTCFileInDirEx(const RKGUI_tchar* pDirectory,const RKGUI_tchar* pSuffix, RKGUI_int iMaxNum,
								 DDB_FileConfig *pRetPath, RKGUI_int *iRetNum);

//删除指定路径内所有空文件夹
RKGUI_void RKGUI_DeleteEmptyDirectoryEx(const RKGUI_tchar* pDirectory);

//删除指定路径内指定扩展名的文件或文件夹
RKGUI_void RKGUI_ClearFileSuffixInDirEx(const RKGUI_tchar* pDirectory,const RKGUI_tchar* pSuffix);

//删除指定路径内所有文件或文件夹(不包括目录本身)
RKGUI_void RKGUI_ClearDirectoryEx(const RKGUI_tchar* pDirectory);


// 删除指定文件名的文件
RKGUI_bool RKGUI_DeleteFile(const RKGUI_char *pFileName);

// 确认文件夹目录是否存在
RKGUI_bool RKGUI_CheckDirectory(const RKGUI_char *pDirectory);

//创建文件夹目录
RKGUI_bool RKGUI_CreateDirectory(const RKGUI_char *pszFileName);	

//创建文件夹目录
RKGUI_bool RKGUI_CreateDirectoryT(const RKGUI_tchar* pDirectory);

//创建文件夹目录扩展
RKGUI_bool RKGUI_CreateDirectoryEx(const RKGUI_tchar* pDirectory,const char* pDivide);

//////////////////////////////////////////////////////////////////////////
// 弹出路径选择对话框，返回所选路径
RKGUI_bool RKGUI_SelectFilePath(char *pFilePath, char *pTitle, void *lpifq);

// 创建程序互斥，只打开一个程序
RKGUI_bool RKGUI_CreateOnlyOneApp(RKGUI_wchar_t *pMutexName);

// 启动指定程序
RKGUI_bool RKGUI_OpenProcess(RKGUI_tchar *pProcName);

//////////////////////////////////////////////////////////////////////////
// 注册服务器监测
RKGUI_bool RKGUI_RegisterServer(RKGUI_bool bRegister, RKGUI_tchar *pServerName, RKGUI_tchar *pWatchName);



#ifdef __cplusplus
}
#endif
#endif
