#include "StdAfx.h"
#include "RF_Algorithm.h"

//To obtain the coordinates of a time difference
floatP GetNextPoint(floatP src, 
	                floatP dst, 
					float Speed, 
					int Time_ms, 
					int MaxDifDistance, 
					int Mode)   // Mode1:横向纵向移动力相等
{
	int nMaxDifDistance=MaxDifDistance;
	float fInterval=Speed*(float)Time_ms/1000;
	float XX=dst.x-src.x;  //差值
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
				else if( *dY==Y1 )        //边界处理
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
	int nLenOfSide=20;  //真实点数 单位：米
	int nExLen=5;   //延长点
	int nHigh=1;    //高度点 >= 1

	const int k=nLenOfSide+nExLen*2+1;  //总点数
	double z=(double)nHigh;       //高度
	double *pMatrix = new double[k*k];//10*i+j  //临时方阵

	// v1 = v0 + aLog(d1)
	// v2 = v0 + aLog(d2)
	// v2 - v1 = aLog(d2/d1)
	// 当 d1 = 1 时， v2 - v1 = aLog(d2);
	// a = (v2 - v1) / Log(d2);
	// ex = a;

	ey=(Vy-Vori)/log10((double)nLenOfSide);  // 计算 x轴 log参数
	ex=(Vx-Vori)/log10((double)nLenOfSide);  // 计算 y轴 log参数
	exy=(Vxy-Vori)/log10(sqrt(2.00)*nLenOfSide); //计算 xy轴 log参数

	//double dd;
	for(int n=nExLen+1;n<=nLenOfSide+nExLen*2;n++)  //从起点的下一点开始 计算原点外延三斜边
	{

		xx=(double)(n-nExLen);  //x轴 相对原点长度
		yy=(double)(n-nExLen);  //y轴 相对原点长度
		xy=xx*xx+yy*yy;

		temp=Vori+ex*0.5*log10(xx*xx+z*z);  //公式  v1 + 1/2 * e * log10(distance)
		pMatrix[(0+nExLen)*k+n]=temp;

		temp=Vori+ey*0.5*log10( yy*yy+z*z );
		pMatrix[n*k+(0+nExLen)]=temp;

		temp=Vori+exy*0.5*log10( xy+z*z );
		pMatrix[n*k+n]=temp;
	}
	temp=(Vori*3+0.5*(ex+ey+exy)*log10(z*z))/3; //计算原点平均值
	pMatrix[(0+nExLen)*k+(0+nExLen)]=temp;

	for(int i=nExLen;i<=nLenOfSide+nExLen*2;i++){   //通过真实三斜边 计算三斜边内部点值
		for(int j=nExLen;j<=nLenOfSide+nExLen*2;j++)
		{
			if( i==j || i==nExLen || j==nExLen)  continue;
			if( i>j )
			{
				pp=(pMatrix[i*k+i]-pMatrix[i*k+nExLen])/(double)(i-nExLen); //计算斜率
				temp=pMatrix[i*k+nExLen]+pp*(j-nExLen);
			}
			else
			{
				pp=(pMatrix[j*k+j]-pMatrix[nExLen*k+j])/(double)(j-nExLen);
				temp=pMatrix[nExLen*k+j]+pp*(i-nExLen);
			}
			pMatrix[i*k+j]=temp;
	}	}

	for(int i=0;i<nExLen;i++)	                 //计算 原点內延相对方形区域
		for(int j=0;j<nExLen;j++)	
			pMatrix[i*k+j]=pMatrix[(nExLen*2-i)*k+nExLen*2-j];
	for(int i=0;i<nExLen;i++)                     //计算 长方形内值
		for(int j=nExLen;j<=nLenOfSide+nExLen*2;j++)
			pMatrix[i*k+j]=pMatrix[(nExLen*2-i)*k+j];
	for(int i=nExLen;i<=nLenOfSide+nExLen*2;i++)  //计算 长方形内值
		for(int j=0;j<nExLen;j++)
			pMatrix[i*k+j]=pMatrix[i*k+nExLen*2-j]; 
	for(int n=0;n<k*k;n++)  //计算完毕赋值
		pVSS[n]=pMatrix[n];
	delete[] pMatrix; //释放内存
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

	//------------------------------1:选择区域,可跳过-----------------------------
/*	int temp_x=3,temp_y=3;
	int num_i,num_j;
	temp=10000;      //赋一个较大值作为比较
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<6;j++)
		{	
			num_i=3+5*i;
			num_j=3+5*j;
			tEdis[i*La+j]=pow((pVSS[0][num_i][num_j]-dB_A),2)+pow((pVSS[1][num_i][num_j]-dB_B),2)+pow((pVSS[2][num_i][num_j]-dB_C),2)+pow((pVSS[3][num_i][num_j]-dB_D),2);  //求方差
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
	//-------------------------2：区域内计算---------------------------------
	RangeY1=0;   //强制全局比较
	RangeY2=MtxL;
	RangeX1=0;
	RangeX2=MtxW;
	for(int j=RangeY1;j<=RangeY2;j++)            //0,5   5,10   ,10,15   15,20
		for(int i=RangeX1;i<=RangeX2;i++)        //0,4   4,8     8,12    12,16
		{
			tEdis[i*La+j]=pow((pVSS[0][i][j]-dB_A),2)+pow((pVSS[1][i][j]-dB_B),2)+pow((pVSS[2][i][j]-dB_C),2)+pow((pVSS[3][i][j]-dB_D),2);  //求方差
		}

	//----------------------------3：选择差异性最小的数个点-------------------------------
	const int WEIGHT_AMOUNT=16;
	unsigned char Xnum[WEIGHT_AMOUNT],Ynum[WEIGHT_AMOUNT];          //选出几个最小的
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
		CString str;                             //监测
		str.Format("\n --i,j-- \n");
		TRACE(str);
		for(int n=0;n<Nwa;n++)
		{	
			str.Format("%d  %d  ",Xnum[n],Ynum[n]);
			TRACE(str);
		} 
	}
	//------------------------4：求权值----------------------------
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
	for(int i=0;i<Nwa;i++)        //通过权值算坐标
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

    //---------------5：转换 30*30 to 20*20 -------------
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
	//---------------------6：删除内存-----------------------------
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
	int nMul=vssmWidth;     //乘数
	int nMul2=vssmWidth*vssmHeight;  //二级乘数

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
		int Wost=(vssmWidth-vssmRealWidth-1)/2;              //offset
		int Lost=(vssmHeight-vssmRealHeight-1)/2;

		if(Xrst<Wost)	Xrst=Wost;
		if(Yrst<Lost)   Yrst=Lost;
		if(Xrst>(vssmRealWidth+Wost))	Xrst=vssmRealWidth+Wost;
		if(Yrst>(vssmRealHeight+Lost))	Yrst=vssmRealHeight+Lost;
		Xrst-=Wost;               
		Yrst-=Lost;

		//---------------------6：删除内存-----------------------------
		delete[] tEdis;
		delete pWATXY;

		relP.x=(float)Xrst;
		relP.y=(float)Yrst;
		return relP;

}

//生成线性矩阵 Vori:起始点数值， Vx：终点数值
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
	int nLenOfSide=20;  //标准长度
	int nExLen=5;      //延长点
	int nHigh=1;      //高度  >= 1

	int overallLen=nLenOfSide+nExLen*2+1; //全长， 总点数， nLenOfSide 为真实点，nExLen为延长点
	double z=(double)nHigh;  //z轴，高度
	double *pLine= new double[overallLen];//10*i+j

	// v1 = v0 + aLog(d1)
	// v2 = v0 + aLog(d2)
	// v2 - v1 = aLog(d2/d1)
	// 当 d1 = 1 时， v2 - v1 = aLog(d2);
	// a = (v2 - v1) / Log(d2);
	// ex = a;
	ex=(Vx-Vori)/log10((double)nLenOfSide);

	for(int n=nExLen;n<overallLen ;n++) // 从起点开始计算，与计算矩阵不同
	{
		xx = (double)(n-nExLen);
		temp=Vori+ex*0.5*log10(xx*xx+z*z);
		pLine[n]=temp;
		
	}

	for (int n =0; n < nExLen; n++) //计算 相对原点反向数值
	{
		pLine[n] = pLine[nExLen*2-n];
	}

	for(int n=0; n<overallLen; n++) //计算完毕，， 赋值
	{
		pLVSSM[n] = pLine[n];
	}

	delete[] pLine;
	return 0;
}

//强度平面矫正   true：正变换  获取远平面点强度，   false： 逆变换
float RssiSpacePlaneCorrection_opt(float Vin, 
	                               float Va,
								   float Vb,
								   float Distance,
								   float High)
{
	return 0;
}

//在一条线上，镜像Rssi的产生，
float GetMirrorRssi_opt(float Vin,   //Vin值应处于  Va与Vb之间 
	                    float Va/* 近点 */, 
						float Vb/* 远点 */, 
						float Distance/* 参考点相隔距离 */, 
						float High/* 待测平面相隔参考点高度 */,    //operation
						float fCoe/* 环境系数=1 */ )  
{
	float fDistance=Distance*fCoe;
	float e=(Vb-Va)/log10(fDistance);
	float temp=(Vin-Va)/e;
	float d0=pow(10,temp);
	float d1=d0*d0-High*High;    //原点
	if (d1<=0)
	{
		return Vb;
	}
	d1=Distance-sqrt(d1);
	if(d1<=0)
	{
		return Va;
	}
	float d2=sqrt( d1*d1+High*High );  //近点
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

	for (int n=0; n<SelPointNum; n++)    //算方差
	{
		pfVariance[n]=pow( (dB[0]-pRssi[n]), 2 )+pow( (dB[1]-pRssi[n+SelPointNum]), 2 )+pow( (dB[2]-pRssi[n+SelPointNum*2]), 2 );
	}


	double temp=0;
	for (int n=0; n<SelPointNum; n++)  //算权值
	{
		temp+= 1/pfVariance[n];
	}
	for (int n=0; n<SelPointNum; n++)
	{
		pfWeight[n]=(float)( 1/(pfVariance[n]*temp) );
	}

	for (int n=0; n<SelPointNum; n++)   //算坐标
	{
		relP.x+=pfWeight[n]*pPoint[n].x;
		relP.y+=pfWeight[n]*pPoint[n].y;
	}

	delete[] pfVariance;
	delete[] pfWeight;
	return relP;
}


int dB_RectAdjust(float* dB_V, float maxdB, float mindB)//强度 逻辑调整
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