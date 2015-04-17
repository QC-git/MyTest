#pragma once

#include "RF_Region.h"
#include "RF_RectRegion.h"
#include "RF_LineRegion.h"
#include "RF_PointRegion.h"
#include "RF_MirrorRectRegion.h"

class CRF_RegionFactory_M
{
public:
	CRF_RegionFactory_M(void* dev);

	RF_CRegionBase* CreateRegion(RF_REGION_STYLE emRegionStyle);

	void DestroyRegion(RF_CRegionBase* pRegion);

private:
	int m_nRegionSN;

	void* m_dev;

	int GenReigonSN();
};