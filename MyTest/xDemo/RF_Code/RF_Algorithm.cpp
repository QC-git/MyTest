#include "StdAfx.h"
#include "RF_Algorithm.h"

//To obtain the coordinates of a time difference
floatP GetNextPoint(floatP src, 
	                floatP dst, 
					float Speed, 
					int Time_ms, 
					int MaxDifDistance, 
					int Mode)   // Mode1:���������ƶ������
{
	int nMaxDifDistance=MaxDifDistance;
	float fInterval=Speed*(float)Time_ms/1000;
	float XX=dst.x-src.x;  //��ֵ
	float YY=dst.y-src.y;


	if (Mode==1)
	{
		if ( abs(XX)>nMaxDifDistance || abs(YY)>nMaxDifDistance )
		{
			src.x=dst.x;
			src.y=dst.y;
			return src;
		}

		if ( XX>fInterval )
		{
			src.x+=fInterval;
		}
		else if ( XX>0 )
		{
			src.x=dst.x;
		}
		else if ( (-XX)>fInterval )
		{
			src.x-=fInterval;
		}
		else if ( (-XX)>0 )
		{
			src.x=dst.x;
		}

		if ( YY>fInterval )
		{
			src.y+=fInterval;
		}
		else if ( YY>0 )
		{
			src.y=dst.y;
		}
		else if ( (-YY)>fInterval )
		{
			src.y-=fInterval;
		}
		else if ( (-YY)>0 )
		{
			src.y=dst.y;
		}
	}
	
	if (Mode==2)
	{
		;
	}

	floatP relP=src;
	return relP;

}

// 0: is not in the obstacle or not converted 1: in the obstacle, and the successful conversion
bool OutTheObstacle_opt(double *dX,
	                    double *dY,
						void* Obstacle,
						int Shape)
{
	if(Shape==1)
	{
		CRectangle_model *p=(CRectangle_model* )Obstacle;
		double oriX,oriY,La,Lb;
		double X0,Y0,Y1;
		oriX=p->oriX;
		oriY=p->oriY;
		La=p->broadwise;
		Lb=p->endwise;

		if( *dX<=oriX || *dX>=(oriX+La) || *dY<=oriY || *dY>=(oriY+Lb) )
		{
			return false;
		}
		if( La<=Lb )
		{
			if( (*dX<=(oriX+La/2)) )				
			{
				X0=oriX;
				Y0=oriY;
				Y1=Y0+(*dX-X0);
				if( *dY<Y1 )	
				{
					*dY=oriY;
					return true;
				}
				else if( *dY==Y1 )        //�߽紦��
				{
					*dX=oriX;
					*dY=oriY;
					return true;
				}
				X0=oriX;
				Y0=oriY+Lb;
				Y1=Y0-(*dX-X0);
				if( *dY>Y1 )
				{
					*dY=oriY+Lb;
					return true;
				}
				else if( *dY==Y1 )
				{
					*dX=oriX;
					*dY=oriY+Lb;
					return true;
				}
				*dX=oriX;          
				return true;
			}
			else
			{
				X0=oriX+La/2;
				Y0=oriY+La/2;
				Y1=Y0-(*dX-X0);
				if( *dY<Y1 )	
				{
					*dY=oriY;
					return true;
				}
				if( *dY==Y1 )
				{
					*dX=oriX+La;
					*dY=oriY;
					return true;
				}
				X0=oriX+La/2;
				Y0=oriY+Lb-La/2;
				Y1=Y0+(*dX-X0);
				if( *dY>Y1 )
				{
					*dY=oriY+Lb;
					return true;
				}
				if( *dY==Y1 )
				{
					*dX=oriX+La;
					*dY=oriY+Lb;
					return true;
				}
				*dX=oriX+La;
				return true;
			}
		}
	}
	return false;
}

