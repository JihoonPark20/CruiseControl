/*
 * driveController.c
 *
 *  Created on: 2017. 6. 29.
 *      Author: ghbae
 */

/*******************************************************************************
* Include
*******************************************************************************/
#include "global.h"
#include "vehicleController.h"
#include "cameraController.h"
#include "timerController.h"
#include "imageProcess.h"
#include "queueADT.h"
#include <math.h>
#include "Schedular.h"
/*******************************************************************************
* Constant
*******************************************************************************/

int Left;
int Right;

/*******************************************************************************
* Define & MACRO
*******************************************************************************/
#define THRESHOLD_JUDGE_CROSSING 10      //Degree

#define BOTH_LINE_DETECT      0
#define ONLY_RIGHT_LINE_DETECT   1
#define ONLY_LEFT_LINE_DETECT   2
#define NO_LINE_DETECT         3

#define UPPERCENTER            85
#define LOWERCENTER            76

/*******************************************************************************
* Type Definition
*******************************************************************************/


/*******************************************************************************
* Exported Global
*******************************************************************************/
extern queue* driveQ;
extern cameraStruct2D camera2D;
extern vehicleControl vehicle;
int line_right=0;
int line_left=0;
extern int line_state;
extern int schoolzone_state;
extern int schoolzone_position;
extern int Hill_state;
extern int Hill_position;
/*******************************************************************************
* Memeber Functions
*******************************************************************************/
void drivingControl(void) {
   /*   ���ʿ� ��ֹ��� �ִ��� �Ǵ��ϰ� ���������ͷ� ������(pure pursuit)�� ���Ѵ�.   */
//   judgeCrossCondition();

   /*   ������ �����ϰ� ��� ����� ���������� ���� �α׸� �����.   */
   if(vehicle.statusData.log.isDetect == TRUE) {
      logUpdate();
   }

   /*   ��� ����� �����ϸ� �������¸� �ʱ�ȭ���ش�   */
   vehicle.statusData.log.isDetect = FALSE;
}

void setPurePursuit(edge edgeAngle) {
   double lengthWayPoint = (((double)CAM_PARAMETER_DISTANCE) / cos(edgeAngle));
   double curvature = (((double)2*sin(edgeAngle))/(lengthWayPoint));
   double servoArgument = atan(((double)VEHICLE_PARAMETER_LENGTH)*curvature);

   vehicle.purePursuit.servoArgument = servoArgument;
   vehicle.purePursuit.curCurvature = curvature;
   vehicle.purePursuit.lengthWayPoint = lengthWayPoint;
}



void setKineticStatus(velocityType mps, rpmType rpm, angleRadianType steeringAngle) {
   /*   ������ɰ�(servoArgu)�̶� ���� �ӵ� ���� ������ ���� � �� �� ������Ʈ   */
   vehicle.statusData.kinetic.curMps = mps;
   vehicle.statusData.kinetic.curRpm = rpm;
   vehicle.statusData.kinetic.curSteeringAngle = steeringAngle;
}

