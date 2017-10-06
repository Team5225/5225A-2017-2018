#pragma config(Sensor, in1,    autoPoti,       sensorPotentiometer)
#pragma config(Sensor, in2,    mobilePoti,     sensorPotentiometer)
#pragma config(Sensor, in3,    liftPoti,       sensorPotentiometer)
#pragma config(Sensor, in4,    armPoti,        sensorPotentiometer)
#pragma config(Sensor, in5,    clawPoti,       sensorPotentiometer)
#pragma config(Sensor, in6,    expander,       sensorAnalog)
#pragma config(Sensor, in7,    leftLine,       sensorLineFollower)
#pragma config(Sensor, in8,    rightLine,      sensorLineFollower)
#pragma config(Sensor, dgtl1,  armSonic,       sensorSONAR_mm)
#pragma config(Sensor, dgtl3,  limBottom,      sensorTouch)
#pragma config(Sensor, dgtl4,  limTop,         sensorTouch)
#pragma config(Sensor, dgtl5,  driveEncL,      sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  driveEncR,      sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  latEnc,         sensorQuadEncoder)
#pragma config(Sensor, dgtl11, clawSonic,      sensorSONAR_mm)
#pragma config(Motor,  port1,           arm,           tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           liftL1,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           driveL1,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           driveL2,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           liftL2,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           mobile,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           driveR1,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveR2,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           liftR,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port10,          claw,          tmotorVex269_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "Vex_Competition_Includes_Custom.c"

// Year-independent libraries

#include "motors.h"
#include "sensors.h"
#include "joysticks.h"
#include "cycle.h"
#include "utilities.h"
#include "pid.h"

#include "motors.c"
#include "sensors.c"
#include "joysticks.c"
#include "cycle.c"
#include "utilities.c"
#include "pid.c"

sCycleData gMainCycle;

//#define MOBILE_LIFT_SAFETY

/* Drive */
#define TCHN Ch4
#define PCHN Ch3
#define LCHN Ch2
#define ACHN Ch1

#define TDZ 10
#define PDZ 10
#define LDZ 25
#define ADZ 25

void setDrive(word left, word right)
{
	gMotor[driveL1].power = gMotor[driveL2].power = left;
	gMotor[driveR1].power = gMotor[driveR2].power = right;
}

void handleDrive()
{
	word left = gJoy[PCHN].cur + gJoy[TCHN].cur;
	word right = gJoy[PCHN].cur - gJoy[TCHN].cur;
	velocityCheck(driveEncL);
	if (gSensor[driveEncL].velGood)
	{
		if (left > 10 && gSensor[driveEncL].velocity < 0) left = 10;
		else if (left < -10 && gSensor[driveEncL].velocity > 0) left = -10;
	}
	velocityCheck(driveEncR);
	if (gSensor[driveEncR].velGood)
	{
		if (right > 10 && gSensor[driveEncR].velocity < 0) right = 10;
		else if (right < -10 && gSensor[driveEncR].velocity > 0) right = -10;
	}
	setDrive(left, right);
}

void stack(bool callHandlers);
task stackAsync();

/* Lift */
typedef enum _sLiftStates {
	liftIdle,
	liftRaise,
	liftLower,
	liftRaiseBrk,
	liftLowerBrk,
	liftHold,
	liftAuto
} sLiftStates;

#define LIFT_UP_KP 2
#define LIFT_DOWN_KP 1.5

#define LIFT_BOTTOM 110
#define LIFT_TOP 4094

short gLiftTarget;
sLiftStates gLiftState = liftIdle;
unsigned long gLiftStart;

void setLift(word power)
{
	gMotor[liftL1].power = gMotor[liftL2].power = gMotor[liftR].power = power;
}

