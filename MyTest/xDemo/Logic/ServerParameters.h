#pragma once

#include "RF_Position_API_v2.h"
#include "MySqlCon.h"

#include <vector>
#include <map>


class DataBase_RW
{
public:
	DataBase_RW();
	virtual ~DataBase_RW();

	// SACommand 状态必须是 fetch next 且有数据可读, c成功 return 0
	virtual int Read_FromDataBase(SACommand* pSaReader) = 0;

	virtual int ReadFile(void* pFile) { return -1; } 

	virtual bool IsDataReadSuc() const = 0;  // 检查是否读取成功 成功返回true； 

protected:
	CMyDataBase* DataBase1() { return m_pCon1; }
	CMyDataBase* DataBase2() { return m_pCon2; }

private:
	CMyDataBase* m_pCon1;
	CMyDataBase* m_pCon2;

};


class RfReader_RW: public DataBase_RW  // 单个接收器信息读取
{
public:
	RfReader_RW();
	~RfReader_RW();

public:
	//@Override
	virtual int Read_FromDataBase(SACommand* pSaReader);  

	//@Override
	virtual bool IsDataReadSuc() const { return m_bIsReadSuc; }

	const RF_INFO_READER_IN& Info() const { return m_info; }

	bool IsNomalReader() const { return m_bIsNomalReader; }

protected:
	RF_INFO_READER_IN m_info;

	bool m_bIsReadSuc; // 是否读取标志位， 带检查

	long m_lDataBaseSn;  // 数据库内部 流水号

	bool m_bIsNomalReader;
};


class RfRegion_RW : public DataBase_RW  // 单个区域信息读取
{
public:
	RfRegion_RW();
	~RfRegion_RW();

	struct SData
	{
		int Rect_ID;
		int nRectType;

		union
		{
			RF_INFO_REGION_RECT_IN stRect;
			RF_INFO_REGION_LINE_IN stLine;
			RF_INFO_REGION_POINT_IN stPoint;
		};
	};

public:
	//@Override
	virtual int Read_FromDataBase(SACommand* pSaReader);

	//@Override
	virtual bool IsDataReadSuc() const { return m_bIsRead&m_bIsReadLink; }

	virtual int ReadLink_FromDataBase();

	const RfRegion_RW::SData& Info() const { return m_info; };

// 	struct SDataBaseValue
// 	{
// 		int     Rect_ID;
// 		int     Map_ID;
// 		int     nRectType;
// 
// 		double dfLeftTop_Pos_X;
// 		double dfLeftTop_Pos_Y;
// 		double dfLeftTop_Pos_Z;        //--左上角
// 		double dfRightTop_Pos_X ;      // --右上角横坐标
// 		double dfRightTop_Pos_Y;       //--右上角纵坐标
// 		double dfRightTop_Pos_Z;       //--右上角
// 		double dfLeftBottom_Pos_X ;    // --左下角横坐标
// 		double dfLeftBottom_Pos_Y;     //--左下角纵坐标
// 		double dfLeftBottom_Pos_Z ;    // --左下角
// 		double dfRightBottom_Pos_X;    //--右下角横坐标
// 		double dfRightBottom_Pos_Y;    // --右下角纵坐标
// 		double dfRightBottom_Pos_Z;    // --右下角
// 
// 		double dfP1_Pos_X;    
// 		double dfP1_Pos_Y;   
// 		double dfP1_Pos_Z;
// 
// 		double dfP2_Pos_X;    
// 		double dfP2_Pos_Y;   
// 		double dfP2_Pos_Z;
// 
// 		double dfP0_Pos_X;    
// 		double dfP0_Pos_Y;   
// 		double dfP0_Pos_Z;
// 
// 		long lLink_RectTagID[4];   // 不是真正的id号， 是关联id号
// 		long lLink_RectReaID[4];
// 
// 		long lLink_LineReaId[2];
// 		long lLink_LineTagId[2];
// 
// 		long lLink_PointReaId;
// 
// 	}; 

private:
	SData m_info;

	//SDataBaseValue m_stData;

	//int m_nDataStyle; // 读取成功不为0； 代表数据类型

	//long m_lDataBaseSn;  // 数据库内部 流水号

 	bool m_bIsRead;
	bool m_bIsReadLink;

	int ReadLink_Rect(int nLinkRectId, RF_INFO_REGION_RECT_IN* pSink);

	int ReadLink_Line(int nLinkRectId, RF_INFO_REGION_LINE_IN* pSink);

	int ReadLink_Point(int nLinkRectId, RF_INFO_REGION_POINT_IN* pSink);

};


typedef std::vector<RfReader_RW*> VEC_PREADER_T;
typedef std::vector<RfRegion_RW*> VEC_PREGION_T;
typedef std::vector<RfReader_RW*>::iterator VEC_ITER_PREADER_T;
typedef std::vector<RfRegion_RW*>::iterator VEC_ITER_PREGION_T;

#define SERVICE_PARAMS_FILENAME    "service_parameters.ini"
#define SERVICE_PARAMS_SECTION     "service_parameters"
#define SERVICE_PARAMS_POSITION_MANAGE_SECTION     "rfid_position_database_parameters"

#define SERVICE_PARAMS_NAME        "name"
#define SERVICE_PARAMS_DB_NAME     "db_name"
#define SERVICE_PARAMS_DB_USER     "db_user"
#define SERVICE_PARAMS_DB_PASSWORD "db_password"
#define SERVICE_PARAMS_DB_IP       "db_ip"
#define SERVICE_PARAMS_DB_PORT     "db_port"


class DataBaseInfo_RW
{
public:
	DataBaseInfo_RW();
	~DataBaseInfo_RW();

	struct SInfo
	{
		char* sName;   //名称  无用
		char* sDbName; //数据库名

		char* sDbUser; //帐号
		char* sDbPwd;  //密码

		char* sDbIP;
		char* sDbPort;
	};

	static class ReadInfoCallBack
	{
	public:
		virtual void AfterGetInfo(const SInfo& stInfo1, const SInfo& stInfo2) = 0;
	};

public:
	int Read();
	// 	int Save(
	// 		const char* sName,
	// 		const char* sDbName,
	// 		const char* sDbUser,
	// 		const char* sDbPwd,
	// 		const char* sDbIP,
	// 		const char* nDbPort);

	void SetSaveFilePath(const char* sFilePath);

	const char* FilePath() { return m_sFilePath; }

	DataBaseInfo_RW::SInfo* Info() { if (!m_bIsHaveInfo) return NULL; return &m_stInfo; }

	DataBaseInfo_RW::SInfo* StorageDbInfo() { if (!m_bIsHaveInfo) return NULL; return &m_stStorageDbInfo; }

	void SetReadInfoCallBack(ReadInfoCallBack* pReadInfoCallBack) { m_pReadInfoCallBack = pReadInfoCallBack; } 

private:
	SInfo m_stInfo;

	SInfo m_stStorageDbInfo;

	bool m_bIsHaveInfo;

	char* m_sFilePath;

	ReadInfoCallBack* m_pReadInfoCallBack;

	void Clear();

};


