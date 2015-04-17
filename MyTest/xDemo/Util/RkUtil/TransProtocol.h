
// ����Э�鶨��
#ifndef _TRANS_PROTOCOL_H_
#define _TRANS_PROTOCOL_H_

//������Ϣ����
#define TRANS_TYPE_DISCONNECT 0//�Ͽ�����
#define TRANS_TYPE_CONNECT 1//��������
#define TRANS_TYPE_MOVING_OBJ_INFO 2

// Ŀ�����Ͷ���
#define TRANS_OBJECTTYPE_UNKNOW     0x00   // δ֪����
#define TRANS_OBJECTTYPE_VEHICLE    0x10   // ��
#define TRANS_OBJECTTYPE_PERSON     0x20   // ��


#define SETUP_CONNECT_TIMES 10
#define SETUP_CONNECT_DELAY 5000 //(ms)

#define HEART_PACKAGE_DELAY  15000 //(ms) ���������
#define HEART_PACKAGE_LOST_MAX 3		// û����ȷӦ����������������������������ʾӦ�ø������������ж�

#define HEART_PACKAGE_DELAY_CONTROL 5000 //(ms)���������
#define HEART_PACKAGE_LOST_MAX_CONTROL 3 // û����ȷӦ����������������������������ʾӦ�ø������������ж�

#define MAX_OBJ_NUM 20                   // ÿ����������������Ŀ����

// �������ʶ
enum PROTOCOL_ERROR_FLAG
{
	TRANS_SUCCESS = 0,		// ������ȷ��ִ��
	TRANS_EFAULT = 1,		// ����ʧ��ֵ
};

// Э�����
enum PROTOCOL_TYPE
{
	TRANS_INITIAL_REQUEST = 0, // �������Ӻ�ĳ�ʼ��Ϣ
	TRANS_INITIAL_RESPOND,	   // ��Ӧ��ʼ��������
	TRANS_MOVING_IDENTIFY, 	   // �˶�Ŀ��ʶ����Ϣ
	TRANS_HEART_PACKAGE,	   // ������
	TRANS_ALARM_INFO, 		   // ������Ϣ
	TRANS_CONNECT_CHECK,	   // ���Ӳ���	
	TRANS_TEST_TEST1,
	TRANS_TEST_TEST2,
	TRANS_FRAME_OBJS,		   //һ֡���˶�Ŀ����Ϣ
	TRANS_REGION_TAGS,         //һ������ı�ǩ����Ϣ  add by hhp @20131226
	TRANS_READER_NUMS          //��������Ϣ add by hhp @20140116
};

// �������Ӻ�ĳ�ʼ��Ϣ TRANS_INITIAL_REQUEST
typedef struct tagTRANSINITIAL_REQUEST
{
	int nLen;
	int nType;
	int nCheckA;
	int nCheckB;
	int nVSourceID;		// ��ƵԴ��ʶ
}TRANSINITIAL_REQUEST, *PTRANSINITIAL_REQUEST;

// ���س�ʼ������
typedef struct tagTRANSINITIAL_RESPOND
{
	int nLen;
	int nType;
	int nCheckA;
	int nCheckB;
	int nVSourceID;
	int nErrFlg;
}TRANSINITIAL_RESPOND, *PTRANSINITIAL_RESPOND;

// ������
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
	int nObjectTypeID;	//Ŀ�������
	int nObjectNo;			//��ǰ����ͷ��ʶ�𵽵�Ŀ����
	float dGeoX;				//Ŀ���������Ϣ
	float dGeoY;
	float dGeoEndX;
	float dGeoEndY;
	float fImageX;			//���ڱ�עĿ��ͼ���������Ϣ,��λΪ�ٷֱ�
	float fImageY;
	float fImageEndX;
	float fImageEndY;
	float fMoveSpeed;	// �˶�Ŀ���ƶ��ٶ�
}ObjInfo,*PObjInfo;


// �˶�Ŀ��ʶ�� TRANS_MOVING_IDENTIFY
typedef struct tagMovingObjInfo
{
	int nLen;
	int nType;         //��Ϣ����
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//����ͷID
	int nObjectNo;		//��ǰ����ͷ��ʶ�𵽵�Ŀ����
	int nObjectTypeID;	//Ŀ�������
	long lMovingTime;   //ʱ���
	double dGeoX;		//Ŀ���������Ϣ
	double dGeoY;
	double dGeoEndX;
	double dGeoEndY;
	float fImageX;		//���ڱ�עĿ��ͼ���������Ϣ,��λΪ�ٷֱ�
	float fImageY;
	float fImageEndX;
	float fImageEndY;
	int nMoveSpeed;	    //�ƶ��ٶ�
}MovingObjInfo, *PMovingObjInfo;

// һ֡�˶����� TRANS_MOVING_IDENTIFY
typedef struct tagFrameObjs
{
	int nLen;
	int nType;          //��Ϣ����,TRANS_FRAME_OBJS
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//����ͷID
	long lMovingTime;   //ʱ���
	int  nObjNum;       //Ŀ������
	ObjInfo Objs[MAX_OBJ_NUM];	//Ŀ�������Ϣ
}FrameObjs, *PFrameObjs;

// һ֡�˶����� TRANS_MOVING_IDENTIFY
// �������˶�Ŀ���������汾 
typedef struct tagFrameObjsBuffEx
{
	int nLen;
	int nType;          //��Ϣ����,TRANS_FRAME_OBJS
	int nCheckA;
	int nCheckB;
	int nCarmeraID;		//����ͷID
	long lMovingTime;   //ʱ���
	long lUpObjsTime;   //����ʱ��� 
	int  nObjNum;       //Ŀ������
	int  nCurBuffNums;  //��ǰ�ɴ�Ŀ������ 
	ObjInfo* pObjs;	    //Ŀ�������Ϣ
}FrameObjsBuffEx, *PFrameObjsBuffEx;


/*******************************************/
// ���Ӳ��� TRANS_CONNECT_CHECK
typedef struct tagCONCHECK
{
	int nTransType; // == TRANS_CONNECT_CHECK

}CONCHECK, *PCONCHECK;

// TRANS_TEST_TEST1
typedef struct tagTestTestInfo
{
	int nLen;
	int nType;         //��Ϣ����
	int nCheckA;
	int nCheckB;
	char buf[32];
}TestTestInfo, *PTestTestInfo;

// TRANS_TEST_TEST2
typedef struct tagTestTest2Info
{
	int nLen;
	int nType;         //��Ϣ����
	int nCheckA;
	int nCheckB;
	char buf[6];
}TestTest2Info, *PTestTest2Info;

#endif//_TRANS_PROTOCOL_H_