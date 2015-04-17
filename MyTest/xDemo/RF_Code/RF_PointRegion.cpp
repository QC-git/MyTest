#include "StdAfx.h"
#include "RF_Device.h"

#include "RF_PointRegion.h"

CRF_PointRegion_M::CRF_PointRegion_M()
{
	m_bAttrInit = false;

	memset(&m_attr, 0, sizeof(RF_PointRegionAttr_s));
}

int CRF_PointRegion_M::SetRegionAttr(RF_PointRegionAttr_s attr)
{
	if ( true == m_bAttrInit ) //只能设置一次
	{
		return -1;
	}

	if ( 0 == attr.readerId )
	{
		return -2;
	}

	m_attr = attr;
	m_bAttrInit = true;

	return 0;
}

int CRF_PointRegion_M::GetRegionAttr(RF_PointRegionAttr_s* pData_out)
{
	if ( false == m_bAttrInit || NULL == pData_out )
	{
		return -1;
	}

	(*pData_out) = m_attr;

	return 0;
}

int CRF_PointRegion_M::BuildVSS()
{
	return 0;
}

int CRF_PointRegion_M::dB_to_XY(long nTagId, floatP* pData_out)
{
	if ( false == m_bAttrInit || 0 == nTagId || NULL == pData_out )
	{
		return -1;
	}

	pData_out->x = m_attr.readerPoint.x;
	pData_out->y = m_attr.readerPoint.y;

	return 0;
}

int CRF_PointRegion_M::GetRssiList(long nTagId, float* pData_out)
{
	int point = 1;
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( 0 == nTagId || NULL == pData_out || NULL == pDev )
	{
		return -1;
	}

	float rssi = pDev->ReadRssi(m_attr.readerId, nTagId);
	if ( rssi <= 0 )
	{
		return -2;
	}

	//调整
	if ( rssi < 80 )
	{
		rssi += 5;
	}

	(*pData_out) = rssi;

	return point;
}

int CRF_PointRegion_M::CheckRegionDevice()
{
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( false == m_bAttrInit || NULL == pDev )
	{
		return -1;
	}


	if ( false == pDev->IsReaderNomal( m_attr.readerId ) )
	{
		//TRACE("\n CRF_PointRegion_M::CheckRegionDevice(), IsReaderNomal fail, readerId = %d", m_attr.readerId);

		return -2;
	}


	return 0;
}


