#include "StdAfx.h"
#include "RF_Math.h"

int float_to_int(float val)
{
	int rel;
	float t;

	rel = (int)val;
	t = val - rel;

	if ( t >= 0.5f )
		rel+=1;
	
	return rel;
}

void point_copy(floatP rst, floatP& dst)
{
	dst.x = rst.x;
	dst.y = rst.y;
}

void point_zero(floatP& p)
{
	p.x = 0.0f;
	p.y = 0.0f;
}

int make_sort(float* dat, int dat_num, int sort_mode)
{
	if ( NULL == dat || dat_num <= 1 )
	{
		return -1;
	}

	float temp;
	if ( 0 == sort_mode )
	{
		for(int i=0;i<dat_num-1;i++)
		{	for(int j=i+1;j<dat_num;j++)
			{
				if( dat[j] < dat[i] )
				{
					temp = dat[j];
					dat[j] = dat[i];
					dat[i] = temp;
				}
			}
		}
	}else
	{
		for(int i=0;i<dat_num-1;i++)
		{	for(int j=i+1;j<dat_num;j++)
			{
				if( dat[j] > dat[i] )
				{
					temp = dat[j];
					dat[j] = dat[i];
					dat[i] = temp;
				}
			}
		}
	}

	return dat_num;
}

/************************************************************************/
/*                                旧函数                                */
/************************************************************************/

//The decimal converted to an integer, but the specified range, four to five homes in
int F2I(double Num_in,int mode,int range1,int range2)
{
	int Num_out;
	Num_out=(int)Num_in;
	if( Num_in>=(Num_out+0.5) )
		Num_out+=1;
	if(mode==1)         //限制在（range1，range2）内
	{
		if(Num_out<range1)
			Num_out=range1;
		else if(Num_out>range2)
			Num_out=range2;
	}
	return Num_out;
}

//Mode 1: clear; Mode 2: point a to point B assignment
void XYctrl(int mode,doubleP *a,doubleP *b)
{
	if(mode==1)  //清0
	{
		a->x=0;
		a->y=0;
		b->x=0;
		b->y=0;
	}
	else if(mode==2)   //赋值 a->b
	{
		b->x=a->x;
		b->y=a->y;
	}
}

//Sorting: mode 1,ascending; mode 2,descending
bool MakeSort(double* Dat,
	int Num,
	int SortMode/* =ASCENDING */,
	int NumMode/* =REALNUMBER_MODE */,
	float Dat_Max/* =100 */)
{
	double temp;

	if(SortMode==ASCENDING)       //Ascending
	{
		if (NumMode==POSITIVENUMBER_MODE)    //检测是否是整数模式
			for(int n=0; n<Num; n++)
			{
				if (Dat[n]<=0)
				{
					Dat[n]=Dat_Max;
				}
			}

			for(int i=0;i<Num-1;i++)
				for(int j=i+1;j<Num;j++)
				{
					if( Dat[j]<Dat[i] )
					{
						temp=Dat[j];
						Dat[j]=Dat[i];
						Dat[i]=temp;
					}
				}

				if (NumMode==POSITIVENUMBER_MODE)    //检测是否是整数模式
					for(int n=0; n<Num; n++)
					{
						if (Dat[n]==Dat_Max)
						{
							Dat[n]=0;
						}
					}
	}

	else if(SortMode==DESCENDIN)    //Descending
	{
		if (NumMode==POSITIVENUMBER_MODE)    //检测是否是整数模式
			for(int n=0; n<Num; n++)
			{
				if (Dat[n]<=0)
				{
					Dat[n]=0;
				}
			}

			for(int i=0;i<Num-1;i++)
				for(int j=i+1;j<Num;j++)
				{
					if( Dat[j]>Dat[i] )
					{
						temp=Dat[j];
						Dat[j]=Dat[i];
						Dat[i]=temp;
					}
				}
	}
	else
		return false;

	return true;
}

//Calculating the median
float GetMedium(float* Dat,int Num)
{
	if(Num==1)
		return *Dat;

	float Result;
	double* tDat=new double[Num];

	for(int n=0;n<Num;n++)
		tDat[n]=Dat[n];

	MakeSort(tDat,Num, ASCENDING, REALNUMBER_MODE);

	if( (Num%2)==1 )     // uneven
	{
		Result=(float)tDat[Num/2];
	}
	else                //even
	{
		Result=(float)(tDat[Num/2]+tDat[Num/2-1])/2;
	}

	delete tDat;
	return Result;
}


float Variance_opt(const float* pValueA, const float* pValueB, int nNum)
{
	float relV=0;
	float temp=0;

	for (int n=0; n<nNum; n++)
	{
		temp=pow( (pValueA[n]-pValueB[n]),2 );
		relV+=temp;
	}

	if (relV==0)
	{
		relV=0.001f;
	}

	return relV;
}

bool Weight_opt_FromVariance(const float* pVariance, int nNum, float* pWeight_Out)
{
	float fDM=0.0f;
	for (int n=0; n<nNum; n++)
	{
		fDM+=1/pVariance[n];
	}
	for (int n=0; n<nNum; n++  )
	{
		pWeight_Out[n]=(1/pVariance[n]) / fDM;
	}

	return TRUE;
}