void updateDrivingStatus(void) {
   /*   SI ���ͷ�Ʈ ���Ŀ� checkProcess2D �Լ�����
    * �̹��� ���μ����� ������ ȣ��Ǵ� �Լ�
    *  �̹��� ó���� ���� ���� ��������
    *  vehicle ����ü�� ������Ʈ�Ѵ�.   */
   edgeNode* curNode;
      int leftCnt = 0, rightCnt = 0;
      int left_m=0,right_m=0;
      int left[camera2D.result.enQCnt], right[camera2D.result.enQCnt];
      int ref_center=0;
      /*   driveQ�� main.c�� ����� �۷ι� ����
       *  ť ����ü�� �������̴�.
       *  ����ü�� �����ʹ� .�� �ƴ� ->�� ȣ���Ѵ�.
       *  ex) driveQ->
       *  ex) edgeNode->
       *  ť ������ queueADT.h*/
      //double leftang=0,rightang=0;
      while( (curNode = deQueue(driveQ)) != NULL ) {
         /*
          // ť�� ����� ���� �������� ���� ���(�� ��ġ, �� ��ġ, ���� ����)�� �ҷ��ͼ� �ش� ��尡 left���� right���� �Ǵ��Ѵ�.
          // ����� col ���� (���� ��ġ ����)��
          // log�� old���� �ҷ��ͼ� ���ϴ���  -> (ex. (oldLeft + oldRight)/2 �ϸ� �뷫 �߰� �����̶�� ����)
          // ������ �߰� ������ ���ϴ���
          // ���ǹ� �������� ������ �ؼ� left���� right���� �Ǵ��Ѵ�.

         if( curNode->col ��  curNode->row �� ���ؼ� left��) {
            leftAvgDir = leftAvgDir + curNode->dir;
            left = left + curNode->row;
            leftCnt++;
         }
         else if( curNode->col �� curNode->row �� ���ؼ� right��) {
            rightAvgDir = rightAvgDir + curNode->dir;
            right = right + curNode->row;
            rightCnt++;
         }
         */
         if (vehicle.statusData.log.oldleft == 0 && vehicle.statusData.log.oldright == 0) {
            ref_center = 80;
         }
         else {
            ref_center = vehicle.statusData.log.oldcenter;
         }
         if (curNode->col <= ref_center) {
            left[leftCnt] = curNode->col;
            leftCnt++;
         } else if (curNode->col > ref_center) {
            right[rightCnt] = curNode->col;
            rightCnt++;
         }

         /*   left�� rigth �������� ������ ������ ������ ������.
          *  deQueue�� ť�� ������ �� ���¶�� �� �����͸� �����Ѵ�.
          *  �� �۾��� ������ �̹��� ������ ó���� ���� �������� left�� right�� ���� �з��ȴ�. */

      }
      if(leftCnt>=10){
                  leftCnt=0;
               }
               if(rightCnt>=10){
                  rightCnt=0;
               }
      int CAM_SITUATION=0;
         if(leftCnt != 0 && rightCnt !=0)
         {
            CAM_SITUATION = BOTH_LINE_DETECT;
         }
         else if(leftCnt == 0  && rightCnt !=0)
         {
            CAM_SITUATION = ONLY_RIGHT_LINE_DETECT;
         }
         else if(leftCnt != 0 && rightCnt == 0)
         {
            CAM_SITUATION = ONLY_LEFT_LINE_DETECT;
         }
         else {
            CAM_SITUATION = NO_LINE_DETECT;
         }


      /*   ī��Ʈ ���� ���� ��հ��� ���Ѵ�. ī��Ʈ ���� 0�̸� ���� �� ������ �߻��ϹǷ� ���ǹ����� ����ó�����ش�.   */
      //if(leftCnt != 0) {
      //   leftAvgDir = (leftAvgDir) / ( (double)leftCnt );
      //   left = (left) / ( (double)leftCnt );

         /*   ������ �����ϸ�  ���� �α׸� ����� ���� ������������ ����Ѵ�.   */
      //   vehicle.statusData.log.isDetect = TRUE;
      //}
      //else {
         /*   leftCnt = 0(���ʿ��� ���� ���� �ϳ��� �ȵǾ��� ��
          * ����ó�� ���ش�.
          * ���� �������� �����µ� ������ �������Ÿ� ������ ������ ��������
          * ������ ���� ������ ������ ���� ����� �����ش�.*/
      //}

      //if(rightCnt != 0) {
      //   rightAvgDir = (rightAvgDir) / ( (double)rightCnt );
      //   right = right / ( (double)rightCnt );

         /*   ������ �����ϸ�  ���� �α׸� ����� ���� ������������ ����Ѵ�.   */
      //   vehicle.statusData.log.isDetect = TRUE;
      //}
      //else {
         /*   rightCnt = 0(�����ʿ��� ���� ���� �ϳ��� �ȵǾ��� ��
         * ����ó�� ���ش�.
         * ���� ������ �����µ� �������� �������Ÿ� ���� ���� ���������� �������� �����ش�.
         * ���� ������ �� �� ���������� ���� log�� old���� �ҷ��ͼ� ���ų�
         * old���� ���ٸ� �ӵ��� ���߰ų� �����Ѵ�.
         * ����������� ������ �������� ��쳪
         * ��ֹ��� ������ ��츦 ����Ͽ� �ڵ��ϸ� �ɵ�.*/
      //}


         //if(schoolzone_state==1 && CAM_SITUATION == ONLY_LEFT_LINE_DETECT && line_state==0 && vehicle.statusData.lineStatus.curTurnState==Straight)
         if(schoolzone_state==1 && CAM_SITUATION == ONLY_LEFT_LINE_DETECT && line_state==0)
         {
        	 line_left++;
        	 if(line_left>2)
        		 line_state = 1;
         }
        //else if(schoolzone_state==1 && CAM_SITUATION == ONLY_RIGHT_LINE_DETECT && line_state==0 && vehicle.statusData.lineStatus.curTurnState==Straight)
         else if(schoolzone_state==1 && CAM_SITUATION == ONLY_RIGHT_LINE_DETECT && line_state==0)
         {
        	line_right++;
        	if(line_right>2)
        		line_state = 2;
        }
         switch(CAM_SITUATION)
            {

            case BOTH_LINE_DETECT ://�Ѵ� ������
               left_m= medianSortStruct(left,0,leftCnt-1);
               right_m=medianSortStruct(right,0,rightCnt-1);
               vehicle.statusData.log.isDetect = TRUE;
               break;


            case ONLY_RIGHT_LINE_DETECT : //����Ʈ���� ����Ʈ����


               right_m=medianSortStruct(right,0,rightCnt-1);
               left_m = right_m-120;
               vehicle.statusData.log.isDetect = TRUE;
               break;


            case ONLY_LEFT_LINE_DETECT : //����Ʈ��� ����Ʈ����


               left_m= medianSortStruct(left,0,leftCnt-1);
               right_m = left_m+120;
               vehicle.statusData.log.isDetect = TRUE;
               break;


            default ://NO_LINE_DETECT

               vehicle.statusData.log.isDetect = FALSE;
               break;
            }
         if(right_m>UPPERCENTER+120){
            right_m=UPPERCENTER+120;
         }
         else if(right_m<LOWERCENTER){
            right_m=LOWERCENTER;
         }
         if(left_m<LOWERCENTER-120){
            left_m=LOWERCENTER-120;
         }
         else if(left_m>UPPERCENTER)
         {
            left_m=UPPERCENTER;
         }

      //      vehicle.statusData.camData.curLeftEdgeAngle = (vehicle.statusData.camData.curLeftEdgeAngle/PHI)*180;
         //   vehicle.statusData.camData.curRightEdgeAngle = (vehicle.statusData.camData.curRightEdgeAngle/PHI)*180;
           //servoset(vehicle.statusData.camData.curcenter);
            if(vehicle.statusData.log.isDetect==TRUE)
			{
				vehicle.statusData.camData.curleft = (int)left_m;
				vehicle.statusData.camData.curright =(int)right_m;
				vehicle.statusData.camData.curcenter = (int)(left_m+right_m)/2;
				if(schoolzone_state==1)
				{
				Left = vehicle.statusData.camData.curleft;
				Right = vehicle.statusData.camData.curright;
				}
		//		vehicle.statusData.camData.curLeftEdgeAngle = (vehicle.statusData.camData.curLeftEdgeAngle/PHI)*180;
			//	vehicle.statusData.camData.curRightEdgeAngle = (vehicle.statusData.camData.curRightEdgeAngle/PHI)*180;
				//if((schoolzone_state==1 && schoolzone_position==1) || (Hill_state == 1))
				if((schoolzone_state==1 && schoolzone_position==1) || (Hill_state == 1 && (Hill_position==0 || Hill_position==1)))
				{
				}
				else
					servoset(vehicle.statusData.camData.curcenter);
			}




      //      double calcedWidth = 0, calibratedWidth = 0;

         //   calcedWidth = ABS(left_m - right_m);
            /*   calcedWidth : ���� ������ ���� �ȼ��� �������̷� ã�� �Ÿ�.
             * 1�����϶��� ��������   60cm�� �����ϱ⶧���� ū ������ ����.
             * ��Ŀ�� �����̳� �б������� calcedWidth�� ũ�� �ٲ��.*/

         //   calibratedWidth = (calcedWidth / 2)   * (cos(leftAvgDir_m) + cos(rightAvgDir_m));
            /*   ���� ����̳� ����������� ������ ���� */

            /*   calcedWidth�� ���� ������������ ���� ��������   */
      //      vehicle.statusData.camData.curLaneWidth = calcedWidth;
            /*   calibratedWidth�� ���� ����� ����ؼ� ������ ��������. �Ƹ� 1���� ���������� �����ϰ� ������ �� ����.   */
         //   vehicle.statusData.camData.curCompWidth = calibratedWidth;

         else {
            ;
         }
}

