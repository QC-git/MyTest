#include "StdAfx.h"

#include "RF_Region.h"

RF_CRegionBase::RF_CRegionBase()
{
	m_id = 0;

	m_dev = NULL;

	m_style = RF_REGION_STYLE_NONE;
}

int RF_CRegionBase::SetId(int id)
{
	if ( id <= 0 )
	{
		TRACE("\n CRF_Region_M::SetId id <= 0");
		return -1;
	}

	m_id = id;

	return 0;
}

int RF_CRegionBase::GetId()
{
	return m_id;
}

int RF_CRegionBase::BandDev(void* dev)
{
	if ( NULL == dev )
	{
		TRACE("\n CRF_Region_M::BandDev NULL == dev");
		return -1;
	}

	m_dev = dev;

	return 0;
}

void* RF_CRegionBase::GetDev()
{
	return m_dev;
}


int RF_CRegionBase::SetMode(int mode)
{
	m_mode = mode;
	return 0;
}

int RF_CRegionBase::GetMode()
{
	return m_mode;
}


///////////////////////////RF_CRegionDefaultParameters////////////////////////////

RF_CRectRDP::RF_CRectRDP()
{
	m_stDefaultInfo.fRssiListP1[0] = 40;
	m_stDefaultInfo.fRssiListP1[1] = 60;
	m_stDefaultInfo.fRssiListP1[2] = 60;
	m_stDefaultInfo.fRssiListP1[3] = 75;

	m_stDefaultInfo.fRssiListP2[0] = 40;
	m_stDefaultInfo.fRssiListP2[1] = 60;
	m_stDefaultInfo.fRssiListP2[2] = 60;
	m_stDefaultInfo.fRssiListP2[3] = 75;

	m_stDefaultInfo.fRssiListP3[0] = 40;
	m_stDefaultInfo.fRssiListP3[1] = 60;
	m_stDefaultInfo.fRssiListP3[2] = 60;
	m_stDefaultInfo.fRssiListP3[3] = 75;

	m_stDefaultInfo.fRssiListP4[0] = 40;
	m_stDefaultInfo.fRssiListP4[1] = 60;
	m_stDefaultInfo.fRssiListP4[2] = 60;
	m_stDefaultInfo.fRssiListP4[3] = 75;
}

RF_CRectRDP::~RF_CRectRDP()
{
	;
}

///////////////////////////RF_CRegionDefaultParameters////////////////////////////


RF_CLineRDP::RF_CLineRDP()
{
	m_stDefaultInfo.fRssiListP1[0] = 40;
	m_stDefaultInfo.fRssiListP1[1] = 60;

	m_stDefaultInfo.fRssiListP2[0] = 40;
	m_stDefaultInfo.fRssiListP2[1] = 60;
}

RF_CLineRDP::~RF_CLineRDP()
{
	;
}