#include "StdAfx.h"
#include "RF_Device.h"

#include "RF_RectRegion.h"

CRF_RectRegion_M::CRF_RectRegion_M()
{
	TRACE("\n CRF_RectRegion_M");

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

};

int CRF_RectRegion_M::SetAttr(RF_RectRegionAttr_s attr)
{
	if ( true == m_bAttrInit )  //ֻ�ܳ�ʼ��һ��
	{
		TRACE("\n CRF_RectRegion_M::SetAttr true == m_bAttrInit");
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
		TRACE("\n  CRF_RectRegion_M::SetAttr fail stPar.right < stPar.left || stPar.bottom < stPar.top ");
		return -2;
	}

	m_Rect.left=attr.left;
	m_Rect.top=attr.top;
	m_Rect.right=attr.right;
	m_Rect.bottom=attr.bottom;


	int nCount=0;    //������id����Ϊ 0
	if (attr.ltReaderID)
		nCount++;
	if (attr.lbReaderID)
		nCount++;
	if (attr.rtReaderID)
		nCount++;
	if (attr.rbReaderID)
		nCount++;
	if ( 4 != nCount ) 
	{
		TRACE("\n  CRF_RectRegion_M::SetAttr fail 4 != nCount ");
		return -3;
	}

	PID[0].Rea=attr.lbReaderID; //����
	PID[1].Rea=attr.rbReaderID; //����
	PID[2].Rea=attr.rtReaderID; //����
	PID[3].Rea=attr.ltReaderID; //����

	if (!RF_CRectRDP::m_bIsUseDefaultRssi)
	{
		if ( 0 == attr.ltTagID ||
			0 == attr.lbTagID ||
			0 == attr.rtTagID ||
			0 == attr.rbTagID )
		{
			TRACE("\n  CRF_RectRegion_M::SetAttr fail some tag = 0");
			return -4;
		}

		PID[0].Tag=attr.lbTagID; //����
		PID[1].Tag=attr.rbTagID; //����
		PID[2].Tag=attr.rtTagID; //����
		PID[3].Tag=attr.ltTagID; //����
	}

	m_bAttrInit = true;

	return 0;
}