void logUpdate(void) {
   /*   ���� ������ ��츦 ����ؼ� ���� ������ �α׸� �����.
    * ���������� ��� �αװ�(old)�� �ҷ��ͼ� �������� ������.   */
   vehicle.statusData.log.oldCompWidth = vehicle.statusData.camData.curCompWidth;
   vehicle.statusData.log.oldLaneWidth = vehicle.statusData.camData.curLaneWidth;
   vehicle.statusData.log.oldLeftEdgeAngle = vehicle.statusData.camData.curLeftEdgeAngle;
   vehicle.statusData.log.oldRightEdgeAngle = vehicle.statusData.camData.curRightEdgeAngle;
   vehicle.statusData.log.oldLengthWayPoint = vehicle.purePursuit.lengthWayPoint;
   vehicle.statusData.log.oldCurCurvature = vehicle.purePursuit.curCurvature;
   vehicle.statusData.log.oldServoArgument = vehicle.purePursuit.servoArgument;
   vehicle.statusData.log.oldleft = vehicle.statusData.camData.curleft;
   vehicle.statusData.log.oldright = vehicle.statusData.camData.curright;
   vehicle.statusData.log.oldcenter = vehicle.statusData.camData.curcenter;
   vehicle.statusData.lineStatus.olddiff = vehicle.statusData.lineStatus.curdiff;
   vehicle.statusData.lineStatus.oldTurnState = vehicle.statusData.lineStatus.curTurnState;
   /*(��� �����͸� ���� ������� ������Ʈ)*/
}


