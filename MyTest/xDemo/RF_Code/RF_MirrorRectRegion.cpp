#include "StdAfx.h"
#include "RF_Device.h"

#include "RF_MirrorRectRegion.h"

CRF_MirrorRectRegion::CRF_MirrorRectRegion()
{
	TRACE("\n CRF_MirrorRectRegion");

	m_bAttrInit = false;
	memset(PID,0,sizeof(RF_ID_s)*4);
	memset(&m_Rect,0,sizeof(RF_Rect_s));

	m_bVSSbuilt = false;
	memset(m_adValue,0,sizeof(double)*PointNum*PointNum);
	memset(oriVSSM,0,sizeof(float)*PointNum*VSSM_Width*VSSM_Height);
	memset(meanVSSM,0,sizeof(float)*PointNum*VSSM_Width*VSSM_Height);
	m_maxValue = 0;
	m_minValue = 0;

	m_bFirstBuiltVSS = false;

	m_ReaderLayoutType = 0;

};

int CRF_MirrorRectRegion::SetAttr(RF_RectRegionAttr_s attr)
{
	if ( true == m_bAttrInit )  //只能初始化一次
	{
		TRACE("\n CRF_MirrorRectRegion::SetAttr true == m_bAttrInit");
		return -1;
	}

	if( attr.left<0 )
		attr.left=0;
	if( attr.top<0 )
		attr.top=0;
	if( attr.right<0 )
		attr.right=0;
	if( attr.bottom<0 )
		attr.bottom=0;
	if ( attr.right < attr.left || attr.bottom < attr.top )
	{
		TRACE("\n  CRF_MirrorRectRegion::SetAttr fail stPar.right < stPar.left || stPar.bottom < stPar.top ");
		return -2;
	}

	int nCount=0;    //接收器id不能为 0
	if (attr.ltReaderID)
		nCount++;
	if (attr.lbReaderID)
		nCount++;
	if (attr.rtReaderID)
		nCount++;
	if (attr.rbReaderID)
		nCount++;
	if ( 2 != nCount ) //镜像模式 只需两个 其它参数与rect相同
	{
		TRACE("\n  CRF_MirrorRectRegion::SetAttr fail 2 != nCount ");
		return -3;
	}
	if ( attr.lbReaderID && attr.rbReaderID )
	{
		m_ReaderLayoutType = READER_LAYOUT_TYPE1_LB_RB;
	}
	else if ( attr.rbReaderID && attr.rtReaderID )
	{
		m_ReaderLayoutType = READER_LAYOUT_TYPE2_RB_RT;
	}
	else if ( attr.rtReaderID && attr.ltReaderID )
	{
		m_ReaderLayoutType = READER_LAYOUT_TYPE3_RT_LT;
	}
	else if ( attr.ltReaderID && attr.lbReaderID )
	{
		m_ReaderLayoutType = READER_LAYOUT_TYPE4_LT_LB;
	}else
	{
		TRACE("\n  CRF_MirrorRectRegion::SetAttr return -4 ");
		return -4;
	}

	m_Rect.left=attr.left;
	m_Rect.top=attr.top;
	m_Rect.right=attr.right;
	m_Rect.bottom=attr.bottom;

	PID[0].Rea=attr.lbReaderID; //左下
	PID[1].Rea=attr.rbReaderID; //右下
	PID[2].Rea=attr.rtReaderID; //右上
	PID[3].Rea=attr.ltReaderID; //左上

	if (!RF_CRectRDP::m_bIsUseDefaultRssi)
	{
		if ( 0 == attr.ltTagID ||
			0 == attr.lbTagID ||
			0 == attr.rtTagID ||
			0 == attr.rbTagID )
		{
			TRACE("\n  CRF_MirrorRectRegion::SetAttr fail some tag = 0");
			return -5;
		}



		PID[0].Tag=attr.lbTagID; //左下
		PID[1].Tag=attr.rbTagID; //右下
		PID[2].Tag=attr.rtTagID; //右上
		PID[3].Tag=attr.ltTagID; //左上
	}

	m_bAttrInit = true;

	return 0;
}

