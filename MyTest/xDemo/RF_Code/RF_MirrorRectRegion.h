#pragma once

#include "RF_RectRegion.h"


class CRF_MirrorRectRegion:public RF_CRegionBase, public RF_CRectRDP
{
public:
	CRF_MirrorRectRegion();
	//~CRF_MirrorRectRegion();

	int SetAttr(RF_RectRegionAttr_s); // suc return 0
	int ReadAttr(RF_RectRegionAttr_s* pData_out); // suc return 0

	//Override
	int BuildVSS();    //500ms建立一次

	//Override
	int dB_to_XY(long nTagId, floatP* pData_out); 

	//Overeride
	int GetRssiList(long nTagId, float* pData_out);

	//Overeride
	int CheckRegionDevice();

	int Read_VSS(int style, float* pVSS, float* pMaxValue, float* pMinValue); //return vss 长度 style = 0 为meanVSS

private:
	enum
	{
		PointNum=4,
		RealWidth =20,
		RealHeight=20,
		VSSM_Width =31,
		VSSM_Height=31,
	};

	bool m_bAttrInit;
	RF_ID_s PID[4];	
	RF_Rect_s m_Rect;

	bool m_bVSSbuilt;
	double m_adValue[PointNum][PointNum];      //强度值？  
	float oriVSSM[PointNum*VSSM_Width*VSSM_Height]; //标准矩阵
	float meanVSSM[PointNum*VSSM_Width*VSSM_Height];     //均值矩阵
	float m_maxValue;
	float m_minValue;

	bool m_bFirstBuiltVSS;

	enum READER_LAYOUT_TYPE
	{
		READER_LAYOUT_TYPE1_LB_RB = 1,    //分布于  左下， 右下   0:1
		READER_LAYOUT_TYPE2_RB_RT = 2,    //        右下， 右上   1:2
		READER_LAYOUT_TYPE3_RT_LT = 3,    //        左上， 右上   2:3
		READER_LAYOUT_TYPE4_LT_LB = 4     //        左上， 左下   3:0
	};
	int m_ReaderLayoutType; // 接收器布局类型 共四种

	void dB_Adjust(float* dB_V);

	floatP dB_to_XY_opt(const float* dB_V,const float* pVSSM);

	bool GenerateSetOfMirrorRssi(long nTag, PVOID pRssiData, float* pRssi, float fCoe=1.0f);

};