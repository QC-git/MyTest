#include "StdAfx.h"

#include "RF_RegionFactory.h"

CRF_RegionFactory_M::CRF_RegionFactory_M(void* dev )
{
	TRACE("\n CRF_RegionFactory");
	if ( NULL == dev )
	{
		TRACE("\n CRF_RegionFactory error NULL == dev ");
		return;
	}

	m_dev =dev;

	m_nRegionSN = 0;

}

RF_CRegionBase* CRF_RegionFactory_M::CreateRegion(RF_REGION_STYLE emRegionStyle)
{
	TRACE("\n CRF_RegionFactory_M::CreateRegion emRegionStyle = %d", emRegionStyle);

	RF_CRegionBase* pRegion = NULL;

	if ( NULL == m_dev )
	{
		TRACE("\n CRF_RegionFactory_M::CreateRegion NULL == m_dev");
		return NULL;
	}

	switch(emRegionStyle)
	{
	case RF_REGION_STYLE_RECT:
		pRegion = new CRF_RectRegion_M;
		break;
	case RF_REGION_STYLE_LINE:
		pRegion = new CRF_LineRegion_M;
		break;
	case RF_REGION_STYLE_POINT:
		pRegion = new CRF_PointRegion_M;
		break;
	case RF_REGION_STYLE_RECT_MIRROR:
		pRegion = new CRF_MirrorRectRegion;
		break;
	default:
		TRACE("\n CRF_RegionFactory_M::CreateRegion no find style");
		return NULL;
	}

	if ( NULL == pRegion )
	{
		TRACE("\n CRF_RegionFactory_M::CreateRegion NULL == pRegion");
		return NULL;
	}

	int nSN = GenReigonSN();
	TRACE("\n CRF_RegionFactory_M::CreateRegion nSN = %d, pRegion = %d", nSN, pRegion);

	pRegion->SetId(nSN);
	pRegion->SetStyle(emRegionStyle);
	pRegion->BandDev(m_dev);
	pRegion->SetMode(0);

	return pRegion;
}

void CRF_RegionFactory_M::DestroyRegion(RF_CRegionBase* pRegion)
{
	if ( NULL == pRegion )
	{
		TRACE("\n CRF_RegionFactory_M::DestroyRegion NULL == pRegion");
		return;
	}


	int nStyle = pRegion->GetStyle();
	int nSN = pRegion->GetId();
	TRACE("\n CRF_RegionFactory_M::DestroyRegion nSN = %d, nStyle = %d", nSN, nStyle);

	switch(nStyle)
	{
	case RF_REGION_STYLE_RECT:
		delete (CRF_RectRegion_M*)pRegion;
		return;
	case RF_REGION_STYLE_LINE:
		delete (CRF_LineRegion_M*)pRegion;
		return;
	case RF_REGION_STYLE_POINT:
		delete (CRF_PointRegion_M*)pRegion;
		return;
	case RF_REGION_STYLE_RECT_MIRROR:
		delete (CRF_MirrorRectRegion*)pRegion;
		return;
	default:
		TRACE("\n CRF_RegionFactory_M::DestroyRegion error! no find style");
		return ;
	}

}

int CRF_RegionFactory_M::GenReigonSN()
{
	return ++m_nRegionSN;
}
