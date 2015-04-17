
// 传输协议定义
#ifndef _TRANS_PROTOCOL_H_
#define _TRANS_PROTOCOL_H_

//定义消息类型
#define TRANS_TYPE_DISCONNECT 0//断开连接
#define TRANS_TYPE_CONNECT 1//请求连接
#define TRANS_TYPE_MOVING_OBJ_INFO 2

// 目标类型定义
#define TRANS_OBJECTTYPE_UNKNOW     0x00   // 未知类型
#define TRANS_OBJECTTYPE_VEHICLE    0x10   // 车
#define TRANS_OBJECTTYPE_PERSON     0x20   // 人


#define SETUP_CONNECT_TIMES 10
#define SETUP_CONNECT_DELAY 5000 //(ms)

#define HEART_PACKAGE_DELAY  15000 //(ms) 心跳包间隔
#define HEART_PACKAGE_LOST_MAX 3		// 没有正确应答的心跳包最大个数，如果超过则提示应用该网络连接已中断

#define HEART_PACKAGE_DELAY_CONTROL 5000 //(ms)心跳包间隔
#define HEART_PACKAGE_LOST_MAX_CONTROL 3 // 没有正确应答的心跳包最大个数，如果超过则提示应用该网络连接已中断

#define MAX_OBJ_NUM 20                   // 每个传输包所带的最大目标数

// 错误码标识
enum PROTOCOL_ERROR_FLAG
{
	TRANS_SUCCESS = 0,		// 请求正确被执行
	TRANS_EFAULT = 1,		// 常规失败值
};

// 协议类别
enum PROTOCOL_TYPE
{
	TRANS_INITIAL_REQUEST = 0, // 建立连接后的初始信息
	TRANS_INITIAL_RESPOND,	   // 回应初始连接请求
	TRANS_MOVING_IDENTIFY, 	   // 运动目标识别信息
	TRANS_HEART_PACKAGE,	   // 心跳包
	TRANS_ALARM_INFO, 		   // 报警信息
	TRANS_CONNECT_CHECK,	   // 连接测试	
	TRANS_TEST_TEST1,
	TRANS_TEST_TEST2,
	TRANS_FRAME_OBJS,		   //一帧的运动目标信息
	TRANS_REGION_TAGS,         //一个区域的标签卡信息  add by hhp @20131226
	TRANS_READER_NUMS          //接收器信息 add by hhp @20140116
};

// 建立连接后的初始信息 TRANS_INITIAL_REQUEST
typedef struct tagTRANSINITIAL_REQUEST
{
	int nLen;
	int nType;
	int nCheckA;
	int nCheckB;
	int nVSourceID;		// 视频源标识
}TRANSINITIAL_REQUEST, *PTRANSINITIAL_REQUEST;

// 返回初始请求结果
typedef struct tagTRANSINITIAL_RESPOND
{
	int nLen;
	int nType;
	int nCheckA;
	int nCheckB;
	int nVSourceID;
	int nErrFlg;
}TRANSINITIAL_RESPOND, *PTRANSINITIAL_RESPOND;

// 心跳包
typedef struct tagHEART_PACKAGE
{
	int nLen;
	int nType;
	int nCheckA;
	int nCheckB;
	int nVSourceID;
	int nErrFlg;
}HEART_PACKAGE, *PHEART_PACKAGE;

typedef struct tagObjInfo
{
	int nObjectTypeID;	//目标的类型
	int nObjectNo;			//当前摄像头所识别到的目标编号
	float dGeoX;				//目标的坐标信息
	float dGeoY;
	float dGeoEndX;
	float dGeoEndY;
	float fImageX;			//用于标注目标图框的坐标信息,单位为百分比
	float fImageY;
	float fImageEndX;
	float fImageEndY;
	float fMoveSpeed;	// 运动目标移动速度
}ObjInfo,*PObjInfo;


// 运动目标识别 TRANS_MOVING_IDENTIFY
typedef struct tagMovingObjInfo
{
	int nLen;
	int nType;         //信息类型
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//摄像头ID
	int nObjectNo;		//当前摄像头所识别到的目标编号
	int nObjectTypeID;	//目标的类型
	long lMovingTime;   //时间戳
	double dGeoX;		//目标的坐标信息
	double dGeoY;
	double dGeoEndX;
	double dGeoEndY;
	float fImageX;		//用于标注目标图框的坐标信息,单位为百分比
	float fImageY;
	float fImageEndX;
	float fImageEndY;
	int nMoveSpeed;	    //移动速度
}MovingObjInfo, *PMovingObjInfo;

// 一帧运动内容 TRANS_MOVING_IDENTIFY
typedef struct tagFrameObjs
{
	int nLen;
	int nType;          //信息类型,TRANS_FRAME_OBJS
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//摄像头ID
	long lMovingTime;   //时间戳
	int  nObjNum;       //目标数量
	ObjInfo Objs[MAX_OBJ_NUM];	//目标具体信息
}FrameObjs, *PFrameObjs;

// 一帧运动内容 TRANS_MOVING_IDENTIFY
// 无限制运动目标最大个数版本 
typedef struct tagFrameObjsBuffEx
{
	int nLen;
	int nType;          //信息类型,TRANS_FRAME_OBJS
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//摄像头ID
	long lMovingTime;   //时间戳
	long lUpObjsTime;   //更新时间戳 
	int  nObjNum;       //目标数量
	int  nCurBuffNums;  //当前可存目标数量 
	ObjInfo* pObjs;	    //目标具体信息
}FrameObjsBuffEx, *PFrameObjsBuffEx;


/*******************************************/
// 连接测试 TRANS_CONNECT_CHECK
typedef struct tagCONCHECK
{
	int nTransType; // == TRANS_CONNECT_CHECK

}CONCHECK, *PCONCHECK;

// TRANS_TEST_TEST1
typedef struct tagTestTestInfo
{
	int nLen;
	int nType;         //信息类型
	int nCheckA;
	int nCheckB;
	char buf[32];
}TestTestInfo, *PTestTestInfo;

// TRANS_TEST_TEST2
typedef struct tagTestTest2Info
{
	int nLen;
	int nType;         //信息类型
	int nCheckA;
	int nCheckB;
	char buf[6];
}TestTest2Info, *PTestTest2Info;

#endif//_TRANS_PROTOCOL_H_