//A square matrix, in one corner as the starting point, can be set to expand the width, the matrix is logarithmic
void VSS_opt(double *pVSS,
	         double Vori ,
			 double Vx,
			 double Vy,
			 double Vxy)
{
	double xx,yy,xy,pp;
	double temp;
	double ex,ey,exy;
	//double p[nLenOfSide][nLenOfSide]=pVSS;
	//Vori=37;
	int nLenOfSide=20;  //��ʵ���� ��λ����
	int nExLen=5;   //�ӳ���
	int nHigh=1;    //�߶ȵ� >= 1

	const int k=nLenOfSide+nExLen*2+1;  //�ܵ���
	double z=(double)nHigh;       //�߶�
	double *pMatrix = new double[k*k];//10*i+j  //��ʱ����

	// v1 = v0 + aLog(d1)
	// v2 = v0 + aLog(d2)
	// v2 - v1 = aLog(d2/d1)
	// �� d1 = 1 ʱ�� v2 - v1 = aLog(d2);
	// a = (v2 - v1) / Log(d2);
	// ex = a;

	ey=(Vy-Vori)/log10((double)nLenOfSide);  // ���� x�� log����
	ex=(Vx-Vori)/log10((double)nLenOfSide);  // ���� y�� log����
	exy=(Vxy-Vori)/log10(sqrt(2.00)*nLenOfSide); //���� xy�� log����

	//double dd;
	for(int n=nExLen+1;n<=nLenOfSide+nExLen*2;n++)  //��������һ�㿪ʼ ����ԭ��������б��
	{

		xx=(double)(n-nExLen);  //x�� ���ԭ�㳤��
		yy=(double)(n-nExLen);  //y�� ���ԭ�㳤��
		xy=xx*xx+yy*yy;

		temp=Vori+ex*0.5*log10(xx*xx+z*z);  //��ʽ  v1 + 1/2 * e * log10(distance)
		pMatrix[(0+nExLen)*k+n]=temp;

		temp=Vori+ey*0.5*log10( yy*yy+z*z );
		pMatrix[n*k+(0+nExLen)]=temp;

		temp=Vori+exy*0.5*log10( xy+z*z );
		pMatrix[n*k+n]=temp;
	}
	temp=(Vori*3+0.5*(ex+ey+exy)*log10(z*z))/3; //����ԭ��ƽ��ֵ
	pMatrix[(0+nExLen)*k+(0+nExLen)]=temp;

	for(int i=nExLen;i<=nLenOfSide+nExLen*2;i++){   //ͨ����ʵ��б�� ������б���ڲ���ֵ
		for(int j=nExLen;j<=nLenOfSide+nExLen*2;j++)
		{
			if( i==j || i==nExLen || j==nExLen)  continue;
			if( i>j )
			{
				pp=(pMatrix[i*k+i]-pMatrix[i*k+nExLen])/(double)(i-nExLen); //����б��
				temp=pMatrix[i*k+nExLen]+pp*(j-nExLen);
			}
			else
			{
				pp=(pMatrix[j*k+j]-pMatrix[nExLen*k+j])/(double)(j-nExLen);
				temp=pMatrix[nExLen*k+j]+pp*(i-nExLen);
			}
			pMatrix[i*k+j]=temp;
	}	}

	for(int i=0;i<nExLen;i++)	                 //���� ԭ�������Է�������
		for(int j=0;j<nExLen;j++)	
			pMatrix[i*k+j]=pMatrix[(nExLen*2-i)*k+nExLen*2-j];
	for(int i=0;i<nExLen;i++)                     //���� ��������ֵ
		for(int j=nExLen;j<=nLenOfSide+nExLen*2;j++)
			pMatrix[i*k+j]=pMatrix[(nExLen*2-i)*k+j];
	for(int i=nExLen;i<=nLenOfSide+nExLen*2;i++)  //���� ��������ֵ
		for(int j=0;j<nExLen;j++)
			pMatrix[i*k+j]=pMatrix[i*k+nExLen*2-j]; 
	for(int n=0;n<k*k;n++)  //������ϸ�ֵ
		pVSS[n]=pMatrix[n];
	delete[] pMatrix; //�ͷ��ڴ�
}

