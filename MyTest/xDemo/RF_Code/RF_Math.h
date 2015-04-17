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
		1,对于浮点型函数，统一输入为float类型，输出为double类型;
		2,
*/

//浮点型转整形，适应规则四舍五入
int float_to_int(float val);  // no test

//坐标复制
void point_copy(floatP rst, floatP& dst);

//坐标清零
void point_zero(floatP& p);

/*
	sort_mode: 0,ASCENDING; 1,DESCENDIN
	return:    if(suc) return dat_num
*/
int make_sort(float* dat, int dat_num, int sort_mode); // sort_mode: 

/************************************************************************/
/*                                旧函数                                */
/************************************************************************/

int F2I(double Num_in,int mode=0,int range1=0,int range2=20);

void XYctrl(int mode,doubleP *a,doubleP *b);

#define ASCENDING             1
#define DESCENDIN             2
#define REALNUMBER_MODE       0
#define POSITIVENUMBER_MODE   1  //整数排序  忽视小数
bool MakeSort(double* Dat,int Num,int SortMode, int NumMode, float Dat_Max=100);



float GetMedium(float* Dat,int Num);

float Variance_opt(const float* pValueA, const float* pValueB,int nNum);

bool Weight_opt_FromVariance(const float* pVariance, int nNum, float* pWeight_Out);



