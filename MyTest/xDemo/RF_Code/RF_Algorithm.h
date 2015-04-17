#pragma once

#include "RF_Math.h"

struct RF_ID_s
{
	long Tag;
	long Rea;
};

struct RF_Rect_s
{
	long left;
	long top;

	long right;
	long bottom;
};

struct Road_model1{
	int style;        //0:横向   1：纵向
	double line;
	double range1;
	double range2;
};

struct Road_model2{
	doubleP P1;      //起始坐标
	doubleP P2;      //终点坐标

};

struct CRectangle_model{               //方形模型
	double oriX,oriY;
	double broadwise;   //  横向  宽 
	double endwise;     //  纵向  长
}; 

//------------------------public-----------------------

/*
	func: 获取下一个时间差的坐标
	src:  上次坐标点
	dst:  目的坐标点
	speed： 每秒前进的速度 单位 m/s
	Time_ms：刷新频率
	MaxDifDistance： 坐标之间最大差值
*/
floatP GetNextPoint(floatP src, floatP dst, float Speed, int Time_ms, int MaxDifDistance, int Mode);  
//------------------------private------------------------
bool OutTheObstacle_opt(double *dX,
	double *dY,
	void* Obstacle,
	int Shape=1);

void VSS_opt(double *pVSS,
	double Vori,
	double Vx,
	double Vy,
	double Vxy);

void dB_to_XY_opt(float* dB_V,
	double* pX,
	double* pY,
	float(*Mtx)[31][31],
	int MtxW=30,
	int MtxL=30,
	int RelW=20,
	int RelL=20);

floatP dB_to_XY_opt2(const float* dB_V,const float* pVSSM,
	int vssmWidth, int vssmHeight, int vssmRealWidth, int vssmRealHeight);

//生成线性矩阵
int LVSSM_opt(double* pLVSSM, double Vori, double Vx);

int LVSSM_dB_to_XY_opt();

//在一条线上，镜像Rssi的产生，
float GetMirrorRssi_opt(float Vin,   //Vin值应处于  Va与Vb之间 
	float Va/* 近点 */, 
	float Vb/* 远点 */, 
	float Distance/* 参考点相隔距离 */, 
	float High/* 待测平面相隔参考点高度 */,
	float fCoe=1/* 环境系数 */ );    //operation

//接收器不规则分布算法
floatP dB_to_XY_IrregularShape_opt(float* dB, 
	int SelPointNum, 
	const float* pRssi, 
	const floatP* pPoint);

int dB_RectAdjust(float* dB_V, float maxdB, float mindB);//强度 逻辑调整

floatP CorrectXY_ForRect(floatP Point, RF_Rect_s& rect, int vssmRealWidth, int vssmRealHeight);
