void selectAuto()
{
	int autoVal = gSensor[autoPoti].value - 2048;
	if (autoVal < 0) gAlliance = allianceBlue;
	else gAlliance = allianceRed;

	autoVal = abs(autoVal);
	if (autoVal < 300) gCurAuto = 0;
	else if (autoVal < 600) gCurAuto = 1;
	else if (autoVal < 900) gCurAuto = 2;
	else if (autoVal < 1200) gCurAuto = 3;
	else if (autoVal < 1500) gCurAuto = 4;
	else if (autoVal < 1800) gCurAuto = 5;
	else gCurAuto = 6;
}

void runAuto()
{
	autoSideMobileLeft();
	return;
	if (gAlliance == allianceBlue)
	{
		switch (gCurAuto)
		{
			case 0: autoSkills(); break;
			case 1: autoStationaryBlueLeft(); break;
		}
	}
	else
	{
		switch (gCurAuto)
		{
			case 0: break;
			default: break;
		}
	}
}

void autoSkills()
{
}

void autoStationaryCore(bool first, int liftUp, int liftDown, tTurnDir turnDir)
{
	unsigned long driveTimeout, coneTimeout;
	if (first)
	{
		hogCPU();
		gPosition.x = 30.5;
		gPosition.y = 30.5;
		gPosition.a = PI / 4;
		releaseCPU();
		//grabPreload();
		setClaw(CLAW_CLOSE_HOLD_POWER);
		sleep(200);
		setArm(-80);
		coneTimeout = nPgmTime + 1500;
		while (gSensor[armPoti].value > 1800 && !TimedOut(coneTimeout, "preload")) sleep(10);
		setArm(80);
		sleep(300);
		setArm(10);
	}
	else
	{
		turnToTargetAsync(48, 48, gPosition.y, gPosition.x, turnDir, 60, 60, true, true, 0);
		driveTimeout = nPgmTime + 1500;
		coneTimeout = nPgmTime + 1000;
		setArm(127);
		while (gSensor[armPoti].value < gArmPositions[2] && !TimedOut(coneTimeout, "sg core 1")) sleep(10);
		setArm(15);
		turnToTargetAwait(driveTimeout);
	}
	moveToTargetAsync(48, 48, gPosition.y, gPosition.x, 40, 4, 1, 12, true, true);
	driveTimeout = nPgmTime + 3000;
	gLiftTarget = liftUp;
	tStart(LiftTaskUp);
	coneTimeout = nPgmTime + 1000;
	while (!gLiftTargetReached && !TimedOut(coneTimeout, "sg core 2")) sleep(10);
	setLift(-10);
	moveToTargetAwait(driveTimeout);
	sleep(500);
	setArm(-90);
	coneTimeout = nPgmTime + 500;
	while (gSensor[armPoti].value > (first ? 1850 : 1750) && !TimedOut(coneTimeout, "sg core 3")) sleep(10);
	setArm(10);
	sleep(500);
	setLift(-50);
	coneTimeout = nPgmTime + 800;
	while (gSensor[liftPoti].value > liftDown && !TimedOut(coneTimeout, "sg core 4")) sleep(10);
	setLift(0);
	setClaw(CLAW_OPEN_POWER);
	coneTimeout = nPgmTime + 500;
	while (gSensor[clawPoti].value < CLAW_OPEN && !TimedOut(coneTimeout, "sg core 5")) sleep(10);
	setClaw(-15);
	setArm(127);
	coneTimeout = nPgmTime + 500;
	while (gSensor[armPoti].value < ARM_TOP - 100 && !TimedOut(coneTimeout, "sg core 6")) sleep(10);
	setArm(10);
	sleep(200);
}

