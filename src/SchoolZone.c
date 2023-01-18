/*
 * SchoolZone.c
 *
 *  Created on: 2017. 7. 7.
 *      Author: User
 */

/*******************************************************************************
* Include
*******************************************************************************/
#include "vehicleController.h"
#include "DMS.h"
#include "global.h"
#include "Schedular.h"
#include "cameraController.h"
#include "timerController.h"
#include "imageProcess.h"


void Leftcross_obstacle(void);
void Rightcross_obstacle(void);

/*******************************************************************************
* Constant
*******************************************************************************/
int schoolzone_state=0;
int schoolzone_position=0;

int line_state=0; //0���ιٲ�������߲�

/*******************************************************************************
* Define & MACRO
*******************************************************************************/

/*******************************************************************************
* Exported Global
*******************************************************************************/
extern int Left;
extern int Right;
extern int DMS_BOTTOM;
//extern int DMS_TOP;
extern int DMS_RIGHT;
extern int DMS_LEFT;
extern int school_cnt_in;
extern int school_cnt_out;
//extern cameraStruct camera;
//extern cameraStruct2D camera2D;

bool isFrontObstacle(void) {
	if(DMS_BOTTOM >650)
		return TRUE;
	else
		return FALSE;
}

void schoolZoneStatus(void)
{
	if(school_cnt_in==1)// ���ܼ��� �������� �ν� ���� ��
	{
	      schoolzone_state = 1;
	      school_cnt_in=0;
	}
	while(schoolzone_state ==1)
   {
		pwmDcMotor(38,0); // ������ �� �ӵ� �ٽû����������
      while(line_state == 1) // 1���� ������ ����츦 ���� �ٳ־��
      {

         switch(schoolzone_position)
         {
            case 0 : // ��ֹ� �� �� ���(����)
            	//if(isFrontObstacle() == TRUE || DMS_LEFT > 500 || DMS_RIGHT > 500 )
            	//if(isFrontObstacle() == TRUE)
            	if(isFrontObstacle() == TRUE ||  DMS_LEFT > 550 || DMS_RIGHT > 550 )
            		schoolzone_position = 1;// ��ֹ� �ν�
            	else
            		schoolzone_position = 0;// ��ֹ� ���ν�
            	break;
            case 1 :             	// ��ֹ� �� ���
            	Rightcross_obstacle();// rightcross��� �Լ��ȿ� ��ֹ����ϰ� �ٽ� �����νı��� �ϴ� ���� ����
            	break;
         }
        if(school_cnt_out==1)// ���ܼ��� �ν����� �� count
         {
            schoolzone_state=0; // ������ ���� ���´�.
            break;
         }
         else if(line_state ==2)// 2������ ��������� while�� ����
            break;
      }
      while(line_state == 2) // 2���� ������ ����츦 ���� �ٳ־��
      {
         switch(schoolzone_position)
         {
            case 0 : // ��ֹ� �� �� ���(����)
            	//if(isFrontObstacle() == TRUE || DMS_LEFT > 500 || DMS_RIGHT > 500)
            	//if(isFrontObstacle() == TRUE)
            	if(isFrontObstacle() == TRUE || DMS_LEFT > 550 || DMS_RIGHT > 550 )
            		schoolzone_position = 1;
            	else
            		schoolzone_position = 0;
            	break;

            case 1 :   //��ֹ��� �� ���
            	Leftcross_obstacle();// leftcross��� �Լ��ȿ� ��ֹ����ϰ� �ٽ� �����νı��� �ϴ� ���� ����
            	break;
         }
         if(school_cnt_out==1)// ���ܼ��� �ν����� �� count
         {
            schoolzone_state=0; // ������ �������´�
            break;
         }
         else if(line_state ==1)// 2������ ��������� while�� ����
            break;
      }
      if(schoolzone_state==0) // ��ü ������ while�� ����

    	  break;
   }

}

void Leftcross_obstacle(void)
{
   while(1)
   {
	  pwmServo(31250, -20); //��ֹ� �ν��ϸ� ���� �������� �ִ��� ����
	  if (Left> 45)// ������ ������ ���� �������´�
		 break;
   }
   while(1)
   {
	  pwmServo(31250, -17);
	  if (Left < 20)     //
		 break;
   }
   while(1)
     {
	   pwmServo(31250, -14);
	   if (Left > 25)     // �ݴ������ �ν��Ҷ� �� �ٽ� ���� �������ε�����
           break;
     }
   while(1)
   {
	   pwmServo(31250, 21);// ���� �������� Ȯ���� �Ѵ�

	   if (Left < 25 && Left > 5)
		   break;
   }
   while(1)
   {
	   pwmServo(31250, 5);// ���� �������� Ȯ���� �Ѵ�
	   if (Left > 5)
	   {
		   line_state =1;
		   schoolzone_position=0;
            break;
	   }
   }
   // �ٽ� �����������θ� ���� ���
}
void Rightcross_obstacle(void)
{
	while(1)
   {
		/*P13_OMR.B.PCL0=1;
		P13_OMR.B.PS0=0;*/
      pwmServo(31250, 21); //��ֹ� �ν��ϸ� ���� �������� �ִ� ����
      if(Right < 120)     // ������ ���� �뿡�� �������´�
         break;
   }
   while(1)
   {
	   /*P13_OMR.B.PCL1=1;
	   P13_OMR.B.PS1=0;*/
      pwmServo(31250, 15);
      if(Right > 130)     //
         break;
   }
   while(1)
   {
	   /*P13_OMR.B.PCL2=1;
	   	P13_OMR.B.PS2=0;*/
	   pwmServo(31250, 10);
      if(Right < 130)    //�ݴ������ �ν��� �� �� �������´�
            break;
   }
   while (1)
   {
      pwmServo(31250, -21); // ������ �������� �ִ� ����
      if (Right > 120 && Right < 140 )//
         break;
   }
   while (1)
   {
	   pwmServo(31250, -5); // �������������� ������ش�
	   if (Right > 120)
	   {
		   line_state =2;
		   schoolzone_position=0;
		   break;
	   }
   }
   // �ٽ� ī�޶�� �������� �����鼭 ������Ƽ� ����
}









