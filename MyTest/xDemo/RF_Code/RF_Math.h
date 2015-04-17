#pragma once

#include <math.h>

struct doubleP{           //   Plane coordinate system 
	double x;
	double y;
};
struct intP{      
	int x;
	int y;
};
struct floatP
{
	float x;
	float y;
};

/*
	Rules:
		1,���ڸ����ͺ�����ͳһ����Ϊfloat���ͣ����Ϊdouble����;
		2,
*/

//������ת���Σ���Ӧ������������
int float_to_int(float val);  // no test

//���긴��
void point_copy(floatP rst, floatP& dst);

//��������
void point_zero(floatP& p);

/*
	sort_mode: 0,ASCENDING; 1,DESCENDIN
	return:    if(suc) return dat_num
*/
int make_sort(float* dat, int dat_num, int sort_mode); // sort_mode: 

/************************************************************************/
/*                                �ɺ���                                */
/************************************************************************/

int F2I(double Num_in,int mode=0,int range1=0,int range2=20);

void XYctrl(int mode,doubleP *a,doubleP *b);

#define ASCENDING             1
#define DESCENDIN             2
#define REALNUMBER_MODE       0
#define POSITIVENUMBER_MODE   1  //��������  ����С��
bool MakeSort(double* Dat,int Num,int SortMode, int NumMode, float Dat_Max=100);



float GetMedium(float* Dat,int Num);

float Variance_opt(const float* pValueA, const float* pValueB,int nNum);

bool Weight_opt_FromVariance(const float* pVariance, int nNum, float* pWeight_Out);



