#include "stdafx.h"
#include "RF_Device.h"

#include "RF_LineRegion.h"

CRF_LineRegion_M::CRF_LineRegion_M()
{
	TRACE("\n CRF_LineRegion_M");

	m_bAttrInit = false;

	memset(&m_attr, 0, sizeof(RF_LineRegionAttr_s));

	m_bLVSSMBuilt = false;

	memset(oriLVSSM, 0, LVSSM_LENGHT*2);
	memset(meanLVSSM, 0, LVSSM_LENGHT*2);
	maxValue = 0;
	minValue = 0;
}

int CRF_LineRegion_M::Region_Init(RF_LineRegionAttr_s attr)
{
	if ( attr.P1.x == attr.P2.x && attr.P1.y == attr.P2.y   )
	{
		return -1;
	}

	if ( 0 == attr.P1_ReaderID ||
		 0 == attr.P2_ReaderID ||
		 attr.P1_ReaderID == attr.P2_ReaderID )
	{
		return -2;
	}

	if (!RF_CLineRDP::m_bIsUseDefaultRssi)
	{
		if ( 0 == attr.P1_TagID ||
			0 == attr.P2_TagID ||
			attr.P1_TagID == attr.P2_TagID )
		{
			return -3;
		}
	}

	m_attr = attr;
	m_bAttrInit = true;

	return 0;
}

bool CRF_LineRegion_M::Read_RegionAttr(RF_LineRegionAttr_s* pData_out)
{
	if (!m_bAttrInit)
	{
		return false;
	}

	*pData_out = m_attr;

	return true;
}

int CRF_LineRegion_M::BuildVSS()
{
	RF_CReaderManager* p = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	double dfRssiValue[2][2];
	
	if ( !m_bAttrInit || NULL == p )
	{
		return -1;
	}

	//获取强度值
	RF_LineRegionAttr_s* pAttr = &m_attr;

	if (RF_CLineRDP::m_bIsUseDefaultRssi)
	{
		RF_CLineRDP::SReaderRssiFromLineTag* pDefaultData = &m_stDefaultInfo;

		//m_adValue[a][b] : a = 接收器， b = 射频卡
		//以接收器为中心，获取该接收器各个距离的接收强度，计算矩阵，共2列线性矩阵
		dfRssiValue[0][0]=(double)pDefaultData->fRssiListP1[0];
		dfRssiValue[0][1]=(double)pDefaultData->fRssiListP1[1];

		dfRssiValue[1][0]=(double)pDefaultData->fRssiListP2[0]; 
		dfRssiValue[1][1]=(double)pDefaultData->fRssiListP2[1];	  	
	}
	else
	{
		if (true)
		{
			//m_adValue[a][b] : a = 射频卡， b = 接收器
			//以参考卡为中心，获取该参考卡各个距离的发送，计算矩阵，共2列线性矩阵
			dfRssiValue[0][0]=(double)p->ReadRssi( pAttr->P1_ReaderID, pAttr->P1_TagID );
			dfRssiValue[0][1]=(double)p->ReadRssi( pAttr->P2_ReaderID, pAttr->P1_TagID );

			dfRssiValue[1][0]=(double)p->ReadRssi( pAttr->P2_ReaderID, pAttr->P2_TagID ); 
			dfRssiValue[1][1]=(double)p->ReadRssi( pAttr->P1_ReaderID, pAttr->P2_TagID );	  	  

		}else
		{
			//m_adValue[a][b] : a = 接收器， b = 射频卡
			//以接收器为中心，获取该接收器各个距离的接收强度，计算矩阵，共2列线性矩阵
			dfRssiValue[0][0]=(double)p->ReadRssi( pAttr->P1_ReaderID, pAttr->P1_TagID );
			dfRssiValue[0][1]=(double)p->ReadRssi( pAttr->P1_ReaderID, pAttr->P2_TagID );

			dfRssiValue[1][0]=(double)p->ReadRssi( pAttr->P2_ReaderID, pAttr->P2_TagID ); 
			dfRssiValue[1][1]=(double)p->ReadRssi( pAttr->P2_ReaderID, pAttr->P1_TagID );	  	

		}
	}

	

	for (int i=0; i<2; i++){
		for(int j=0; j<2; j++)
		{
			if (dfRssiValue[i][j]<=0)
			{
				m_bLVSSMBuilt = false;
				return -2;
			}
	}   }

	//生成矩阵

	double tLVSSM[LVSSM_LENGHT*2];

	if ( 0 != LVSSM_opt(tLVSSM, dfRssiValue[0][0], dfRssiValue[0][1]) )
	{
		return -3;
	}
	for(int n=0; n<LVSSM_LENGHT; n++) //从左往右
	{
		oriLVSSM[n] = (float)tLVSSM[n];
		//TRACE("\n");
		//TRACE("%.2f\t", oriLVSSM[n]);
	}

	if ( 0 != LVSSM_opt(tLVSSM, dfRssiValue[1][0], dfRssiValue[1][1]) )
	{
		return -4;
	}
	for(int n=0; n<LVSSM_LENGHT; n++) //从右往左
		oriLVSSM[LVSSM_LENGHT+(LVSSM_LENGHT-n-1)] = (float)tLVSSM[n];

	//生成均值矩阵
	int n2;
	for (int n=0; n<LVSSM_LENGHT; n++)
	{
		n2 = LVSSM_LENGHT+(LVSSM_LENGHT-n-1);
		meanLVSSM[n] = ( oriLVSSM[n] + oriLVSSM[n2] )/2;
		meanLVSSM[n2] = meanLVSSM[n];
	}

	//获取最大最小值
	float tMax=0;
	float tMin=100;
	float temp;
	for(int n=0;n<2;n++)
	{
		for(int j=5;j<=25;j++)
		{
			temp=meanLVSSM[ n*LVSSM_LENGHT + j ];
			if( temp>tMax )	tMax=temp;
			if( temp<tMin )	tMin=temp;
		}
	}
	maxValue = tMax;
	minValue = tMin;

	m_bLVSSMBuilt = true;

	return 0;
}