int CRF_MirrorRectRegion::ReadAttr(RF_RectRegionAttr_s* pData_out)
{
	if ( false == m_bAttrInit )
	{
		TRACE("\n CRF_MirrorRectRegion::ReadAttr false == m_bAttrInit");
		return -1;
	}

	pData_out->left=m_Rect.left;
	pData_out->top=m_Rect.top;
	pData_out->right=m_Rect.right;
	pData_out->bottom=m_Rect.bottom;

	pData_out->ltReaderID=PID[3].Rea;
	pData_out->lbReaderID=PID[0].Rea;
	pData_out->rtReaderID=PID[2].Rea;
	pData_out->rbReaderID=PID[1].Rea;

	pData_out->ltTagID=PID[3].Tag;
	pData_out->lbTagID=PID[0].Tag;
	pData_out->rtTagID=PID[2].Tag;
	pData_out->rbTagID=PID[1].Tag;

	return 0;
}

int CRF_MirrorRectRegion::BuildVSS()
{
	RF_CReaderManager* p = (RF_CReaderManager*)RF_CRegionBase::GetDev();
	if ( NULL == p || false == m_bAttrInit )
	{
		//TRACE("\n CRF_MirrorRectRegion::BuildVSS fail  NULL == p || false == m_bAttrInit");
		return -1;
	}

	if (m_bIsUseDefaultRssi)  // 采用默认参数
	{
		RF_CRectRDP::SReaderRssiFromRectTag* pDefaultData = &m_stDefaultInfo;

		//m_adValue[a][b] : a = 接收器， b = 射频卡
		//以接收器为中心，获取该接收器各个距离的接收强度，计算矩阵，共四列矩阵
		m_adValue[0][0]=(double)pDefaultData->fRssiListP1[0];	//(0,0)  3 ----------- 2
		m_adValue[0][1]=(double)pDefaultData->fRssiListP1[1];	//(x,0)   |           |
		m_adValue[0][2]=(double)pDefaultData->fRssiListP1[2];	//(0,y)   |           |
		m_adValue[0][3]=(double)pDefaultData->fRssiListP1[3];	//(x,y)   |           |
		//        |___________|                                                                  	
		m_adValue[1][0]=(double)pDefaultData->fRssiListP2[0];  //       0             1
		m_adValue[1][1]=(double)pDefaultData->fRssiListP2[1];	  	
		m_adValue[1][2]=(double)pDefaultData->fRssiListP2[2];	  
		m_adValue[1][3]=(double)pDefaultData->fRssiListP2[3];	  

		m_adValue[2][0]=(double)pDefaultData->fRssiListP3[0]; 
		m_adValue[2][1]=(double)pDefaultData->fRssiListP3[1];
		m_adValue[2][2]=(double)pDefaultData->fRssiListP3[2];
		m_adValue[2][3]=(double)pDefaultData->fRssiListP3[3];

		m_adValue[3][0]=(double)pDefaultData->fRssiListP4[0];
		m_adValue[3][1]=(double)pDefaultData->fRssiListP4[1];
		m_adValue[3][2]=(double)pDefaultData->fRssiListP4[2];
		m_adValue[3][3]=(double)pDefaultData->fRssiListP4[3];
	}
	else
	{
		//-------------------------------------载入数值-----------------------------------------
		if ( READER_LAYOUT_TYPE1_LB_RB == m_ReaderLayoutType )
		{
			m_adValue[3][0] = m_adValue[0][0]=(double)p->ReadRssi( PID[0].Rea, PID[0].Tag );	//(0,0)  3 ----------- 2   
			m_adValue[3][1] = m_adValue[0][1]=(double)p->ReadRssi( PID[0].Rea, PID[1].Tag );	//(x,0)   |           |
			m_adValue[3][2] = m_adValue[0][2]=(double)p->ReadRssi( PID[0].Rea, PID[3].Tag );	//(0,y)   |           |
			m_adValue[3][3] = m_adValue[0][3]=(double)p->ReadRssi( PID[0].Rea, PID[2].Tag );	//(x,y)   |           |
			//        |___________|  	                                                                 	
			m_adValue[2][0] = m_adValue[1][0]=(double)p->ReadRssi( PID[1].Rea, PID[1].Tag );  //       0             1   
			m_adValue[2][1] = m_adValue[1][1]=(double)p->ReadRssi( PID[1].Rea, PID[0].Tag );	//  	
			m_adValue[2][2] = m_adValue[1][2]=(double)p->ReadRssi( PID[1].Rea, PID[2].Tag );	//       0             1  
			m_adValue[2][3] = m_adValue[1][3]=(double)p->ReadRssi( PID[1].Rea, PID[3].Tag );	  

		}

		else if ( READER_LAYOUT_TYPE2_RB_RT == m_ReaderLayoutType )
		{
			//(0,0)  3 ----------- 2  2 
			//(x,0)   |           |
			//(0,y)   |           |	
			//(x,y)   |           |
			//        |___________|  	                    
			//       0             1   1
			m_adValue[0][0] = m_adValue[1][0]=(double)p->ReadRssi( PID[1].Rea, PID[1].Tag ); 
			m_adValue[0][1] = m_adValue[1][1]=(double)p->ReadRssi( PID[1].Rea, PID[0].Tag );		
			m_adValue[0][2] = m_adValue[1][2]=(double)p->ReadRssi( PID[1].Rea, PID[2].Tag );	 
			m_adValue[0][3] = m_adValue[1][3]=(double)p->ReadRssi( PID[1].Rea, PID[3].Tag );	  

			m_adValue[3][0] = m_adValue[2][0]=(double)p->ReadRssi( PID[2].Rea, PID[2].Tag );   
			m_adValue[3][1] = m_adValue[2][1]=(double)p->ReadRssi( PID[2].Rea, PID[3].Tag );	 	
			m_adValue[3][2] = m_adValue[2][2]=(double)p->ReadRssi( PID[2].Rea, PID[1].Tag );	
			m_adValue[3][3] = m_adValue[2][3]=(double)p->ReadRssi( PID[2].Rea, PID[0].Tag );	 
		}

		else if ( READER_LAYOUT_TYPE3_RT_LT == m_ReaderLayoutType )                   
		{
			//       3             2
			//(0,0)  3 ----------- 2   
			//(x,0)   |           |
			//(0,y)   |           |
			//(x,y)   |           |
			//        |___________| 
			//       0             1  
			m_adValue[1][0] = m_adValue[2][0]=(double)p->ReadRssi( PID[2].Rea, PID[2].Tag );   
			m_adValue[1][1] = m_adValue[2][1]=(double)p->ReadRssi( PID[2].Rea, PID[3].Tag );	 	
			m_adValue[1][2] = m_adValue[2][2]=(double)p->ReadRssi( PID[2].Rea, PID[1].Tag );	
			m_adValue[1][3] = m_adValue[2][3]=(double)p->ReadRssi( PID[2].Rea, PID[0].Tag );	 

			m_adValue[0][0] = m_adValue[3][0]=(double)p->ReadRssi( PID[3].Rea, PID[3].Tag );	  
			m_adValue[0][1] = m_adValue[3][1]=(double)p->ReadRssi( PID[3].Rea, PID[2].Tag );	
			m_adValue[0][2] = m_adValue[3][2]=(double)p->ReadRssi( PID[3].Rea, PID[0].Tag );	
			m_adValue[0][3] = m_adValue[3][3]=(double)p->ReadRssi( PID[3].Rea, PID[1].Tag );	
		}

		else if ( READER_LAYOUT_TYPE4_LT_LB == m_ReaderLayoutType )
		{
			m_adValue[1][0] = m_adValue[0][0]=(double)p->ReadRssi( PID[0].Rea, PID[0].Tag );	  
			m_adValue[1][1] = m_adValue[0][1]=(double)p->ReadRssi( PID[0].Rea, PID[1].Tag );	
			m_adValue[1][2] = m_adValue[0][2]=(double)p->ReadRssi( PID[0].Rea, PID[3].Tag );
			m_adValue[1][3] = m_adValue[0][3]=(double)p->ReadRssi( PID[0].Rea, PID[2].Tag );

			m_adValue[2][0] = m_adValue[3][0]=(double)p->ReadRssi( PID[3].Rea, PID[3].Tag );	  
			m_adValue[2][1] = m_adValue[3][1]=(double)p->ReadRssi( PID[3].Rea, PID[2].Tag );	
			m_adValue[2][2] = m_adValue[3][2]=(double)p->ReadRssi( PID[3].Rea, PID[0].Tag );	
			m_adValue[2][3] = m_adValue[3][3]=(double)p->ReadRssi( PID[3].Rea, PID[1].Tag );	
		}else
		{
			TRACE("\n CRF_MirrorRectRegion::BuildVSS reader id error ");
			return -2;
		}
	}

	

	for (int i=0; i<4; i++){
		for(int j=0; j<4; j++)
		{
			if (m_adValue[i][j]<=0)
			{
				//TRACE("\n CRF_RectRegion_M::BuildVSS fail  m_adValue[i][j]<=0");
				return -3;
			}
		}	}

	//------------------------------生存标准矩阵--------------------------------------
	double* tVSSM=new double[31*31];
	if ( NULL == tVSSM )
	{
		TRACE("\n CRF_MirrorRectRegion::BuildVSS fail  NULL == tVSSM");
		return -4;
	}

	int nMul=VSSM_Width;
	int nMul2=VSSM_Width*VSSM_Height;

	VSS_opt( tVSSM,m_adValue[0][0],m_adValue[0][1],m_adValue[0][2],m_adValue[0][3] );           //Reader 0
	for(int i=0;i<=30;i++)
		for(int j=0;j<=30;j++)
		{
			oriVSSM[i*nMul+j]=(float)tVSSM[i*31+j];
		}

		VSS_opt( tVSSM,m_adValue[1][0],m_adValue[1][1],m_adValue[1][2],m_adValue[1][3] );           //Reader 1
		for(int i=0;i<=30;i++)
			for(int j=0;j<=30;j++)
			{
				oriVSSM[1*nMul2+i*nMul+(30-j)]=(float)tVSSM[i*31+j];
			}

			VSS_opt( tVSSM,m_adValue[2][0],m_adValue[2][1],m_adValue[2][2],m_adValue[2][3] );           //Reader 2
			for(int i=0;i<=30;i++)
				for(int j=0;j<=30;j++)
				{
					oriVSSM[2*nMul2+(30-i)*nMul+(30-j)]=(float)tVSSM[i*31+j];
				}

				VSS_opt( tVSSM,m_adValue[3][0],m_adValue[3][1],m_adValue[3][2],m_adValue[3][3] );           //Reader 3
				for(int i=0;i<=30;i++)
					for(int j=0;j<=30;j++)
					{
						oriVSSM[3*nMul2+(30-i)*nMul+j]=(float)tVSSM[i*31+j];
					}

					//-----------------------------产生均值矩阵--------------------------------------
					for(int i=0;i<=30;i++)
						for(int j=0;j<=30;j++)
						{
							meanVSSM[i*nMul+j]=( oriVSSM[i*nMul+j]+oriVSSM[1*nMul2+i*nMul+(30-j)]+oriVSSM[2*nMul2+(30-i)*nMul+(30-j)]+oriVSSM[3*nMul2+(30-i)*nMul+j] )/4;
							meanVSSM[1*nMul2+i*nMul+(30-j)]     =meanVSSM[i*nMul+j];
							meanVSSM[2*nMul2+(30-i)*nMul+(30-j)]=meanVSSM[i*nMul+j];
							meanVSSM[3*nMul2+(30-i)*nMul+j]     =meanVSSM[i*nMul+j];
						}

						//----------------------------求最大最小值---------------------------------------
						float tMax=0;
						float tMin=100;
						float temp;
						for(int n=0;n<4;n++)
						{
							for(int i=5;i<=25;i++)  //只去真实区域值
								for(int j=5;j<=25;j++)
								{
									temp=meanVSSM[ n*nMul2+i*nMul+j ];
									if( temp>tMax )	tMax=temp;
									if( temp<tMin )	tMin=temp;
								}
						}
						m_maxValue=tMax;
						m_minValue=tMin;

						if ( false == m_bVSSbuilt )
						{
							m_bVSSbuilt = true;
						}

						if (true && false == m_bFirstBuiltVSS )
						{
							m_bFirstBuiltVSS = true;

							int k1 = 31;
							int k2 = 31 * 31;

							TRACE("\n Mirror Rect ");
							TRACE("\n 1 ");

							for (int j = 30; j>=0; j--)
							{	
								TRACE("\n");
								for (int i=0; i<=30; i++)
								{
									TRACE("\t %.2f", meanVSSM[ j*k1 + i ]);
								}
							}

							TRACE("\n 2 ");

							for (int j = 30; j>=0; j--)
							{	
								TRACE("\n");
								for (int i=0; i<=30; i++)
								{
									TRACE("\t %.2f", meanVSSM[ k2*1 + j*k1 + i ]);
								}
							}

							TRACE("\n 3 ");

							for (int j = 30; j>=0; j--)
							{	
								TRACE("\n");
								for (int i=0; i<=30; i++)
								{
									TRACE("\t %.2f", meanVSSM[ k2*2 + j*k1 + i ]);
								}
							}

							TRACE("\n 4 ");

							for (int j = 30; j>=0; j--)
							{	
								TRACE("\n");
								for (int i=0; i<=30; i++)
								{
									TRACE("\t %.2f", meanVSSM[ k2*3 + j*k1 + i ]);
								}
							}
						}


						delete[] tVSSM;

						return 0;
}


