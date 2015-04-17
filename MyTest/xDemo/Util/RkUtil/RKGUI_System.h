#ifndef __RKGUI_SYSTEM_H__
#define __RKGUI_SYSTEM_H__


#ifdef __cplusplus
extern "C"{
#endif


#include "RKGUI_Typedef.h"

#define RKGUI_LOG_PATH "\\RKGUI_Log.txt"

#define RKGUI_LOG(s)  RKGUI_PutStringToLog s

#define RKGUI_PROCESS_MAXNUMS 16              // CPU ������


//////////////////////////////////////////////////////////////////////////
//�ļ���Ϣ�ṹ��
typedef struct tagDDB_FileConfig
{
	unsigned long dwFileLen;                   //�ļ���С
	RKGUI_tchar tcFileName[RKGUI_MAX_PATH];    //�ļ���
}DDB_FileConfig, *PDDB_FileConfig;

//Ӳ����Ϣ
typedef struct tagRKGUI_DiskInfo_st
{
	char szDiskSign[8];              // �̷���
	unsigned int  uDiskType;         // ���� 0:δ֪�豸  1:�޸�Ŀ¼ 2:�ƶ�Ӳ�̻�U�̵��ƶ��豸 3:����Ӳ��
	unsigned long dwTotalMBSize;     // Ӳ��������
	unsigned long dwFreeMBSize;      // ��������
}RKGUI_DiskInfo_st, *PRKGUI_DiskInfo_st;

//����Ӳ����Ϣ
typedef struct tagRKGUI_AllDisk_st
{
	int nDiskNums;                    // Ӳ������
	RKGUI_DiskInfo_st *pDiskInfo;     // Ӳ����Ϣָ��
}RKGUI_AllDisk_st, *PRKGUI_AllDisk_st;

//yuv ת RGB
typedef   unsigned char     uint8_t;
typedef   ULONGLONG         uint64_t;
void YUV_TO_RGB24(uint8_t *pYUV_Y, int nLines_Y, uint8_t *pYUV_U,
				  uint8_t *pYUV_V, int nLines_UV, uint8_t *pRGB,
				  int nWidth, int nHeight,int nLineBytes);


//////////////////////////////////////////////////////////////////////////
//ϵͳ����
//��ȡϵͳCPU����
RKGUI_int RKGUI_GetCPUNumbers(int* pProNums);

//��ȡϵͳCPUʹ���� 0-100%
//˵��: ����ʹ�ÿ���ʱ������������ܹ�ÿ�������������׼ȷ
//      ����һ�ε��ò�׼ȷ
RKGUI_int RKGUI_GetCPU_UseRate( int *pUsePer);

//��ȡϵͳʱ��
RKGUI_void RKGUI_GetSystemTime(int DateTime[6]);

//��ȡϵͳʱ��(��������)
RKGUI_void RKGUI_GetSystemWeekTime(int DateTime[7]);

//����ϵͳʱ��
RKGUI_void RKGUI_SetSystemTime(int DateTime[6]);

//��ȡ��ǰϵͳʱ�������
RKGUI_int RKGUI_GetDayOfWeek();

//��ȡ��ǰϵͳ�ڴ�״̬
RKGUI_void RKGUI_GetSystemMemoryStatus(int *iTotalMem, int *iAvailPhysMem, int *iAvailVirtualMem);

//��ʾ��ǰϵͳ�ڴ�״̬
RKGUI_void RKGUI_DrawMemoryStatus(HDC hDc, RECT rcShow);

//д��ǰϵͳ�ڴ�״̬MemoeyStatus.txt
RKGUI_void RKGUI_PrintSystemMemoryStatus();

//��ȡ��ǰ����ʣ��ռ�
RKGUI_int  RKGUI_GetDiskFreeSpace(RKGUI_char *pRootPath);

//��ȡ��ǰ����Ӳ������
RKGUI_int  RKGUI_GetDiskNums(int *pDiskNum);

//��ȡ����Ӳ����Ϣ
RKGUI_int  RKGUI_GetLocalDiskInfo(RKGUI_AllDisk_st* pDiskAllInfo);

//��ȡ��ǰϵͳ���е�ʱ��
RKGUI_int  RKGUI_GetTickCount();

//��ȡ����·��
RKGUI_int RKGUI_GetGlobalModuleAppPath(RKGUI_tchar *pAppFile);

//��ȡ����·��
RKGUI_int RKGUI_GetModuleAppPath(RKGUI_tchar *tcAppPath);

//��ȡ��������(����·��)
RKGUI_int RKGUI_GetModuleAppName(RKGUI_tchar *tcAppName);

//��ȡ�ļ�������Ŀ¼
RKGUI_int RKGUI_GetFilePath(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFilePath);

//��ȡ�ļ���
RKGUI_int RKGUI_GetFileName(const RKGUI_tchar *pFileFullPath, RKGUI_tchar *pFileName);

//��explorer���ļ�����Ŀ¼
RKGUI_int RKGUI_ExplorerDirectory(const RKGUI_tchar *pFileName, int  nType);

//��ȡ����·�����ַ�
RKGUI_int RKGUI_GetModuleAppPath_W(RKGUI_wchar_t *wcAppPath);

//����log·��
RKGUI_int RKGUI_SetLogFilePath();

//д��¼��log�ļ�
RKGUI_int RKGUI_SaveLogData(RKGUI_uchar *pData, RKGUI_int dwLength, RKGUI_tchar* pFileName);

//дlog�ļ�
RKGUI_void RKGUI_PutStringToLog(RKGUI_tchar* pLogBuffer, ...);

//дlog�ļ�(logʱ��)
RKGUI_void RKGUI_PutStringToLogWidthTime(RKGUI_tchar* pLogBuffer, ...);

//Utf-8ת���ֽ��ַ�
RKGUI_int RKGUI_UtfToMultiByte(const RKGUI_uchar* utf8, RKGUI_char *ascii, int asciilen);



//////////////////////////////////////////////////////////////////////////
//�ļ�����
//��ȡ�ļ���С
RKGUI_ulong RKGUI_GetFileSize(RKGUI_tchar *pFileName);

//����ָ��·����ָ����չ�����ļ�
RKGUI_void RKGUI_FindTCFileInDirEx(const RKGUI_tchar* pDirectory,const RKGUI_tchar* pSuffix, RKGUI_int iMaxNum,
								 DDB_FileConfig *pRetPath, RKGUI_int *iRetNum);

//ɾ��ָ��·�������п��ļ���
RKGUI_void RKGUI_DeleteEmptyDirectoryEx(const RKGUI_tchar* pDirectory);

//ɾ��ָ��·����ָ����չ�����ļ����ļ���
RKGUI_void RKGUI_ClearFileSuffixInDirEx(const RKGUI_tchar* pDirectory,const RKGUI_tchar* pSuffix);

//ɾ��ָ��·���������ļ����ļ���(������Ŀ¼����)
RKGUI_void RKGUI_ClearDirectoryEx(const RKGUI_tchar* pDirectory);


// ɾ��ָ���ļ������ļ�
RKGUI_bool RKGUI_DeleteFile(const RKGUI_char *pFileName);

// ȷ���ļ���Ŀ¼�Ƿ����
RKGUI_bool RKGUI_CheckDirectory(const RKGUI_char *pDirectory);

//�����ļ���Ŀ¼
RKGUI_bool RKGUI_CreateDirectory(const RKGUI_char *pszFileName);	

//�����ļ���Ŀ¼
RKGUI_bool RKGUI_CreateDirectoryT(const RKGUI_tchar* pDirectory);

//�����ļ���Ŀ¼��չ
RKGUI_bool RKGUI_CreateDirectoryEx(const RKGUI_tchar* pDirectory,const char* pDivide);

//////////////////////////////////////////////////////////////////////////
// ����·��ѡ��Ի��򣬷�����ѡ·��
RKGUI_bool RKGUI_SelectFilePath(char *pFilePath, char *pTitle, void *lpifq);

// �������򻥳⣬ֻ��һ������
RKGUI_bool RKGUI_CreateOnlyOneApp(RKGUI_wchar_t *pMutexName);

// ����ָ������
RKGUI_bool RKGUI_OpenProcess(RKGUI_tchar *pProcName);

//////////////////////////////////////////////////////////////////////////
// ע����������
RKGUI_bool RKGUI_RegisterServer(RKGUI_bool bRegister, RKGUI_tchar *pServerName, RKGUI_tchar *pWatchName);



#ifdef __cplusplus
}
#endif
#endif