//Computing and virtual matrix and the output X, Y coordinates, coordinate range can be specified matrix range
void dB_to_XY_opt(float* dB_V,
	              double* pX,
				  double* pY,
				  float(*Mtx)[31][31],
				  int MtxW,
				  int MtxL,
				  int RelW,
				  int RelL)  //matrix  real
{
	double temp;
	//const int nMtxRdr=4;       //   matrix reader  Num
	int La=MtxW+1;
	int Lb=MtxL+1;

	float (*pVSS)[31][31]=Mtx;

	float dB_A=*dB_V;
	float dB_B=*(dB_V+1);
	float dB_C=*(dB_V+2);
	float dB_D=*(dB_V+3);
	double* tEdis=new double[La*Lb];        //20*20    
	int RangeX1,RangeX2,RangeY1,RangeY2;

	//------------------------------1:ѡ������,������-----------------------------
/*	int temp_x=3,temp_y=3;
	int num_i,num_j;
	temp=10000;      //��һ���ϴ�ֵ��Ϊ�Ƚ�
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<6;j++)
		{	
			num_i=3+5*i;
			num_j=3+5*j;
			tEdis[i*La+j]=pow((pVSS[0][num_i][num_j]-dB_A),2)+pow((pVSS[1][num_i][num_j]-dB_B),2)+pow((pVSS[2][num_i][num_j]-dB_C),2)+pow((pVSS[3][num_i][num_j]-dB_D),2);  //�󷽲�
			if(tEdis[i*La+j]<temp)
			{
				temp=tEdis[i*La+j];
				temp_x=num_i;
				temp_y=num_j;

			}
		}
	}
	RangeY1=temp_y-6;if(RangeY1<0)  RangeY1=0;
	RangeY2=temp_y+5;if(RangeY2>30) RangeY2=30;
	RangeX1=temp_x-6;if(RangeX1<0)  RangeX1=0;
	RangeX2=temp_x+5;if(RangeX2>30) RangeX2=30;
	*/
	//-------------------------2�������ڼ���---------------------------------
	RangeY1=0;   //ǿ��ȫ�ֱȽ�
	RangeY2=MtxL;
	RangeX1=0;
	RangeX2=MtxW;
	for(int j=RangeY1;j<=RangeY2;j++)            //0,5   5,10   ,10,15   15,20
		for(int i=RangeX1;i<=RangeX2;i++)        //0,4   4,8     8,12    12,16
		{
			tEdis[i*La+j]=pow((pVSS[0][i][j]-dB_A),2)+pow((pVSS[1][i][j]-dB_B),2)+pow((pVSS[2][i][j]-dB_C),2)+pow((pVSS[3][i][j]-dB_D),2);  //�󷽲�
		}

	//----------------------------3��ѡ���������С��������-------------------------------
	const int WEIGHT_AMOUNT=16;
	unsigned char Xnum[WEIGHT_AMOUNT],Ynum[WEIGHT_AMOUNT];          //ѡ��������С��
	double WA_Dat[WEIGHT_AMOUNT];
	int Nwa=WEIGHT_AMOUNT;
	for(int n=0;n<Nwa;n++)
	{
		temp=10000;
		for(int j=RangeY1;j<=RangeY2;j++)            //0,5   5,10   ,10,15   15,20
			for(int i=RangeX1;i<=RangeX2;i++)        //0,4   4,8     8,12    12,16
			{
				if(tEdis[i*La+j]<temp)
				{
					temp=tEdis[i*La+j];
					Xnum[n]=i;
					Ynum[n]=j;
					WA_Dat[n]=tEdis[i*La+j];
				}
			}
			tEdis[ Xnum[n]*La+Ynum[n] ]=10000;
	}
	if(0)
	{
		CString str;                             //���
		str.Format("\n --i,j-- \n");
		TRACE(str);
		for(int n=0;n<Nwa;n++)
		{	
			str.Format("%d  %d  ",Xnum[n],Ynum[n]);
			TRACE(str);
		} 
	}
	//------------------------4����Ȩֵ----------------------------
	float w[WEIGHT_AMOUNT]; 
	temp=0;
	for(int i=0;i<Nwa;i++)
	{
		temp+=1/WA_Dat[i];
	}
	for(int i=0;i<Nwa;i++)
	{
		w[i]=(float)( (1/WA_Dat[i]) / temp );
	}
	double Xrst=0;    //result
	double Yrst=0;   
	for(int i=0;i<Nwa;i++)        //ͨ��Ȩֵ������
	{
		Xrst+=w[i]*Xnum[i];
		Yrst+=w[i]*Ynum[i];
	}
	if(0)
	{
		CString str;                             
		str.Format("\n --Result:X,Y-before change-- \n");
		TRACE(str);
		str.Format("%.2f  ",Xrst);
		TRACE(str);
		str.Format("%.2f  ",Yrst);
		TRACE(str);
	}

    //---------------5��ת�� 30*30 to 20*20 -------------
	int Wost=(MtxW-RelW)/2;              //offset
	int Lost=(MtxL-RelL)/2;

	if(Xrst<Wost)	Xrst=Wost;
	if(Yrst<Lost)   Yrst=Lost;
	if(Xrst>(RelW+Wost))	Xrst=RelW+Wost;
	if(Yrst>(RelL+Lost))	Yrst=RelL+Lost;
	Xrst-=Wost;               
	Yrst-=Lost;
	*pX=Xrst;
	*pY=Yrst;
	if(0)
	{
		CString str;                             
		str.Format("\n --Result:X,Y-- \n");
		TRACE(str);
		str.Format("%.2f  ",Xrst);
		TRACE(str);
		str.Format("%.2f  ",Yrst);
		TRACE(str);
	}
	//---------------------6��ɾ���ڴ�-----------------------------
	delete tEdis;
	
}