// void* CRF_LineRegion_M::ReadAttr()
// {
// 	if (!m_bAttrInit)
// 	{
// 		return NULL;
// 	}
// 
// 	*pData_out = m_attr;
// 
// 	return *m_attr;
// }

int CRF_LineRegion_M::GetRssiList(long nTagId, float* pData_out)
{
	int point = 2;
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( 0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev )
	{
		TRACE("\n CRF_LineRegion_M::GetRssiList  0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev ");
		return -1;
	}

	float* pData = pData_out;
	pData[0] = pDev->ReadRssi(m_attr.P1_ReaderID, nTagId);
	pData[1] = pDev->ReadRssi(m_attr.P2_ReaderID, nTagId);
	if ( pData[0] <= 0 || pData[1] <= 0 )
	{
		//TRACE("\n CRF_LineRegion_M::GetRssiList pData[0] <= 0 || pData[1] <= 0");
		return -2;
	}

	return point;
}

int CRF_LineRegion_M::CheckRegionDevice()
{
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( false == m_bAttrInit || NULL == pDev )
	{
		return -1;
	}


	if ( false == pDev->IsReaderNomal( m_attr.P1_ReaderID ) || 
		false == pDev->IsReaderNomal( m_attr.P2_ReaderID ) )
	{
		TRACE("\n CRF_LineRegion_M::CheckRegionDevice fail");

		return -2;
	}


	return 0;
}

int CRF_LineRegion_M::Read_VSS(int style, float* pVSS, float* pMaxValue, float* pMinValue)
{
	if ( !m_bLVSSMBuilt || NULL == pVSS )
	{
		return -1;
	}


	int len = LVSSM_LENGHT*2;
	float* VSS;
	if ( 0 == style )
	{
		VSS = meanLVSSM;
	}else
	{
		VSS = oriLVSSM;
	}

	for(int n=0;n<len;n++)
	{
		pVSS[n]=VSS[n];
	}

	if (pMaxValue)
	{
		*pMaxValue = maxValue;
	}

	if (pMinValue)
	{
		*pMinValue = minValue;
	}

	return len;
}


