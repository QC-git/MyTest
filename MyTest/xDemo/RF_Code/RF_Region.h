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

	//��������豸�Ƿ�����
	virtual int CheckRegionDevice() = 0;

	/*
		��ȡһ�� rssi ǿ��ֵ ���ظ�����Ӧ�벻ͬ�������н�����������Ӧ ������
	*/
	virtual int GetRssiList(long nTagId, float* pData_out) = 0; 

	//virtual int ReadAttr(void* pAttr_out) = 0;

private:
	int m_id;  // >0

	void* m_dev; // != NULL

	RF_REGION_STYLE m_style; // 1 - 3

	int m_mode;  //����ģʽ�������趨
};

class RF_CRegionDefaultParameters  // �����еĲ���
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

class RF_CRectRDP: public RF_CRegionDefaultParameters  // �����������еĲ���  ���ھ�������;�������
{
public:
	RF_CRectRDP();
	~RF_CRectRDP();

	//void CopyDefault();

	//��������ȡ�������ĸ�����ο����Ĳ����� �����γ�vss
	//
	struct SReaderRssiFromRectTag  // �ĸ��������� ���½ǿ�ʼ����ʱ���ĸ���
	{
		float fRssiListP1[4];
		float fRssiListP2[4];
		float fRssiListP3[4];
		float fRssiListP4[4];
	};
public:
	SReaderRssiFromRectTag m_stDefaultInfo;
};

class RF_CLineRDP: public RF_CRegionDefaultParameters   // �����������еĲ���  
{
public:
	RF_CLineRDP();
	~RF_CLineRDP();

public:
	struct SReaderRssiFromLineTag  // 2���������� ���½ǿ�
	{
		float fRssiListP1[2];
		float fRssiListP2[2];
	};

public:
	SReaderRssiFromLineTag m_stDefaultInfo;

};




