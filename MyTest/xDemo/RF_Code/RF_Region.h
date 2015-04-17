#pragma once

#include "RF_Algorithm.h"
// 
// #define RF_REGION_STYLE_RECT  1
// #define RF_REGION_STYLE_LINE  2
// #define RF_REGION_STYLE_POINT 3

#include "RF_Position_API_v2.h"

class RF_CRegionBase
{
public:
	RF_CRegionBase();

	int SetId(int id);
	int GetId();

	int BandDev(void* dev);
	void* GetDev();

	void SetStyle(RF_REGION_STYLE style) { m_style = style; }
	RF_REGION_STYLE GetStyle() { return m_style; }

	int SetMode(int mode);
	int GetMode();

	virtual int BuildVSS() = 0;

	virtual int dB_to_XY(long nTagId, floatP* pData_out) = 0;

	//检测区域设备是否正常
	virtual int CheckRegionDevice() = 0;

	/*
		获取一组 rssi 强度值 返回个数，应与不同的区域中接收器个数对应 不排序
	*/
	virtual int GetRssiList(long nTagId, float* pData_out) = 0; 

	//virtual int ReadAttr(void* pAttr_out) = 0;

private:
	int m_id;  // >0

	void* m_dev; // != NULL

	RF_REGION_STYLE m_style; // 1 - 3

	int m_mode;  //运算模式，自由设定
};

class RF_CRegionDefaultParameters  // 区域共有的参数
{
public:
	RF_CRegionDefaultParameters()
		: m_bIsUseDefaultRssi(true)
	{

	}
	~RF_CRegionDefaultParameters() {;}

public:
	bool m_bIsUseDefaultRssi;
};

class RF_CRectRDP: public RF_CRegionDefaultParameters  // 矩形区域特有的参数  用于矩形区域和镜像区域
{
public:
	RF_CRectRDP();
	~RF_CRectRDP();

	//void CopyDefault();

	//接收器获取矩形内四个定点参考卡的参数， 用以形成vss
	//
	struct SReaderRssiFromRectTag  // 四个接收器以 左下角开始，逆时针四个点
	{
		float fRssiListP1[4];
		float fRssiListP2[4];
		float fRssiListP3[4];
		float fRssiListP4[4];
	};
public:
	SReaderRssiFromRectTag m_stDefaultInfo;
};

class RF_CLineRDP: public RF_CRegionDefaultParameters   // 线性区域特有的参数  
{
public:
	RF_CLineRDP();
	~RF_CLineRDP();

public:
	struct SReaderRssiFromLineTag  // 2个接收器以 左下角开
	{
		float fRssiListP1[2];
		float fRssiListP2[2];
	};

public:
	SReaderRssiFromLineTag m_stDefaultInfo;

};