int CRF_LineRegion_M::dB_to_XY(long nTagId, floatP* pData_out)
{
	RF_CReaderManager* pData = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( 0 == nTagId || NULL == pData || NULL == pData_out || false == m_bLVSSMBuilt )
	{
		return -1;
	}


	float* pLVSSM;
	if (1)
	{
		pLVSSM = meanLVSSM;
	}else
	{
		pLVSSM = oriLVSSM;
	}


	//获取数值
	float dB[2];
	dB[0]=pData->ReadRssi(m_attr.P1_ReaderID, nTagId); //获取射频卡在四个接收器下的数值
	dB[1]=pData->ReadRssi(m_attr.P2_ReaderID, nTagId);
	if ( dB[0] <= 0 || dB[1] <= 0 )
	{
		return -2;
	}

	//数值调整 
	//dB_Adjust

	//计算
	if ( 0 != dB_to_XY_opt(dB, pLVSSM, pData_out) )
	{
		TRACE("\n CRF_LineRegion_M::dB_to_XY   nTagId = %x, 0 != dB_to_XY_opt ", nTagId);
		return -3;
	}

	return 0;
}

int CRF_LineRegion_M::dB_to_XY_opt(const float* dB_V,const float* pLVSSM, floatP* pData_out)
{
	if ( NULL == dB_V || NULL == pLVSSM || NULL == pData_out )
	{
		return -1;
	}

	//计算方差
	double t_dfEdis[LVSSM_LENGHT];
	float t_dfEdis_par[2]; 
	for(int n=0; n<LVSSM_LENGHT; n++)
	{
		t_dfEdis_par[0] = pLVSSM[n];
		t_dfEdis_par[1] = pLVSSM[n+LVSSM_LENGHT];
		t_dfEdis[n] = Variance_opt(t_dfEdis_par, dB_V, 2);
	}

	//选取点
	double temp;
	const int WEIGHT_AMOUNT=4;   //选取最小的点数
	float waPoint[WEIGHT_AMOUNT];   //点方位
	float waData[WEIGHT_AMOUNT];     //强度
	float waWeight[WEIGHT_AMOUNT];       //权值

	for(int n=0;n<WEIGHT_AMOUNT;n++)
	{
		temp=10000;
		for(int i=0; i<31; i++)        //0,4   4,8     8,12    12,16
		{

			if( t_dfEdis[i]<temp)
			{
				temp=t_dfEdis[i]; 

				waPoint[n]=(float)i;
				waData[n]=(float)temp;

				t_dfEdis[i]=10000;

			}		

		}		
	}

	//求权值
	Weight_opt_FromVariance(waData, WEIGHT_AMOUNT, waWeight);

	//权值算相对原点距离
	double Xrst = 0;
	for (int n=0; n<WEIGHT_AMOUNT; n++)
	{
		Xrst += (waWeight[n]) * (waPoint[n]);
	}

	//转换为真实长度
	Xrst -= 5;
	if ( Xrst <0  )
		Xrst = 0;
	if ( Xrst > 20 )
		Xrst = 20;

	//转换为真实坐标
	floatP p1 = m_attr.P1;
	floatP p2 = m_attr.P2;
	floatP relP; 

// 	float d0_2 = ( p1.x - p2.x)*( p1.x - p2.x) + ( p1.y - p2.y)*( p1.y - p2.y);
// 	float d0 = sqrt(d0_2);  //line 真实长度
// 	if ( d0 <= 0 )
// 	{
// 		return -3;
// 	}
	float k = Xrst/20;  //虚拟长度 占 虚拟总长度的比例

	relP.x = p1.x + ( p2.x - p1.x )*k;
	relP.y = p1.y + ( p2.y - p1.y )*k;

	*pData_out = relP;
	return 0;
}