#if !defined AFD_READER_COMM
#define AFD_READER_COMM
typedef struct RFID_Packet_t 
{
	UCHAR Tag_id[4];
	UCHAR Usr_Info[4];
	UINT RN;
	UCHAR Ant;
	UCHAR Atten;
	UCHAR Rssi;		//433 Only
	UCHAR InfoEx;
	UCHAR TagIDOk;
	UCHAR TagKeyPress;
	UCHAR TagPw;
	
}RfPacket;

typedef struct RFID_PacketEx 
{
	RfPacket pk;
	UCHAR Len;
	UCHAR TagNum;
	UCHAR bMiddle;
	UCHAR LQI;		//433 Only
	UCHAR MID[4];
	UCHAR MRssi;
	UCHAR MLQI;
}RfPacketEx;

typedef struct CmdPk_s
{
	UINT Jh;
	UCHAR nEvent;
	UCHAR Err;
	UCHAR CmdLen;
	char CmdLine[256];
}CmdPk_t;

extern  int _stdcall  AFD_ReaderComm_Open(LPCTSTR strCfg);
extern  BOOL _stdcall AFD_ReaderComm_Close(UINT uiCommSN);
extern  void _stdcall AFD_ReaderComm_CloseAll(void);
extern  BOOL _stdcall AFD_ReaderComm_LinkRecover(UINT uiTimeout_ms);
extern  int _stdcall  AFD_ReaderComm_GetCommList(char* outStr);
extern  int _stdcall  AFD_ReaderComm_GetTags(UINT uiCommSN, char* outDatas, int num);

#endif                                                  