void handleLift()
{
	if (RISING(Btn5U))
	{
		stopTask(stackAsync);
		gLiftTarget = LIFT_TOP;
		gLiftState = liftRaise;
		gLiftStart = nPgmTime;
	}
	else if (FALLING(Btn5U))
	{
		stopTask(stackAsync);
		gLiftTarget = gSensor[liftPoti].value;
	}
	if (RISING(Btn5D))
	{
		stopTask(stackAsync);
		gLiftTarget = LIFT_BOTTOM;
		gLiftState = liftLower;
		gLiftStart = nPgmTime;
	}
	if (RISING(Btn8L))
	{
		startTask(stackAsync);
	}

	switch (gLiftState)
	{
		case liftRaise:
		{
			short error = gLiftTarget - gSensor[liftPoti].value;
			if (error <= 0 || gSensor[limTop].value)
			{
				velocityClear(liftPoti);
				setLift(-10);
				gLiftState = liftRaiseBrk;
			}
			else
			{
				float output = error * LIFT_UP_KP;
				if (output < 60) output = 60;
				setLift((word)output);
			}
			break;
		}
		case liftRaiseBrk:
		{
			velocityCheck(liftPoti);
			if (gSensor[liftPoti].velGood && gSensor[liftPoti].velocity <= 0)
			{
				gLiftState = liftHold;
				writeDebugStreamLine("Lift up %d", nPgmTime - gLiftStart);
			}
			break;
		}
		case liftLower:
		{
			short error = gLiftTarget - gSensor[liftPoti].value;
			if (error >= 0 || gSensor[limBottom].value)
			{
				velocityClear(liftPoti);
				setLift(10);
				gLiftState = liftLowerBrk;
			}
			else
			{
				float output = error * LIFT_DOWN_KP;
				if (output > -60) output = -60;
				setLift((word)output);
			}
			break;
		}
		case liftLowerBrk:
		{
			velocityCheck(liftPoti);
			if (gSensor[liftPoti].velGood && gSensor[liftPoti].velocity >= 0)
			{
				gLiftState = liftHold;
				writeDebugStreamLine("Lift down %d", nPgmTime - gLiftStart);
			}
			break;
		}
		case liftHold:
		{
			if (gSensor[limBottom].value)
				setLift(-10);
			else
				setLift(12);
			break;
		}
	}
}


/* Arm */
typedef enum _sArmStates {
	armIdle,
	armRaise,
	armLower,
	armRaiseBrk,
	armLowerBrk,
	armHold,
	armAuto
} sArmStates;

#define ARM_UP_KP 0.15
#define ARM_DOWN_KP 0.18
#define ARM_POSITIONS (ARR_LEN(gArmPositions) - 1)

short gArmPositions[] = { 300, 1250, 2350 };
word gArmHoldPower[] = { -10, 12, 10 };
short gArmPosition = 2;
short gArmTarget;
sArmStates gArmState = armIdle;

void setArm(word power)
{
	if (power != gMotor[arm].power) writeDebugStreamLine("%d %d", gMainCycle.count, power);
	gMotor[arm].power = power;
}

void handleArm()
{
	if (RISING(Btn6U))
	{
		stopTask(stackAsync);
		if (gArmPosition < ARM_POSITIONS)
		{
			gArmTarget = gArmPositions[++gArmPosition];
			gArmState = armRaise;
		}
	}
	if (RISING(Btn6D))
	{
		stopTask(stackAsync);
		if (gArmPosition > 0)
		{
			gArmTarget = gArmPositions[--gArmPosition];
			gArmState = armLower;
		}
	}

	switch (gArmState)
	{
		case armRaise:
		{
			short error = gArmTarget - gSensor[armPoti].value;
			if (error <= 0)
			{
				velocityClear(armPoti);
				setArm(-8);
				gArmState = armRaiseBrk;
			}
			else
			{
				float output = error * ARM_UP_KP;
				if (output < 50) output = 50;
				setArm((word)output);
			}
			break;
		}
		case armRaiseBrk:
		{
			velocityCheck(armPoti);
			if (gSensor[armPoti].velGood && gSensor[armPoti].velocity <= 0) gArmState = armHold;
			break;
		}
		case armLower:
		{
			short error = gArmTarget - gSensor[armPoti].value;
			if (error >= 0)
			{
				velocityClear(armPoti);
				setArm(8);
				gArmState = armLowerBrk;
			}
			else
			{
				float output = error * ARM_DOWN_KP;
				if (output > -30) output = -30;
				setArm((word)output);
			}
			break;
		}
		case armLowerBrk:
		{
			velocityCheck(armPoti);
			if (gSensor[armPoti].velGood && gSensor[armPoti].velocity >= 0) gArmState = armHold;
			break;
		}
		case armHold:
		{
			setArm(gArmHoldPower[gArmPosition]);
			break;
		}
	}
}


