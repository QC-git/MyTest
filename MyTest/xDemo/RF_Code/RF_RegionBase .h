//#include "vld.h"
#pragma once

#include "RF_RectRegion.h"
#include "RF_Device.h"

bool Load_RfDevice(HANDLE hDevice);


class CRF_RectRegionBase_M
{

public:
	CRF_RectRegionBase_M();

public:
	bool Region_Init(RF_RectRegionAttr_s);
	bool Read_RegionAttr(RF_RectRegionAttr_s* pData_out);

	bool Build_VSSM(PVOID pRssiData, bool bIsMirror=false);    //500ms����һ��
	bool Read_VSSM(int nVSSMStyle, float* pVSSM, float* pMax, float* pMin);

	bool GetRssiList(long nTag, PVOID pRssiData, float* pOutRssi, bool bIsMirror=false);   //��ȡTag�ڸ�����ĸ�����������ǿ�ȴ�С  ��ֵ��С��������

	floatP dB_to_XY(long nTag, PVOID pRssiData,  bool MirrorMode=false, int AlgMode=0);      //����ʼ���ɹ��������ת��
	floatP dB_to_XY2(long nTag, PVOID pRssiData, PVOID pReaderData,  bool MirrorMode=false, int AlgMode=0);  // AlgMode = 0,��ֵ���� AlgMode = 1�� �������
	floatP CorrectXY(floatP Point);

private:
	enum
	{
		PointNum=4,
		RealWidth =20,
		RealHeight=20,
		VSSM_Width =31,
		VSSM_Height=31,
	};

	bool m_bIsIDInit;
	bool m_bIsRectInit;
	bool m_IsMirror;

	RF_ID_s PID[4];	
	RF_Rect_s m_Rect;

	//------------------------���������Ϣ-----------------------------
	bool m_bIsVSSM_Built;
	double m_adValue[PointNum][PointNum];        
	float oriVSSM[PointNum*VSSM_Width*VSSM_Height]; //��׼����
	float meanVSSM[PointNum*VSSM_Width*VSSM_Height];     //��ֵ����
	struct ParameterModel
	{
		float Max;
		float Min;
	}m_Par;

	//-------------------------ǿ��ת������----------------------------
	void dB_Adjust(float* dB_V);

	floatP dB_to_XY_opt(const float* dB_V,const float* pVSSM);

	bool GenerateSetOfMirrorRssi(long nTag, PVOID pRssiData, float* pRssi, float fCoe=1.0f); 

	//int CheckRegionReaderIsNomal(PVOID pReaderData);    //���������Ľ���������

	int IncompleteState_opt(long lTag, PVOID pRssiData, PVOID pReaderData, floatP* Pout, bool bIsMirrorRect);   //����ֵ  ��Ч���������� ��Ϊ4ʱ  ������������ 


};