floatP dB_to_XY_opt2(const float* dB_V,const float* pVSSM,
	int vssmWidth, int vssmHeight, int vssmRealWidth, int vssmRealHeight)
	//matrix  real
{
	double temp;
	floatP relP;
	relP.x=-1;
	relP.y=-1;

	int La=vssmWidth;        //31 
	int Lb=vssmHeight;       //31
	int nMul=vssmWidth;     //����
	int nMul2=vssmWidth*vssmHeight;  //��������

	double* tEdis=new double[La*Lb];        //20*20   
	if (tEdis==NULL)
	{
		return relP;
	}

	//------------------------------1:ѡ������,������-----------------------------
	//-------------------------2�������ڼ���---------------------------------
	int RangeX1=0;
	int RangeX2=La;
	int RangeY1=0;   //ǿ��ȫ�ֱȽ�
	int RangeY2=Lb;
	float fValue[4];

	//���㷽��
	for(int j=RangeY1;j<RangeY2;j++)            //0,5   5,10   ,10,15   15,20
		for(int i=RangeX1;i<RangeX2;i++)        //0,4   4,8     8,12    12,16
		{
			fValue[0]=pVSSM[j*nMul+i];
			fValue[1]=pVSSM[j*nMul+i+1*nMul2];
			fValue[2]=pVSSM[j*nMul+i+2*nMul2];
			fValue[3]=pVSSM[j*nMul+i+3*nMul2];
			tEdis[j*nMul+i]=Variance_opt(fValue, dB_V, 4);   //���� 31*31�����ڵķ���
		}

		//----------------------------3��ѡ���������С��������------------------------------
		const int WEIGHT_AMOUNT=16;   //ѡȡ��С�ĵ���
		int Nwa=WEIGHT_AMOUNT;

		struct WeightAmountToXY_TM   // TM:��ʱģ��
		{
			floatP Point[WEIGHT_AMOUNT];   //�㷽λ
			float Data[WEIGHT_AMOUNT];     //ǿ��
			float w[WEIGHT_AMOUNT];       //Ȩֵ
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
		//------------------------4����Ȩֵ----------------------------
		float* pWeight=new float[Nwa];
		Weight_opt_FromVariance(pWATXY->Data, Nwa, pWeight);

		double Xrst=0;    //result
		double Yrst=0;   
		for(int i=0;i<Nwa;i++)        //ͨ��Ȩֵ������
		{
			Xrst+= pWeight[i] *( pWATXY->Point[i].x);
			Yrst+= pWeight[i] *( pWATXY->Point[i].y);
		}

		delete[] pWeight;
		//---------------5��ת�� 30*30 to 20*20 -------------
		int Wost=(vssmWidth-vssmRealWidth-1)/2;              //offset
		int Lost=(vssmHeight-vssmRealHeight-1)/2;

		if(Xrst<Wost)	Xrst=Wost;
		if(Yrst<Lost)   Yrst=Lost;
		if(Xrst>(vssmRealWidth+Wost))	Xrst=vssmRealWidth+Wost;
		if(Yrst>(vssmRealHeight+Lost))	Yrst=vssmRealHeight+Lost;
		Xrst-=Wost;               
		Yrst-=Lost;

		//---------------------6��ɾ���ڴ�-----------------------------
		delete[] tEdis;
		delete pWATXY;

		relP.x=(float)Xrst;
		relP.y=(float)Yrst;
		return relP;

}

//�������Ծ��� Vori:��ʼ����ֵ�� Vx���յ���ֵ
int LVSSM_opt(double* pLVSSM, double Vori, double Vx)
{
	if ( NULL == pLVSSM || 
		 Vori < 0 || Vx < 0 || 
		 Vx < Vori )
	{
		return -1;
	}

	double xx,yy,xy,pp;
	double temp;
	double ex;
	//double p[nLenOfSide][nLenOfSide]=pVSS;
	//Vori=37;
	int nLenOfSide=20;  //��׼����
	int nExLen=5;      //�ӳ���
	int nHigh=1;      //�߶�  >= 1

	int overallLen=nLenOfSide+nExLen*2+1; //ȫ���� �ܵ����� nLenOfSide Ϊ��ʵ�㣬nExLenΪ�ӳ���
	double z=(double)nHigh;  //z�ᣬ�߶�
	double *pLine= new double[overallLen];//10*i+j

	// v1 = v0 + aLog(d1)
	// v2 = v0 + aLog(d2)
	// v2 - v1 = aLog(d2/d1)
	// �� d1 = 1 ʱ�� v2 - v1 = aLog(d2);
	// a = (v2 - v1) / Log(d2);
	// ex = a;
	ex=(Vx-Vori)/log10((double)nLenOfSide);

	for(int n=nExLen;n<overallLen ;n++) // ����㿪ʼ���㣬��������ͬ
	{
		xx = (double)(n-nExLen);
		temp=Vori+ex*0.5*log10(xx*xx+z*z);
		pLine[n]=temp;
		
	}

	for (int n =0; n < nExLen; n++) //���� ���ԭ�㷴����ֵ
	{
		pLine[n] = pLine[nExLen*2-n];
	}

	for(int n=0; n<overallLen; n++) //������ϣ��� ��ֵ
	{
		pLVSSM[n] = pLine[n];
	}

	delete[] pLine;
	return 0;
}

//ǿ��ƽ�����   true�����任  ��ȡԶƽ���ǿ�ȣ�   false�� ��任
float RssiSpacePlaneCorrection_opt(float Vin, 
	                               float Va,
								   float Vb,
								   float Distance,
								   float High)
{
	return 0;
}

//��һ�����ϣ�����Rssi�Ĳ�����
float GetMirrorRssi_opt(float Vin,   //VinֵӦ����  Va��Vb֮�� 
	                    float Va/* ���� */, 
						float Vb/* Զ�� */, 
						float Distance/* �ο���������� */, 
						float High/* ����ƽ������ο���߶� */,    //operation
						float fCoe/* ����ϵ��=1 */ )  
{
	float fDistance=Distance*fCoe;
	float e=(Vb-Va)/log10(fDistance);
	float temp=(Vin-Va)/e;
	float d0=pow(10,temp);
	float d1=d0*d0-High*High;    //ԭ��
	if (d1<=0)
	{
		return Vb;
	}
	d1=Distance-sqrt(d1);
	if(d1<=0)
	{
		return Va;
	}
	float d2=sqrt( d1*d1+High*High );  //����
	float relRssi=Va+e*log10(d2);

	return relRssi;
}


floatP dB_to_XY_IrregularShape_opt(float* dB, 
	                               int SelPointNum, 
								   const float* pRssi, 
								   const floatP* pPoint)
{
	floatP relP;
	relP.x=0;
	relP.y=0;

	float* pfVariance=new float[SelPointNum];
	float* pfWeight=new float[SelPointNum];

	if (pfVariance==NULL || pfWeight==NULL)
	{
		delete[] pfVariance;
		delete[] pfWeight;
		return relP;
	}

	for (int n=0; n<SelPointNum; n++)    //�㷽��
	{
		pfVariance[n]=pow( (dB[0]-pRssi[n]), 2 )+pow( (dB[1]-pRssi[n+SelPointNum]), 2 )+pow( (dB[2]-pRssi[n+SelPointNum*2]), 2 );
	}


	double temp=0;
	for (int n=0; n<SelPointNum; n++)  //��Ȩֵ
	{
		temp+= 1/pfVariance[n];
	}
	for (int n=0; n<SelPointNum; n++)
	{
		pfWeight[n]=(float)( 1/(pfVariance[n]*temp) );
	}

	for (int n=0; n<SelPointNum; n++)   //������
	{
		relP.x+=pfWeight[n]*pPoint[n].x;
		relP.y+=pfWeight[n]*pPoint[n].y;
	}

	delete[] pfVariance;
	delete[] pfWeight;
	return relP;
}


int dB_RectAdjust(float* dB_V, float maxdB, float mindB)//ǿ�� �߼�����
{
	float *(dB[4]);    //����1��4�ֱ��Ǵ�С����
	float *tdB;    //����1��4�ֱ��Ǵ�С����
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
		float MAX = maxdB;
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
		float MIN = mindB;
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

		return 0;
}


floatP CorrectXY_ForRect(floatP Point, RF_Rect_s& rect, int vssmRealWidth, int vssmRealHeight)
{
	if (Point.x<0 || Point.y<0)
	{
		return Point;
	}

	RF_Rect_s Rect = rect;
	float kW=(float)( Rect.right-Rect.left  )/vssmRealWidth;
	float kH=(float)( Rect.bottom-Rect.top )/vssmRealHeight;
	Point.x= Rect.left + Point.x*kW;
	Point.y= Rect.top  + Point.y*kH;

	return Point;
}