/* Claw */

typedef enum _sClawStates {
	clawIdle,
	clawOpening,
	clawClosing,
	clawOpened,
	clawClosed
} sClawStates;

#define CLAW_CLOSE_POWER 127
#define CLAW_OPEN_POWER -127
#define CLAW_CLOSE_HOLD_POWER 5
#define CLAW_OPEN_HOLD_POWER 5

#define CLAW_OPEN 3000
#define CLAW_CLOSE 400

sClawStates gClawState = clawIdle;

void setClaw(word power)
{
	gMotor[claw].power = power;
}

void handleClaw()
{
	if (RISING(Btn7U))
	{
		gClawState = clawClosing;
		setClaw(CLAW_CLOSE_POWER);
	}
	if (RISING(Btn7D))
	{
		gClawState = clawOpening;
		setClaw(CLAW_OPEN_POWER);
	}

	switch (gClawState)
	{
		case clawIdle:
		{
			setClaw(0);
			break;
		}
		case clawOpening:
		{
			if (gSensor[clawPoti].value >= CLAW_OPEN) gClawState = clawOpened;
			break;
		}
		case clawClosing:
		{
			if (gSensor[clawPoti].value <= CLAW_CLOSE) gClawState = clawClosed;
			break;
		}
	}
}


/* Mobile Goal */

typedef enum _sMobileStates {
	mobileIdle,
	mobileRaise,
	mobileLower,
	mobileHold
} sMobileStates;

#define MOBILE_TOP 2950
#define MOBILE_BOTTOM 600
#define MOBILE_20 1000

#define MOBILE_UP_POWER 127
#define MOBILE_DOWN_POWER -127
#define MOBILE_UP_HOLD_POWER 10
#define MOBILE_DOWN_HOLD_POWER -10

short gMobileTarget;
word gMobileHoldPower;
sMobileStates gMobileState = mobileIdle;

void setMobile(word power)
{
	gMotor[mobile].power = power;
}

void handleMobile()
{
#ifdef MOBILE_LIFT_SAFETY
	if (gSensor[liftPoti].value > 2200)
	{
#endif
		if (RISING(Btn8U))
		{
			gMobileTarget = MOBILE_TOP;
			gMobileState = mobileRaise;
			gMobileHoldPower = MOBILE_UP_HOLD_POWER;
			setMobile(MOBILE_UP_POWER);
		}
		if (RISING(Btn8D))
		{
			gMobileTarget = MOBILE_BOTTOM;
			gMobileState = mobileLower;
			gMobileHoldPower = MOBILE_DOWN_HOLD_POWER;
			setMobile(MOBILE_DOWN_POWER);
		}
#ifdef MOBILE_LIFT_SAFETY
	}
#endif

	switch (gMobileState)
	{
		case mobileRaise:
		{
			if (gSensor[mobilePoti].value >= gMobileTarget) gMobileState = mobileHold;
			break;
		}
		case mobileLower:
		{
			if (gSensor[mobilePoti].value <= gMobileTarget) gMobileState = mobileHold;
			break;
		}
		case mobileHold:
		{
			setMobile(gMobileHoldPower);
			break;
		}
		case mobileIdle:
		{
			setMobile(0);
			break;
		}
	}
}


/* Macros */

int gNumCones = 0;
const int gStackPos[11] = { 0, 0, 0, 150, 320, 510, 720, 950, 1200, 1470, 1760 };