int CRF_MirrorRectRegion::dB_to_XY(long nTagId, floatP* pData_out)
{
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( NULL == pDev || 0 == nTagId || NULL == pData_out )
	{
		TRACE("\n CRF_MirrorRectRegion::dB_to_XY NULL == pDev || 0 == nTagId || NULL == pData_out");
		return -1;
	}

	if ( false == m_bVSSbuilt )
	{
		return -2;
	}

	//选择矩阵
	float* pVSSM;
	if (1)     //采用均值矩阵
	{
		pVSSM=meanVSSM;
	}
	else
	{
		pVSSM=oriVSSM; //采用原始矩阵
	}

	//读取待测卡数值
	float dB[4];
	if ( false == GenerateSetOfMirrorRssi(nTagId, pDev, dB) ) //镜像一组数值
	{
		TRACE("\n false == GenerateSetOfMirrorRssi nTagId = %x", nTagId);
		return -3;
	}

	//数值调整
	dB_RectAdjust(dB, m_maxValue, m_minValue);

	//计算
	floatP relP = dB_to_XY_opt2(dB, pVSSM, VSSM_Width, VSSM_Height,RealWidth, RealHeight);

	//矫正的真实坐标
	relP = CorrectXY_ForRect(relP, m_Rect, RealWidth, RealHeight);

	(*pData_out) = relP;

	return 0;
}