void autoStationaryBlueLeft()
{
	autoStationaryCore(true, 2200, 1600, cw);
	moveToTargetAsync(33, 35, 38.5, 38.5, -40, 6, 4, 4, false, true);
	unsigned long driveTimeout = nPgmTime + 2000;
	moveToTargetAwait(driveTimeout);
	sleep(200);
	turnToTargetAsync(63, 23, gPosition.y, gPosition.x, ccw, 60, 60, true, true, 0);
	driveTimeout = nPgmTime + 1500;
	turnToTargetAwait(driveTimeout);
	sleep(200);
	moveToTargetAsync(63, 23, gPosition.y, gPosition.x, 60, 4, 1, 8, false, true);
	driveTimeout = nPgmTime + 3000;
	tStart(dropArm);
	setLift(-80);
	unsigned long coneTimeout = nPgmTime + 1000;
	while (!gSensor[limBottom].value && !TimedOut(coneTimeout, "sg lb 1")) sleep(10);
	setLift(-10);
	moveToTargetAwait(driveTimeout);
	moveToTargetAsync(63, 23, 33, 35, 40, 4, 1, 1, false, true);
	driveTimeout = nPgmTime + 1000;
	sleep(200);
	setClaw(CLAW_CLOSE_POWER);
	coneTimeout = nPgmTime + 1000;
	while (gSensor[clawPoti].value > CLAW_CLOSE && !TimedOut(coneTimeout, "sg lb 2")) sleep(10);
	setClaw(CLAW_CLOSE_HOLD_POWER);
	sleep(300);
	moveToTargetAwait(driveTimeout);
	gPosition.x += 1;
	autoStationaryCore(false, 2250, 2100, cw);
	moveToTarget(61, 35, 48, 48, -50, 6, 3, 3, false, true);
}

void autoStationaryRedRight()
{
	autoStationaryCore(true, 2150, 1600, ccw);
	moveToTargetAsync(35, 33, 38.5, 38.5, -40, 6, 4, 4, false, true);
	unsigned long driveTimeout = nPgmTime + 2000;
	moveToTargetAwait(driveTimeout);
	sleep(200);
	turnToTargetAsync(25, 58, gPosition.y, gPosition.x, cw, 60, 60, true, true, 0);
	driveTimeout = nPgmTime + 1500;
	turnToTargetAwait(driveTimeout);
	sleep(200);
	moveToTargetAsync(25, 58, gPosition.y, gPosition.x, 60, 4, 1, 8, false, true);
	driveTimeout = nPgmTime + 3000;
	tStart(dropArm);
	setLift(-80);
	unsigned long coneTimeout = nPgmTime + 1000;
	while (!gSensor[limBottom].value && !TimedOut(coneTimeout, "sg lb 1")) sleep(10);
	setLift(-10);
	moveToTargetAwait(driveTimeout);
	moveToTargetAsync(25, 58, 35, 33, 40, 4, 1, 1, false, true);
	driveTimeout = nPgmTime + 1000;
	sleep(200);
	setClaw(CLAW_CLOSE_POWER);
	coneTimeout = nPgmTime + 1000;
	while (gSensor[clawPoti].value > CLAW_CLOSE && !TimedOut(coneTimeout, "sg lb 2")) sleep(10);
	setClaw(CLAW_CLOSE_HOLD_POWER);
	sleep(300);
	moveToTargetAwait(driveTimeout);
	sleep(100);
	gPosition.x += 2.5;
	gPosition.y += 4.5;
	autoStationaryCore(false, 2250, 2100, ccw);
	moveToTarget(35, 61, 48, 48, -50, 6, 3, 3, false, true);
}