void stack()
{
void stack(bool callHandlers)
{
	gLiftTarget = LIFT_BOTTOM + gStackPos[gNumCones];
	gLiftState = liftRaise;
	while (gLiftTarget - gSensor[liftPoti].value > 100)
	{
		if (callHandlers) handleLift();
		sleep(10);
	}
	gArmTarget = gArmPositions[gArmPosition = 2];
	gArmState = armRaise;
	while (gArmTarget - gSensor[armPoti].value > 100)
	{
		if (callHandlers) handleArm();
		sleep(10);
	}
	sleep(200);
	setClaw(-127);
	sleep(500);
	if (gNumCones < 11) ++gNumCones;
	setClaw(0);
	gArmTarget = gArmPositions[gArmPosition = 1];
	gArmState = armLower;
	while (gArmTarget - gSensor[armPoti].value < -400)
	{
		if (callHandlers) handleArm();
		sleep(10);
	}
	gLiftTarget = LIFT_BOTTOM;
	gLiftState = liftLower;
	while (gLiftTarget - gSensor[liftPoti].value < -100)
	{
		if (callHandlers) handleLift();
		sleep(10);
	}
}

task stackAsync()
{
	stack(false);
	nSchedulePriority = 0;
}

#define OFFSET_20_ZONE_P1 700
#define OFFSET_20_ZONE_P2 850

void alignAndScore20(bool callHandlers)
{
	while (gMotor[driveL1].power > 0 || gMotor[driveR1].power > 0)
	{
		setDrive(gSensor[leftLine].value ? -10 : 50, gSensor[rightLine].value ? -10 : 50);
		sleep(10);
	}
	setDrive(127, 127);
	int initial = gSensor[driveEncL].value;
	while (gSensor[driveEncL].value < initial + OFFSET_20_ZONE_P1) sleep(10);
	gMobileTarget = MOBILE_20;
	gMobileState = mobileLower;
	gMobileHoldPower = 10;
	setMobile(MOBILE_DOWN_POWER);
	while (gSensor[driveEncL].value < initial + OFFSET_20_ZONE_P2)
	{
		if (callHandlers) handleMobile();
		sleep(10);
	}
	setDrive(-10, -10);
	while (gMobileState == mobileLower)
	{
		if (callHandlers) handleMobile();
		sleep(10);
	}
	sleep(200);
	setDrive(-127, -127);
	gMobileTarget = MOBILE_BOTTOM;
	gMobileState = mobileLower;
	gMobileHoldPower = MOBILE_DOWN_HOLD_POWER;
	setMobile(MOBILE_DOWN_POWER);
	while (gSensor[driveEncL].value > initial)
	{
		if (callHandlers) handleMobile();
		sleep(10);
	}
}

task alignAndScore20Async()
{
	alignAndScore20(false);
}

void handleMacros()
{
	if (RISING(Btn8R))
	{
		if (getTaskPriority(alignAndScore20Async)) stopTask(alignAndScore20Async);
		else startTask(alignAndScore20Async);
	}
	if (RISING(Btn7L))
	{
		if (getTaskPriority(stackAsync)) stopTask(stackAsync);
		else startTask(stackAsync);
	}
}


/* LCD */

void handleLcd()
{
	string line;
	sprintf(line, "%4d %4d", gSensor[driveEncL].value, gSensor[driveEncR].value);

	clearLCDLine(0);
	displayLCDString(0, 0, line);
}

// This function gets called 2 seconds after power on of the cortex and is the first bit of code that is run
void startup()
{
	// Setup and initilize the necessary libraries
	setupMotors();
	setupSensors();
	setupJoysticks();

	setupDgtIn(leftLine);
	setupDgtIn(rightLine);

	gJoy[TCHN].deadzone = TDZ;
	gJoy[PCHN].deadzone = PDZ;
	gJoy[LCHN].deadzone = LDZ;
	gJoy[ACHN].deadzone = ADZ;
}

// This function gets called every 25ms during disabled (DO NOT PUT BLOCKING CODE IN HERE)
void disabled()
{

}

// This task gets started at the begining of the autonomous period
task autonomous()
{
	startSensors(); // Initilize the sensors
}

// This task gets started at the beginning of the usercontrol period
task usercontrol()
{
	startSensors(); // Initilize the sensors
	initCycle(gMainCycle, 10);

	while (true)
	{
		updateSensorInputs();
		updateJoysticks();

		handleDrive();
		handleLift();
		handleArm();
		handleClaw();
		handleMobile();
		handleMacros();

		handleLcd();

		updateSensorOutputs();
		updateMotors();
		endCycle(gMainCycle);
	}
}
