#pragma once

#include "RF_Region.h"

#define LVSSM_LENGHT 31
#define LVSSM_LENGHT_REAL   20

struct RF_LineRegionAttr_s
{
	floatP P1;       //  P1 --------------------- P2
	floatP P2;

	UINT P1_ReaderID;  
	UINT P2_ReaderID;   

	UINT P1_TagID;
	UINT P2_TagID;
};

class CRF_LineRegion_M:public RF_CRegionBase, public RF_CLineRDP
{
public:
	CRF_LineRegion_M();

	int Region_Init(RF_LineRegionAttr_s);   // suc return 0
	bool Read_RegionAttr(RF_LineRegionAttr_s*);  // suc return true

	//Override
	int BuildVSS();    //500ms建立一次

	//Override
	int dB_to_XY(long nTagId, floatP* pData_out); 

	//Overeride
	int GetRssiList(long nTagId, float* pData_out);

	//Overeride
	int CheckRegionDevice();

// 	//Override
// 	void* ReadAttr();

	int Read_VSS(int style, float* pVSS, float* pMaxValue, float* pMinValue);  // return vss len

private:
	bool m_bAttrInit;
	RF_LineRegionAttr_s m_attr;

	bool m_bLVSSMBuilt;
	float oriLVSSM[LVSSM_LENGHT*2];
	float meanLVSSM[LVSSM_LENGHT*2];
	float maxValue;
	float minValue;

	int dB_to_XY_opt(const float* dB_V,const float* pLVSSM, floatP* pData_out);

};