void autoSideMobileLeft()
{
	gPosition.y = 50;
	gPosition.x = 13.5;
	gPosition.a = 0;
	setClaw(CLAW_CLOSE_HOLD_POWER);
	moveToTargetAsync(96, 13.5, 50, 13.5, 60, 4, 1.5, 1.5, false, true);
	unsigned long driveTimeout = nPgmTime + 5000;
	setMobile(MOBILE_DOWN_POWER);
	unsigned long mobileTimeout = nPgmTime + 2000;
	while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(mobileTimeout, "sm L 1")) sleep(10);
	setMobile(MOBILE_DOWN_HOLD_POWER);
	moveToTargetAwait(driveTimeout);
	sleep(300);
	scoreFirstExternal(0);
	setMobile(MOBILE_UP_POWER);
	mobileTimeout = nPgmTime + 3000;
	while (gSensor[mobilePoti].value < 1600 && !TimedOut(mobileTimeout, "sm L 2")) sleep(10);
	gArmDown = false;
	startTask(dropArm);
	unsigned long coneTimeout = nPgmTime + 1500;
	while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(mobileTimeout, "sm L 3")) sleep(10);
	setMobile(MOBILE_UP_HOLD_POWER);
	while (!gArmDown && !TimedOut(coneTimeout, "sm L 7")) sleep(10);
	moveToTarget(107, 15, 60, 2, 1.5, 1.5, false, false);
	sleep(100);
	stack(false);
	sleep(200);
	moveToTarget(72, 15, -127, 6, 1.5, 3, false, false);
	turnToTarget(46, 36, ccw, 80, 10, false, true, 180);
	moveToTarget(46, 36, -80, 6, 4, 6, true, true);
	sleep(200);
	turnToTarget(0, 0, ccw, 60, 60, true, true, 0);
	setMobile(MOBILE_DOWN_POWER);
	mobileTimeout = nPgmTime + 1000;
	while (gSensor[mobilePoti].value > MOBILE_MIDDLE_DOWN && !TimedOut(mobileTimeout, "sm L 4")) sleep(10);
	setMobile(15);
	setDrive(100, 100);
	sleep(1000);
	setDrive(-127, -127);
	driveTimeout = nPgmTime + 1000;
	sleep(300);
	while (gSensor[leftLine].value > 1600 && gSensor[rightLine].value > 1600 && !TimedOut(driveTimeout, "sm L 5")) sleep(10);
	setDrive(0, 0);
}

void autoSideMobileRight()
{
	gPosition.y = 13.5;
	gPosition.x = 50;
	gPosition.a = PI / 2;
	setClaw(CLAW_CLOSE_HOLD_POWER);
	moveToTargetAsync(13.5, 95, 13.5, 50, 60, 4, 1.5, 1.5, false, true);
	unsigned long driveTimeout = nPgmTime + 5000;
	setMobile(MOBILE_DOWN_POWER);
	unsigned long mobileTimeout = nPgmTime + 2000;
	while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(mobileTimeout, "sm R 1")) sleep(10);
	setMobile(MOBILE_DOWN_HOLD_POWER);
	moveToTargetAwait(driveTimeout);
	sleep(300);
	scoreFirstExternal(PI / 2);
	setMobile(MOBILE_UP_POWER);
	mobileTimeout = nPgmTime + 3000;
	while (gSensor[mobilePoti].value < 1600 && !TimedOut(mobileTimeout, "sm R 2")) sleep(10);
	gArmDown = false;
	startTask(dropArm);
	unsigned long coneTimeout = nPgmTime + 1500;
	while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(mobileTimeout, "sm R 3")) sleep(10);
	setMobile(MOBILE_UP_HOLD_POWER);
	while (!gArmDown && !TimedOut(coneTimeout, "sm L 7")) sleep(10);
	moveToTarget(gPosition.y, 108, 60, 2, 1.5, 1.5, false, false);
	sleep(100);
	stack(false);
	sleep(200);
	moveToTarget(15, 72, -127, 6, 1.5, 3, false, false);
	turnToTarget(34, 46, cw, 10, 80, false, true, 180);
	moveToTarget(34, 46, -80, 6, 4, 6, true, true);
	sleep(200);
	turnToTarget(0, 0, cw, 60, 60, true, true, 0);
	setMobile(MOBILE_DOWN_POWER);
	mobileTimeout = nPgmTime + 1000;
	while (gSensor[mobilePoti].value > MOBILE_MIDDLE_DOWN && !TimedOut(mobileTimeout, "sm R 4")) sleep(10);
	setMobile(15);
	setDrive(100, 100);
	sleep(1000);
	setDrive(-127, -127);
	driveTimeout = nPgmTime + 1000;
	sleep(300);
	while (gSensor[leftLine].value > 1600 && gSensor[rightLine].value > 1600 && !TimedOut(driveTimeout, "sm R 5")) sleep(10);
	setDrive(0, 0);
}

void autoTest()
{
	gPosition.x = gPosition.y = gPosition.a = 0;
	moveToTarget(15, 0, 0, 0, 127, 6, 1, 2, false, true);
	moveToTarget(25, 10, 15, 0, 127, 6, 1, 2, false, true);
	moveToTarget(60, -20, 25, 10, 127, 8, 2, 2, true, true);
}