void servoset(int center) {
	int curstate = vehicle.statusData.lineStatus.curTurnState;
	int oldstate = vehicle.statusData.lineStatus.oldTurnState;
	double cur_diff = vehicle.statusData.lineStatus.curdiff;
	double old_diff = vehicle.statusData.lineStatus.olddiff;

	if (center > UPPERCENTER) {
		cur_diff = center - (UPPERCENTER);
		if (cur_diff >= 6)
			curstate = TurningRight;
		else {
			curstate = TurnRight;
		}
	} else if (center < LOWERCENTER) {
		cur_diff = center - (LOWERCENTER);
		if (cur_diff <= -6) {
			curstate = TurningLeft;
		} else {
			curstate = TurnLeft;
		}
	} else {
		cur_diff = 0;
		curstate = Straight;
	}

	switch (curstate) {
	case TurningLeft:
		pwmServo(31250, (cur_diff - 2) * 0.5729);
		break;
	case TurnLeft:
		pwmServo(31250, (cur_diff - 2) * 0.5729);
		break;
	case TurningRight:
		pwmServo(31250, (cur_diff + 4) * 0.5729);
		break;
	case TurnRight:
		pwmServo(31250, (cur_diff + 4) * 0.5729);
		break;
	default:
		pwmServo(31250, cur_diff * 0.5729);
		break;
	}

	if (schoolzone_state == 1 || Hill_state == 1 || vehicle.statusData.camData.AEB == 1) {
	} else {
		if (oldstate == Straight) {
			if (cur_diff >= 5 || cur_diff <= -5) {
				pwmDcMotor(0, 70);
				while (1) {
					if (Scheduler_10ms()) {
						break;
					}
					while (1) {
						if (Scheduler_10ms()) {
							break;
						}
					}
				}
			}
		}

		if (curstate == TurningLeft || curstate == TurningRight) {
			pwmDcMotor(50, 0);
		} else {

			if (oldstate == TurningLeft || oldstate == TurningRight) {
				vehicle.statusData.lineStatus.turnCnt++;
				if (oldstate == TurningLeft) {
					curstate = TurningLeft;
				} else if (oldstate == TurningRight) {
					curstate = TurningRight;
				}
				pwmDcMotor(50, 0);
				if (vehicle.statusData.lineStatus.turnCnt >= 4) {
					pwmDcMotor(50, 0);
					vehicle.statusData.lineStatus.turnCnt=0;
				}
			} else {
				vehicle.statusData.lineStatus.turnCnt=0;
				if(curstate==TurnRight ||curstate==TurnLeft)
					pwmDcMotor(70,0);
				else
					pwmDcMotor(90, 0);
			}
		}
	}
}

