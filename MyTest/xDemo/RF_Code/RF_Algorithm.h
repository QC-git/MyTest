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
	int style;        //0:����   1������
	double line;
	double range1;
	double range2;
};

struct Road_model2{
	doubleP P1;      //��ʼ����
	doubleP P2;      //�յ�����

};

struct CRectangle_model{               //����ģ��
	double oriX,oriY;
	double broadwise;   //  ����  �� 
	double endwise;     //  ����  ��
}; 

//------------------------public-----------------------

/*
	func: ��ȡ��һ��ʱ��������
	src:  �ϴ������
	dst:  Ŀ�������
	speed�� ÿ��ǰ�����ٶ� ��λ m/s
	Time_ms��ˢ��Ƶ��
	MaxDifDistance�� ����֮������ֵ
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

//�������Ծ���
int LVSSM_opt(double* pLVSSM, double Vori, double Vx);

int LVSSM_dB_to_XY_opt();

//��һ�����ϣ�����Rssi�Ĳ�����
float GetMirrorRssi_opt(float Vin,   //VinֵӦ����  Va��Vb֮�� 
	float Va/* ���� */, 
	float Vb/* Զ�� */, 
	float Distance/* �ο���������� */, 
	float High/* ����ƽ������ο���߶� */,
	float fCoe=1/* ����ϵ�� */ );    //operation

//������������ֲ��㷨
floatP dB_to_XY_IrregularShape_opt(float* dB, 
	int SelPointNum, 
	const float* pRssi, 
	const floatP* pPoint);

int dB_RectAdjust(float* dB_V, float maxdB, float mindB);//ǿ�� �߼�����

floatP CorrectXY_ForRect(floatP Point, RF_Rect_s& rect, int vssmRealWidth, int vssmRealHeight);