int CRF_RectRegion_M::ReadAttr(RF_RectRegionAttr_s* pData_out)
{
	if ( false == m_bAttrInit )
	{
		TRACE("\n CRF_RectRegion_M::ReadAttr false == m_bAttrInit");
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

int CRF_RectRegion_M::BuildVSS()
{
	RF_CReaderManager* p = (RF_CReaderManager*)RF_CRegionBase::GetDev();
	if ( NULL == p || false == m_bAttrInit )
	{
		//TRACE("\n CRF_RectRegion_M::BuildVSS fail  NULL == p || false == m_bAttrInit");
		return -1;
	}

	if (m_bIsUseDefaultRssi)  // ����Ĭ�ϲ���
	{
		RF_CRectRDP::SReaderRssiFromRectTag* pDefaultData = &m_stDefaultInfo;

		//m_adValue[a][b] : a = �������� b = ��Ƶ��
		//�Խ�����Ϊ���ģ���ȡ�ý�������������Ľ���ǿ�ȣ�������󣬹����о���
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
	else //�����������
	{
		if (true)
		{
			//m_adValue[a][b] : a = ��Ƶ���� b = ������
			//�Բο���Ϊ���ģ���ȡ�òο�����������ķ��ͣ�������󣬹����о���
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
			//m_adValue[a][b] : a = �������� b = ��Ƶ��
			//�Խ�����Ϊ���ģ���ȡ�ý�������������Ľ���ǿ�ȣ�������󣬹����о���
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


	for (int i=0; i<4; i++){
		for(int j=0; j<4; j++)
		{
			if (m_adValue[i][j]<=0)
			{
				//TRACE("\n CRF_RectRegion_M::BuildVSS fail  m_adValue[i][j]<=0");
				return -2;
			}
		}	}

	//------------------------------�����׼����--------------------------------------
	double* tVSSM=new double[31*31];
	if ( NULL == tVSSM )
	{
		TRACE("\n CRF_RectRegion_M::BuildVSS fail  NULL == tVSSM");
		return -3;
	}

	int nMul=VSSM_Width;
	int nMul2=VSSM_Width*VSSM_Height;

	VSS_opt( tVSSM,m_adValue[0][0],m_adValue[0][1],m_adValue[0][2],m_adValue[0][3] );           //Reader 0
	for(int i=0;i<=30;i++){
		for(int j=0;j<=30;j++){
			oriVSSM[i*nMul+j]=(float)tVSSM[i*31+j];
		}
	}

	VSS_opt( tVSSM,m_adValue[1][0],m_adValue[1][1],m_adValue[1][2],m_adValue[1][3] );           //Reader 1
	for(int i=0;i<=30;i++){
		for(int j=0;j<=30;j++){
			oriVSSM[1*nMul2+i*nMul+(30-j)]=(float)tVSSM[i*31+j];
		}
	}

	VSS_opt( tVSSM,m_adValue[2][0],m_adValue[2][1],m_adValue[2][2],m_adValue[2][3] );           //Reader 2
	for(int i=0;i<=30;i++){
		for(int j=0;j<=30;j++){
			oriVSSM[2*nMul2+(30-i)*nMul+(30-j)]=(float)tVSSM[i*31+j];
		}
	}

	VSS_opt( tVSSM,m_adValue[3][0],m_adValue[3][1],m_adValue[3][2],m_adValue[3][3] );           //Reader 3
	for(int i=0;i<=30;i++){
		for(int j=0;j<=30;j++){
			oriVSSM[3*nMul2+(30-i)*nMul+j]=(float)tVSSM[i*31+j];
		}
	}

	//-----------------------------������ֵ����--------------------------------------
	for(int i=0;i<=30;i++){
		for(int j=0;j<=30;j++){
			meanVSSM[i*nMul+j]=( oriVSSM[i*nMul+j]+oriVSSM[1*nMul2+i*nMul+(30-j)]+oriVSSM[2*nMul2+(30-i)*nMul+(30-j)]+oriVSSM[3*nMul2+(30-i)*nMul+j] )/4;
			meanVSSM[1*nMul2+i*nMul+(30-j)]     =meanVSSM[i*nMul+j];
			meanVSSM[2*nMul2+(30-i)*nMul+(30-j)]=meanVSSM[i*nMul+j];
			meanVSSM[3*nMul2+(30-i)*nMul+j]     =meanVSSM[i*nMul+j];
		}
	}

	//----------------------------�������Сֵ---------------------------------------
	float tMax=0;
	float tMin=100;
	float temp;
	for(int n=0;n<4;n++){
		for(int i=5;i<=25;i++){  //ֻȥ��ʵ����ֵ
			for(int j=5;j<=25;j++){
				temp=meanVSSM[ n*nMul2+i*nMul+j ];
				if( temp>tMax )	tMax=temp;
				if( temp<tMin )	tMin=temp;
			}
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

		TRACE("\n Rect ");
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


int CRF_RectRegion_M::dB_to_XY(long nTagId, floatP* pData_out)
{
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( NULL == pDev || 0 == nTagId || NULL == pData_out )
	{
		TRACE("\n CRF_RectRegion_M::dB_to_XY NULL == pDev || 0 == nTagId || NULL == pData_out");
		return -1;
	}

	if ( false == m_bVSSbuilt )
	{
		return -2;
	}

	//ѡ�����
	float* pVSSM;
	if (1)     //���þ�ֵ����
	{
		pVSSM=meanVSSM;
	}
	else
	{
		pVSSM=oriVSSM; //����ԭʼ����
	}

	//��ȡ���⿨��ֵ
	float dB[4];
	dB[0]=pDev->ReadRssi(PID[0].Rea, nTagId); //��ȡ��Ƶ�����ĸ��������µ���ֵ
	dB[1]=pDev->ReadRssi(PID[1].Rea, nTagId);
	dB[2]=pDev->ReadRssi(PID[2].Rea, nTagId);
	dB[3]=pDev->ReadRssi(PID[3].Rea, nTagId);

	for (int n=0; n<4; n++)
	{
		if (dB[n]<=0)
		{
			return -3;
		}
	}

	dB_RectAdjust(dB, m_maxValue, m_minValue);

	//����
	floatP relP = dB_to_XY_opt2(dB, pVSSM, VSSM_Width, VSSM_Height,RealWidth, RealHeight);

	//��������ʵ����
	relP = CorrectXY_ForRect(relP, m_Rect, RealWidth, RealHeight);

	(*pData_out) = relP;

	return 0;
}

int CRF_RectRegion_M::GetRssiList(long nTagId, float* pData_out)
{
	int point = 4;
	RF_CReaderManager* pDev = (RF_CReaderManager*)RF_CRegionBase::GetDev();

	if ( 0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev )
	{
		TRACE("\n CRF_RectRegion_M::GetRssiList  0 == nTagId || NULL == pData_out || false == m_bAttrInit || NULL == pDev ");
		return -1;
	}

	float* pData = pData_out;
	for (int n=0; n<point; n++)
	{
		pData[n] = pDev->ReadRssi(PID[0].Rea, nTagId);
		if ( pData[n] <= 0 )
		{
			//TRACE("\n CRF_RectRegion_M::GetRssiList  pData[%d] <= 0 ", n);
			return -2;
		}
	}

	return point;
}

int CRF_RectRegion_M::CheckRegionDevice()
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
			TRACE("\n CRF_RectRegion_M::CheckRegionDevice fail");

			return -2;
		}
	}

	return 0;
}

int CRF_RectRegion_M::Read_VSS(int style, float* pVSS, float* pMaxValue, float* pMinValue)
{
	if ( false == m_bVSSbuilt || NULL == pVSS )
	{
		TRACE("\n CRF_RectRegion_M::Read_VSS false == m_bVSSbuilt || NULL == pVSS");
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