int CRF_MirrorRectRegion::GetRssiList(long nTagId, float* pData_out)
{
	int point = 2;
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( 0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev )
	{
		TRACE("\n CRF_MirrorRectRegion::GetRssiList  0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev ");
		return -1;
	}

	float* pData = pData_out;
	switch(m_ReaderLayoutType)
	{
	case READER_LAYOUT_TYPE1_LB_RB:
		{
			pData[0] = pDev->ReadRssi(PID[0].Rea, nTagId);
			pData[1] = pDev->ReadRssi(PID[1].Rea, nTagId);
		}
		break;
	case READER_LAYOUT_TYPE2_RB_RT:
		{
			pData[0] = pDev->ReadRssi(PID[1].Rea, nTagId);
			pData[1] = pDev->ReadRssi(PID[2].Rea, nTagId);
		}
		break;
	case READER_LAYOUT_TYPE3_RT_LT:
		{
			pData[0] = pDev->ReadRssi(PID[2].Rea, nTagId);
			pData[1] = pDev->ReadRssi(PID[3].Rea, nTagId);
		}
		break;
	case READER_LAYOUT_TYPE4_LT_LB:
		{
			pData[0] = pDev->ReadRssi(PID[3].Rea, nTagId);
			pData[1] = pDev->ReadRssi(PID[0].Rea, nTagId);
		}
		break;
	default:
		TRACE("\n  CRF_MirrorRectRegion::GetRssiList return -2");
		return -2;
	}

	if ( pData[0] <= 0 || pData[1] <= 0 )
	{
		//TRACE("\n  CRF_MirrorRectRegion::GetRssiList pData[0] <= 0 || pData[1] <= 0");
		return -3;
	}
	
	return point;
}

