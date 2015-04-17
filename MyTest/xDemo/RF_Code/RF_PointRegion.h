#pragma once

#include "RF_Region.h"

struct RF_PointRegionAttr_s
{

	floatP readerPoint;

	int readerId;
};

class CRF_PointRegion_M:public RF_CRegionBase
{
public:
	CRF_PointRegion_M();

	int SetRegionAttr(RF_PointRegionAttr_s attr);
	int GetRegionAttr(RF_PointRegionAttr_s* pData_out);

	//Override
	int BuildVSS();   

	//Override
	int dB_to_XY(long nTagId, floatP* pData_out); 

	//Overeride
	int GetRssiList(long nTagId, float* pData_out);

	//Overeride
	int CheckRegionDevice();

private:
	bool m_bAttrInit;

	RF_PointRegionAttr_s m_attr;

};