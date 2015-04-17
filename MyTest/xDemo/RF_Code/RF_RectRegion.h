#pragma once

#include "RF_Region.h"

struct RF_RectRegionAttr_s
{
	long left;
	long top;
	long right;
	long bottom;

	UINT ltReaderID;      //lefttop
	UINT lbReaderID;      //leftbottom
	UINT rtReaderID;      //righttop
	UINT rbReaderID;      //rightbottom

	UINT ltTagID;
	UINT lbTagID;
	UINT rtTagID;
	UINT rbTagID;
};

class CRF_RectRegion_M:public RF_CRegionBase, public RF_CRectRDP
{
public:
	CRF_RectRegion_M();
	//~CRF_RectRegion_M();

	int SetAttr(RF_RectRegionAttr_s);             // suc return 0
	int ReadAttr(RF_RectRegionAttr_s* pData_out); // suc return 0

	//Override
	int BuildVSS();    //500ms建立一次

	//Override
	int dB_to_XY(long nTagId, floatP* pData_out); 

	//Overeride
	int GetRssiList(long nTagId, float* pData_out);

	//Override
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

	void dB_Adjust(float* dB_V);

	floatP dB_to_XY_opt(const float* dB_V,const float* pVSSM);

};