int CRF_MirrorRectRegion::CheckRegionDevice()
{
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( false == m_bAttrInit || NULL == pDev )
	{
		return -1;
	}

	for (int n=0; n<4; n++)
	{
		if ( false == pDev->IsReaderNomal( PID[n].Rea ) )
		{
			TRACE("\n CRF_MirrorRectRegion::CheckRegionDevice fail");

			return -2;
		}
	}

	return 0;
}

int CRF_MirrorRectRegion::Read_VSS(int style, float* pVSS, float* pMaxValue, float* pMinValue)
{
	if ( false == m_bVSSbuilt || NULL == pVSS )
	{
		TRACE("\n CRF_MirrorRectRegion::Read_VSS false == m_bVSSbuilt || NULL == pVSS");
		return -1;
	}

	int len = PointNum*VSSM_Width*VSSM_Height;
	float* VSS;
	if ( 0 == style )
	{
		VSS = meanVSSM;
	}else
	{
		VSS = oriVSSM;
	}

	for(int n=0;n<len;n++)
	{
		pVSS[n]=VSS[n];
	}

	if (pMaxValue)
	{
		*pMaxValue = m_maxValue;
	}

	if (pMinValue)
	{
		*pMinValue = m_minValue;
	}

	return len;
}

bool CRF_MirrorRectRegion::GenerateSetOfMirrorRssi(long nTag, PVOID pRssiData, float* pRssi, float fCoe/* =1.0f */)
{
	if (pRssiData==NULL)
	{
		return FALSE;
	}
	RF_CReaderManager* pData= (RF_CReaderManager*)pRssiData;

	float dB[4];
	if ( PID[0].Rea!=0 && PID[1].Rea!=0 )
	{
		float P2[3],P3[3];

		dB[0]=pData->ReadRssi(PID[0].Rea, nTag);
		dB[1]=pData->ReadRssi(PID[1].Rea, nTag);

		dB[2]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe);
		dB[3]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P2[0]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe);   //  0
		P2[1]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe); 
		P2[2]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		P3[0]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);   //  3
		P3[1]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe); 
		P3[2]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		dB[2]=(P2[0]+P2[1]+P2[2])/3;
		dB[3]=(P3[0]+P3[1]+P3[2])/3;
	}
	else if ( PID[1].Rea!=0 && PID[2].Rea!=0 )
	{
		float P0[3],P3[3];

		dB[1]=pData->ReadRssi(PID[1].Rea, nTag);
		dB[2]=pData->ReadRssi(PID[2].Rea, nTag);

		dB[0]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe);
		dB[3]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P0[0]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe);   //  0
		P0[1]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe); 
		P0[2]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		P3[0]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);   //  3
		P3[1]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe); 
		P3[2]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		dB[0]=(P0[0]+P0[1]+P0[2])/3;
		dB[3]=(P3[0]+P3[1]+P3[2])/3;
	}
	else if ( PID[2].Rea!=0 && PID[3].Rea!=0 )
	{
		float P0[3],P1[3];

		dB[2]=pData->ReadRssi(PID[2].Rea, nTag);
		dB[3]=pData->ReadRssi(PID[3].Rea, nTag);

		dB[0]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe);
		dB[1]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P0[0]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe);   //  0
		P0[1]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe); 
		P0[2]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		P1[0]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);   //  3
		P1[1]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe); 
		P1[2]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		dB[0]=(P0[0]+P0[1]+P0[2])/3;
		dB[1]=(P1[0]+P1[1]+P1[2])/3;
	}
	else if ( PID[3].Rea!=0 && PID[0].Rea!=0 )
	{
		float P1[3],P2[3];

		dB[3]=pData->ReadRssi(PID[3].Rea, nTag);
		dB[0]=pData->ReadRssi(PID[0].Rea, nTag);

		dB[1]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe);
		dB[2]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P1[0]=GetMirrorRssi_opt( dB[2], m_minValue, m_maxValue, 20, 2, fCoe);   //  0
		P1[1]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20, 2, fCoe); 
		P1[2]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		P2[0]=GetMirrorRssi_opt( dB[3], m_minValue, m_maxValue, 20, 2, fCoe);   //  3
		P2[1]=GetMirrorRssi_opt( dB[1], m_minValue, m_maxValue, 20, 2, fCoe); 
		P2[2]=GetMirrorRssi_opt( dB[0], m_minValue, m_maxValue, 20*1.414f, 2, fCoe); 

		dB[1]=(P1[0]+P1[1]+P1[2])/3;
		dB[2]=(P2[0]+P2[1]+P2[2])/3;
	}


	for (int n=0; n<4; n++)
	{
		if (dB[n]<=0)
		{
			return FALSE;
		}
		pRssi[n]=dB[n];
	}

	return TRUE;
}

