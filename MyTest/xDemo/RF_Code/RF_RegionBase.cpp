#include "stdafx.h"
#include "RF_RegionBase .h"

#include <math.h>

RF_CReaderManager* g_pRfDev=NULL;

bool Load_RfDevice(HANDLE hDevice)
{
	if (hDevice)
	{
		g_pRfDev=(RF_CReaderManager*)hDevice;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////

CRF_RectRegionBase_M::CRF_RectRegionBase_M()
{
	m_bIsIDInit=false;
	m_bIsRectInit=false;

	memset(PID,0,sizeof(RF_ID_s)*4);
	memset(oriVSSM,0,sizeof(float)*PointNum*VSSM_Width*VSSM_Height);
	memset(meanVSSM,0,sizeof(float)*PointNum*VSSM_Width*VSSM_Height);

	m_bIsVSSM_Built=false;

	m_IsMirror=true;

}

bool CRF_RectRegionBase_M::Region_Init(RF_RectRegionAttr_s stPar)
{
	bool bIsRectInit=false;
	bool bIsReaderInit=false;
	bool bIsTagInit=false;

	if( stPar.left<0 )
		stPar.left=0;
	if( stPar.top<0 )
		stPar.top=0;
	if( stPar.right<0 )
		stPar.right=0;
	if( stPar.bottom<0 )
		stPar.bottom=0;
	if ( stPar.right < stPar.left || stPar.bottom < stPar.top )
	{
		TRACE("\n CRF_RectRegion_M::Region_Init fail stPar.right < stPar.left || stPar.bottom < stPar.top ");
		return false;
	}
	bIsRectInit=true;

	int nCount=0;
	if (stPar.ltReaderID)
		nCount++;
	if (stPar.lbReaderID)
		nCount++;
	if (stPar.rtReaderID)
		nCount++;
	if (stPar.rbReaderID)
		nCount++;
	if ( !m_IsMirror && nCount==4 ) 
	{
		bIsReaderInit=true;
	}
	if ( m_IsMirror && nCount>=2 ) //镜像模式 只需两个
	{
		bIsReaderInit=true;
	}
	
	if ( stPar.ltTagID && stPar.lbTagID && stPar.rtTagID && stPar.rbTagID )
	{
		bIsTagInit=true;
	}

	//--------------------------------------------------------

	if ( bIsRectInit && bIsReaderInit && bIsTagInit )
	{
		m_Rect.left=stPar.left;
		m_Rect.top=stPar.top;
		m_Rect.right=stPar.right;
		m_Rect.bottom=stPar.bottom;
	
		PID[0].Rea=stPar.lbReaderID; //左下
		PID[1].Rea=stPar.rbReaderID; //右下
		PID[2].Rea=stPar.rtReaderID; //右上
		PID[3].Rea=stPar.ltReaderID; //左上

		PID[0].Tag=stPar.lbTagID; //左下
		PID[1].Tag=stPar.rbTagID; //右下
		PID[2].Tag=stPar.rtTagID; //右上
		PID[3].Tag=stPar.ltTagID; //左上
		
		m_bIsRectInit=true;
		return TRUE;
	}

	return FALSE;

}

bool CRF_RectRegionBase_M::Read_RegionAttr(RF_RectRegionAttr_s* pData_out)
{
	if ( !m_bIsRectInit )
	{
		return FALSE;
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

	return TRUE;

}

//  [12/15/2013 qcy]
bool CRF_RectRegionBase_M::Build_VSSM(PVOID pRssiData, bool bMirror/* Mode=false */)    //当为true时，采用镜像生成法，相应的精度降低
{
	RF_CReaderManager* p=g_pRfDev;
	//const CrossRssiMax=60;

	if ( NULL == p )
	{
		return FALSE;
	}

	if ( false == bMirror )
	{
		if (!m_bIsRectInit)
		{
			return FALSE;
		}

		//-------------------------------------载入数值-----------------------------------------
		if (true)
		{
			//m_adValue[a][b] : a = 射频卡， b = 接收器
			//以参考卡为中心，获取该参考卡各个距离的发送，计算矩阵，共四列矩阵
			m_adValue[0][0]=(double)p->ReadRssi( PID[0].Rea, PID[0].Tag);	//(0,0)  3 ----------- 2
			m_adValue[0][1]=(double)p->ReadRssi( PID[1].Rea, PID[0].Tag);	//(x,0)   |           |
			m_adValue[0][2]=(double)p->ReadRssi( PID[3].Rea, PID[0].Tag);	//(0,y)   |           |
			m_adValue[0][3]=(double)p->ReadRssi( PID[2].Rea, PID[0].Tag);	//(x,y)   |           |
			//        |___________|                                                                  	
			m_adValue[1][0]=(double)p->ReadRssi( PID[1].Rea, PID[1].Tag);  //       0             1
			m_adValue[1][1]=(double)p->ReadRssi( PID[0].Rea, PID[1].Tag);	  	
			m_adValue[1][2]=(double)p->ReadRssi( PID[2].Rea, PID[1].Tag);	  
			m_adValue[1][3]=(double)p->ReadRssi( PID[3].Rea, PID[1].Tag);	  

			m_adValue[2][0]=(double)p->ReadRssi( PID[2].Rea, PID[2].Tag );
			m_adValue[2][1]=(double)p->ReadRssi( PID[3].Rea, PID[2].Tag );
			m_adValue[2][2]=(double)p->ReadRssi( PID[1].Rea, PID[2].Tag );
			m_adValue[2][3]=(double)p->ReadRssi( PID[0].Rea, PID[2].Tag );

			m_adValue[3][0]=(double)p->ReadRssi( PID[3].Rea, PID[3].Tag );
			m_adValue[3][1]=(double)p->ReadRssi( PID[2].Rea, PID[3].Tag );
			m_adValue[3][2]=(double)p->ReadRssi( PID[0].Rea, PID[3].Tag );
			m_adValue[3][3]=(double)p->ReadRssi( PID[1].Rea, PID[3].Tag );
		}else
		{
			//m_adValue[a][b] : a = 接收器， b = 射频卡
			//以接收器为中心，获取该接收器各个距离的接收强度，计算矩阵，共四列矩阵
			m_adValue[0][0]=(double)p->ReadRssi( PID[0].Rea, PID[0].Tag);	//(0,0)  3 ----------- 2
			m_adValue[0][1]=(double)p->ReadRssi( PID[0].Rea, PID[1].Tag);	//(x,0)   |           |
			m_adValue[0][2]=(double)p->ReadRssi( PID[0].Rea, PID[3].Tag);	//(0,y)   |           |
			m_adValue[0][3]=(double)p->ReadRssi( PID[0].Rea, PID[2].Tag);	//(x,y)   |           |
			//        |___________|                                                                  	
			m_adValue[1][0]=(double)p->ReadRssi( PID[1].Rea, PID[1].Tag);  //       0             1
			m_adValue[1][1]=(double)p->ReadRssi( PID[1].Rea, PID[0].Tag);	  	
			m_adValue[1][2]=(double)p->ReadRssi( PID[1].Rea, PID[2].Tag);	  
			m_adValue[1][3]=(double)p->ReadRssi( PID[1].Rea, PID[3].Tag);	  

			m_adValue[2][0]=(double)p->ReadRssi( PID[2].Rea, PID[2].Tag );
			m_adValue[2][1]=(double)p->ReadRssi( PID[2].Rea, PID[3].Tag );
			m_adValue[2][2]=(double)p->ReadRssi( PID[2].Rea, PID[1].Tag );
			m_adValue[2][3]=(double)p->ReadRssi( PID[2].Rea, PID[0].Tag );

			m_adValue[3][0]=(double)p->ReadRssi( PID[3].Rea, PID[3].Tag );
			m_adValue[3][1]=(double)p->ReadRssi( PID[3].Rea, PID[2].Tag );
			m_adValue[3][2]=(double)p->ReadRssi( PID[3].Rea, PID[1].Tag );
			m_adValue[3][3]=(double)p->ReadRssi( PID[3].Rea, PID[0].Tag );
		}
		
	}

	else if ( true == bMirror ) //采用镜像法
	{
		//-------------------------------------载入数值-----------------------------------------
		if ( PID[0].Rea!=NULL &&  PID[1].Rea!=NULL )
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

		else if ( PID[1].Rea!=NULL &&  PID[2].Rea!=NULL )
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

		else if ( PID[2].Rea!=NULL &&  PID[3].Rea!=NULL )                   
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

		else if ( PID[3].Rea!=NULL &&  PID[0].Rea!=NULL )
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
			return FALSE;
		}

	}

	for (int i=0; i<4; i++)
		for(int j=0; j<4; j++)
		{
			if (m_adValue[i][j]<=0)
			{
				m_bIsVSSM_Built=false;
				return FALSE;
			}
		}

	//  调整矩阵参数
// 	if( bMirror==true )
// 	{
// 		for(int j=0; j<4; j++)
// 			for(int i=1;i<4; i++)
// 			{
// 				if (m_adValue[j][i]<=53)
// 				{
// 					m_adValue[j][i]=(53-m_adValue[j][i])*2/3+m_adValue[j][i];
// 				}
// 			}
// 	}
	

	//------------------------------生存标准矩阵--------------------------------------
	double* tVSSM=new double[31*31];
	if ( NULL == tVSSM )
	{
		TRACE("\n  NULL == tVSSM ");
		return FALSE;
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
		for(int n=0;n<4;n++)
		{
			for(int i=5;i<=25;i++)  //只去真实区域值
				for(int j=5;j<=25;j++)
				{
					float temp=meanVSSM[ n*nMul2+i*nMul+j ];
					if( temp>tMax )	tMax=temp;
					if( temp<tMin )	tMin=temp;
				}
		}
		m_Par.Max=tMax;
		m_Par.Min=tMin;

		m_bIsVSSM_Built=true;
		delete[] tVSSM;
		return TRUE;
	
}

bool CRF_RectRegionBase_M::Read_VSSM(int nVSSMStyle, float* pVSSM, float* pMax, float* pMin)
{
	if ( nVSSMStyle<0 || nVSSMStyle>1 || !m_bIsVSSM_Built )
	{
		return FALSE;
	}

	if (nVSSMStyle==0)
	{
		int Num=PointNum*VSSM_Width*VSSM_Height;
		for(int n=0;n<Num;n++)
		{
			pVSSM[n]=oriVSSM[n];
		}
	}
	else if (nVSSMStyle==1)
	{
		int Num=PointNum*VSSM_Width*VSSM_Height;
		for(int n=0;n<Num;n++)
		{
			pVSSM[n]=meanVSSM[n];
		}
	}

	*pMax=m_Par.Max;
	*pMin=m_Par.Min;

	return TRUE;

}

bool CRF_RectRegionBase_M::GetRssiList(long nTag, PVOID pRssiData, float* pOutRssi, bool IsMirror /* =false */ )
{
	RF_CReaderManager* p=g_pRfDev;
// 	bool bIndex=p->CheckTagID(nTag);
// 	if (!bIndex)
// 	{
// 		return FALSE;
// 	}

	int Num=PointNum;
	double* tRssi=new double[Num];
	if (IsMirror==false)
	{
		for (int n=0; n<Num; n++)
		{
			tRssi[n]=(double)( p->ReadRssi(PID[n].Rea, nTag) );
		}
	}
	else
	{
		tRssi[0]=(double)( p->ReadRssi(PID[0].Rea, nTag) );
		tRssi[1]=(double)( p->ReadRssi(PID[1].Rea, nTag) );
		tRssi[2]=(double)( p->ReadRssi(PID[2].Rea, nTag) );;
		tRssi[3]=(double)( p->ReadRssi(PID[3].Rea, nTag) );;

	}
	
	for (int n=0; n<Num; n++)
	{
		if (tRssi[n]<=0)
		{
			tRssi[n]=0;
		}
	}
	MakeSort(tRssi,Num, ASCENDING, POSITIVENUMBER_MODE);

	for (int n=0; n<Num; n++)
	{
		pOutRssi[n]=(float)tRssi[n];
	}

	delete[] tRssi;
	return TRUE;


}

//  [12/15/2013 qcy]  potected
void CRF_RectRegionBase_M::dB_Adjust(float* dB_V)//强度 逻辑调整
{
	float *(dB[4]);    //排序，1到4分别是从小到大
	float *tdB;    //排序，1到4分别是从小到大
	dB[0]=dB_V;
	dB[1]=(dB_V+1);
	dB[2]=(dB_V+2);
	dB[3]=(dB_V+3);

	int i,j;
	i=0;j=0;
	for(i=0;i<3;i++)
		for(j=i+1;j<4;j++)
		{
			if( *(dB[j]) < *(dB[i]) )	
			{
				tdB=dB[i];
				dB[i]=dB[j];
				dB[j]=tdB;
			}
		}

		float temp;
		float MAX=m_Par.Max;
		{
			if( *(dB[0])>MAX )
			{
				temp=*(dB[0])-MAX;
				*(dB[0])-=temp;	*(dB[1])-=temp;*(dB[2])-=temp;*(dB[3])-=temp;
			}
			else if( *(dB[1])>MAX )
			{
				temp=*(dB[1])-MAX;
				*(dB[0])-=temp/2;*(dB[1])-=temp;*(dB[2])-=temp;*(dB[3])-=temp;
			}
			else if( *(dB[2])>MAX )	
			{
				temp=*(dB[2])-MAX;
				*(dB[0])-=temp/3;*(dB[1])-=temp/3;*(dB[2])-=temp;*(dB[3])-=temp;
			}
			else if( *(dB[3])>MAX )	
			{
				temp=*(dB[3])-MAX;
				*(dB[0])-=temp/4;*(dB[1])-=temp/4;*(dB[2])-=temp/4;*(dB[3])-=temp;
			}
		}
		float MIN=m_Par.Min;
		if( *(dB[1])<MIN )
		{
			temp=MIN-*(dB[1]);
			*(dB[0])+=temp;*(dB[1])+=temp;*(dB[2])+=temp/3;*(dB[3])+=temp/3;
		} 
		else if( *(dB[0])<MIN )
		{
			temp=MIN-*(dB[0]);
			*(dB[0])+=temp;*(dB[1])+=temp/4;*(dB[2])+=temp/4;*(dB[3])+=temp/4;
		} 

}

floatP CRF_RectRegionBase_M::dB_to_XY_opt(const float* dB_V,const float* pVSSM)
	//matrix  real
{
	double temp;
	floatP relP;
	relP.x=-1;
	relP.y=-1;

	int La=VSSM_Width;        //31 
	int Lb=VSSM_Height;       //31
	int nMul=VSSM_Width;     //乘数
	int nMul2=VSSM_Height*VSSM_Width;  //二级乘数

	double* tEdis=new double[La*Lb];        //20*20   
	if (tEdis==NULL)
	{
		return relP;
	}

	//------------------------------1:选择区域,可跳过-----------------------------
	//-------------------------2：区域内计算---------------------------------
	int RangeX1=0;
	int RangeX2=La;
	int RangeY1=0;   //强制全局比较
	int RangeY2=Lb;
	float fValue[4];

	//计算方差
	for(int j=RangeY1;j<RangeY2;j++)            //0,5   5,10   ,10,15   15,20
		for(int i=RangeX1;i<RangeX2;i++)        //0,4   4,8     8,12    12,16
		{
			fValue[0]=pVSSM[j*nMul+i];
			fValue[1]=pVSSM[j*nMul+i+1*nMul2];
			fValue[2]=pVSSM[j*nMul+i+2*nMul2];
			fValue[3]=pVSSM[j*nMul+i+3*nMul2];
			tEdis[j*nMul+i]=Variance_opt(fValue, dB_V, 4);   //计算 31*31个点内的方差
		}

	//----------------------------3：选择差异性最小的数个点------------------------------
	const int WEIGHT_AMOUNT=16;   //选取最小的点数
	int Nwa=WEIGHT_AMOUNT;

	struct WeightAmountToXY_TM   // TM:临时模型
	{
		floatP Point[WEIGHT_AMOUNT];   //点方位
		float Data[WEIGHT_AMOUNT];     //强度
		float w[WEIGHT_AMOUNT];       //权值
	};
	
	WeightAmountToXY_TM* pWATXY=new WeightAmountToXY_TM;
	if (pWATXY==NULL)
	{
		return relP;
	}
	memset(pWATXY,0,sizeof(WeightAmountToXY_TM));

	for(int n=0;n<Nwa;n++)
	{
		temp=10000;
		for(int j=RangeY1;j<RangeY2;j++)            //0,5   5,10   ,10,15   15,20
			for(int i=RangeX1;i<RangeX2;i++)        //0,4   4,8     8,12    12,16
			{
				
				if(tEdis[j*nMul+i]<temp)
				{
					temp=tEdis[j*nMul+i]; 
					pWATXY->Point[n].x=(float)i;
					pWATXY->Point[n].y=(float)j;
					pWATXY->Data[n]=(float)temp;
					tEdis[ j*nMul+i ]=10000;

				}		
				
			}		
	}
	//------------------------4：求权值----------------------------
	float* pWeight=new float[Nwa];
	Weight_opt_FromVariance(pWATXY->Data, Nwa, pWeight);

	double Xrst=0;    //result
	double Yrst=0;   
	for(int i=0;i<Nwa;i++)        //通过权值算坐标
	{
		Xrst+= pWeight[i] *( pWATXY->Point[i].x);
		Yrst+= pWeight[i] *( pWATXY->Point[i].y);
	}

	delete[] pWeight;
	//---------------5：转换 30*30 to 20*20 -------------
	int Wost=(VSSM_Width-RealWidth-1)/2;              //offset
	int Lost=(VSSM_Height-RealHeight-1)/2;

	if(Xrst<Wost)	Xrst=Wost;
	if(Yrst<Lost)   Yrst=Lost;
	if(Xrst>(RealWidth+Wost))	Xrst=RealWidth+Wost;
	if(Yrst>(RealHeight+Lost))	Yrst=RealHeight+Lost;
	Xrst-=Wost;               
	Yrst-=Lost;

	//---------------------6：删除内存-----------------------------
	delete[] tEdis;
	delete pWATXY;

	relP.x=(float)Xrst;
	relP.y=(float)Yrst;
	return relP;

}

bool CRF_RectRegionBase_M::GenerateSetOfMirrorRssi(long nTag, PVOID pRssiData, float* pRssi, float fCoe/* =1.0f */)
{
	if (pRssiData==NULL)
	{
		return FALSE;
	}
	RF_CReaderManager* pData=g_pRfDev;

	float dB[4];
	if ( PID[0].Rea!=0 && PID[1].Rea!=0 )
	{
		float P2[3],P3[3];

		dB[0]=pData->ReadRssi(PID[0].Rea, nTag);
		dB[1]=pData->ReadRssi(PID[1].Rea, nTag);

		dB[2]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe);
		dB[3]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P2[0]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  0
		P2[1]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P2[2]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		P3[0]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  3
		P3[1]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P3[2]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		dB[2]=(P2[0]+P2[1]+P2[2])/3;
		dB[3]=(P3[0]+P3[1]+P3[2])/3;
	}
	else if ( PID[1].Rea!=0 && PID[2].Rea!=0 )
	{
		float P0[3],P3[3];

		dB[1]=pData->ReadRssi(PID[1].Rea, nTag);
		dB[2]=pData->ReadRssi(PID[2].Rea, nTag);

		dB[0]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe);
		dB[3]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P0[0]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  0
		P0[1]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P0[2]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		P3[0]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  3
		P3[1]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P3[2]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		dB[0]=(P0[0]+P0[1]+P0[2])/3;
		dB[3]=(P3[0]+P3[1]+P3[2])/3;
	}
	else if ( PID[2].Rea!=0 && PID[3].Rea!=0 )
	{
		float P0[3],P1[3];

		dB[2]=pData->ReadRssi(PID[2].Rea, nTag);
		dB[3]=pData->ReadRssi(PID[3].Rea, nTag);

		dB[0]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe);
		dB[1]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P0[0]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  0
		P0[1]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P0[2]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		P1[0]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  3
		P1[1]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P1[2]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		dB[0]=(P0[0]+P0[1]+P0[2])/3;
		dB[1]=(P1[0]+P1[1]+P1[2])/3;
	}
	else if ( PID[3].Rea!=0 && PID[0].Rea!=0 )
	{
		float P1[3],P2[3];

		dB[3]=pData->ReadRssi(PID[3].Rea, nTag);
		dB[0]=pData->ReadRssi(PID[0].Rea, nTag);

		dB[1]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe);
		dB[2]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe);

		//-------------------------------------------------------------------------------
		P1[0]=GetMirrorRssi_opt( dB[2], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  0
		P1[1]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P1[2]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

		P2[0]=GetMirrorRssi_opt( dB[3], m_Par.Min, m_Par.Max, 20, 2, fCoe);   //  3
		P2[1]=GetMirrorRssi_opt( dB[1], m_Par.Min, m_Par.Max, 20, 2, fCoe); 
		P2[2]=GetMirrorRssi_opt( dB[0], m_Par.Min, m_Par.Max, 20*1.414f, 2, fCoe); 

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

// int CRF_RectRegion_M::CheckRegionReaderIsNomal(PVOID pReaderData)
// {
// 	CRF_ReaderControl_M* p=(CRF_ReaderControl_M*)pReaderData;
// 
// 	int relNum=0;
// 	for (int n=0; n<4; n++)
// 	{
// 		if ( p->CheckReaderIsNomal(PID[n].Rea) )
// 		{
// 			relNum++;
// 		}
// 	}
// 	return relNum;
// 
// }

int CRF_RectRegionBase_M::IncompleteState_opt(long lTag, PVOID pRssiData, PVOID pReaderData, floatP* Pout, bool bIsMirrorRect)
{
	RF_CReaderManager* pData=g_pRfDev;
	//CRF_ReaderControl_M* pReader=(CRF_ReaderControl_M*)pReaderData;

	int nNomalReaderNum=0;
	float tMin=100,temp;
	int tRoom=0;
	for (int n=0; n<4; n++)
	{
		if ( pData->IsReaderNomal(PID[n].Rea) )
		{
			temp=pData->ReadRssi(PID[n].Rea, lTag);
			if (temp<=0)
			{
				continue;
			}
			nNomalReaderNum++;

			if (temp<tMin)
			{
				tMin=temp;
				tRoom=n+1;
			}
		}
	}

	if (nNomalReaderNum==4)
	{
		return nNomalReaderNum;
	}

	floatP relP;
	relP.x=-1;
	relP.y=-1;
	if ( !bIsMirrorRect )
	{
		switch(tRoom)
		{
		case 1:relP.x=5;   relP.y=5;  break;
		case 2:relP.x=15;  relP.y=5;  break;
		case 3:relP.x=15;  relP.y=15; break;
		case 4:relP.x=5;   relP.y=15; break;
		default:break;
		}
	}
	else
	{
		switch(tRoom)
		{
// 		case 2:relP.x=10;  relP.y=0;  break;
// 		case 3:relP.x=10;  relP.y=20; break;
		case 1:relP.x=4;   relP.y=4;  break;
		case 2:relP.x=16;  relP.y=4;  break;
		case 3:relP.x=16;  relP.y=16; break;
		case 4:relP.x=4;   relP.y=16; break;
		default:break;
		}
	}
	
	(*Pout)=relP;
	return nNomalReaderNum;

}

//  [12/15/2013 qcy]
floatP CRF_RectRegionBase_M::dB_to_XY(long nTag, PVOID pRssiData, bool MirrorMode/* =false */, int AlgMode/* =0 */)
{
	floatP relP;
	relP.x=-1;
	relP.y=-1;
	if ( !m_bIsVSSM_Built ||
		 0 == nTag || NULL == pRssiData )
	{
		return relP;
	}
	RF_CReaderManager* pData=g_pRfDev;

	float* pVSSM;
	if (AlgMode==0)     //采用均值矩阵
	{
		pVSSM=meanVSSM;
	}
	else
	{
		pVSSM=oriVSSM;   //采用原始矩阵
	}

	float dB[4];
	if ( MirrorMode==false )
	{
		dB[0]=pData->ReadRssi(PID[0].Rea, nTag); //获取射频卡在四个接收器下的数值
		dB[1]=pData->ReadRssi(PID[1].Rea, nTag);
		dB[2]=pData->ReadRssi(PID[2].Rea, nTag);
		dB[3]=pData->ReadRssi(PID[3].Rea, nTag);

		for (int n=0; n<4; n++)
		{
			if (dB[n]<=0)
			{
				return relP;
			}
		}
	}
	else
	{
		bool bSuc=GenerateSetOfMirrorRssi(nTag, pRssiData, dB); //镜像一组数值
		if ( !bSuc )
		{
			return relP;
		}
	}

	dB_Adjust(dB); //对数值进行过滤盒调整

	relP=dB_to_XY_opt(dB,oriVSSM); //计算数值

	return relP;
}

floatP CRF_RectRegionBase_M::CorrectXY(floatP Point)
{
	if (Point.x<0 || Point.y<0)
	{
		return Point;
	}

	RF_Rect_s Rect=m_Rect;
	float kW=(float)( Rect.right-Rect.left  )/RealWidth;
	float kH=(float)( Rect.bottom-Rect.top )/RealHeight;
	Point.x= m_Rect.left + Point.x*kW;
	Point.y= m_Rect.top  + Point.y*kH;

	return Point;
}

floatP CRF_RectRegionBase_M::dB_to_XY2(long nTag, PVOID pRssiData, PVOID pReaderData, bool MirrorMode/* =false */, int AlgMode/* =0 */)
{
	floatP relP;
	relP.x=-1;
	relP.y=-1;

	int nReaderNomalNum=IncompleteState_opt(nTag, pRssiData, pReaderData, &relP, MirrorMode);
	if (  !MirrorMode && nReaderNomalNum!=4 )
	{
		return relP;
	}
	if ( MirrorMode && nReaderNomalNum!=2 )
	{
		return relP;
	}
	if ( !m_bIsVSSM_Built )
	{
		return relP;
	}

	floatP* pRefP1=new floatP[4];
	floatP* pRefP2=new floatP[4];
    floatP UnkP=dB_to_XY(nTag, pRssiData, MirrorMode, AlgMode);     // unknown P
	pRefP1[0]=dB_to_XY(PID[0].Tag, pRssiData, MirrorMode, AlgMode);
	pRefP1[1]=dB_to_XY(PID[1].Tag, pRssiData, MirrorMode, AlgMode);
	pRefP1[2]=dB_to_XY(PID[2].Tag, pRssiData, MirrorMode, AlgMode);
	pRefP1[3]=dB_to_XY(PID[3].Tag, pRssiData, MirrorMode, AlgMode);
	for (int n=0; n<4; n++)
	{
		if (pRefP1[n].x<0 || pRefP1[n].y<0)
		{
			delete[] pRefP1;
			delete[] pRefP2;
			return relP;
		}
	}
	if (UnkP.x<0 || UnkP.y<0)
	{
		delete[] pRefP1;
		delete[] pRefP2;
		return UnkP;
	}

	//---------------------------------------------------------------------------
	pRefP2[0].x=0;   pRefP2[0].y=0;
	pRefP2[1].x=20;  pRefP2[1].y=0; 
	pRefP2[2].x=20;  pRefP2[2].y=20;
	pRefP2[3].x=0;   pRefP2[3].y=20;

	float* pVar=new float[4];
	float dB[4], tdB[4];
	GenerateSetOfMirrorRssi(nTag, pRssiData, dB);

	GenerateSetOfMirrorRssi(PID[0].Tag, pRssiData, tdB);
	pVar[0]=Variance_opt(dB, tdB, 4);
	GenerateSetOfMirrorRssi(PID[1].Tag, pRssiData, tdB);
	pVar[1]=Variance_opt(dB, tdB, 4);
	GenerateSetOfMirrorRssi(PID[2].Tag, pRssiData, tdB);
	pVar[2]=Variance_opt(dB, tdB, 4);
	GenerateSetOfMirrorRssi(PID[3].Tag, pRssiData, tdB);
	pVar[3]=Variance_opt(dB, tdB, 4);

	float* pWeight=new float[4];
	Weight_opt_FromVariance(pVar, 4, pWeight);

	float temp;
	floatP UnkP_D_value;
	UnkP_D_value.x=0;
	UnkP_D_value.y=0;
	for (int n=0; n<4; n++)
	{
		temp=pRefP1[n].x-pRefP2[n].x;
		UnkP_D_value.x+=temp*pWeight[n];

		temp=pRefP1[n].y-pRefP2[n].y;
		UnkP_D_value.y+=temp*pWeight[n];

	}

	delete[] pVar;
	delete[] pWeight;

	UnkP.x=UnkP.x-UnkP_D_value.x;
	UnkP.y=UnkP.y-UnkP_D_value.y;
	if (UnkP.x<0)
	{
		UnkP.x=0;
	}
	if (UnkP.y<0)
	{
		UnkP.y=0;
	}

	relP=UnkP;

	delete[] pRefP1;
	delete[] pRefP2;
	return relP;


}