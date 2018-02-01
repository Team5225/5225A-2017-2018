void selectAuto()
{
	updateSensorInput(autoPoti);
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
	autoSkills();
	return;
	selectAuto();
	writeDebugStreamLine("Selected auto: %s %d", gAlliance == allianceBlue ? "blue" : "red", gCurAuto);
	if (gAlliance == allianceBlue)
	{
		switch (gCurAuto)
		{
			case 0: autoSkills(); break;
			case 1: autoStationaryBlueLeft(); break;
			case 2: autoSideMobileLeft(); break;
		}
	}
	else
	{
		switch (gCurAuto)
		{
			case 0: autoTest(); break;
			case 1: autoStationaryRedRight(); break;
			case 2: autoSideMobileRight(); break;
		}
	}
}

void skillsRaiseMobile()
{
	setMobile(MOBILE_UP_POWER);
	unsigned long timeout = nPgmTime + 2000;
	while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(timeout, "skills/mobile")) sleep(10);
	setMobile(MOBILE_UP_HOLD_POWER);
}

void raiseMobileMid()
{
	setMobile(MOBILE_UP_POWER);
	unsigned long timeout = nPgmTime + 1000;
	while (gSensor[mobilePoti].value < MOBILE_MIDDLE_UP && !TimedOut(timeout, "mid")) sleep(10);
	setMobile(15);
}

void lowerMobile()
{
	setMobile(MOBILE_DOWN_POWER);
	unsigned long timeout = nPgmTime + 2000;
	while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(timeout, "lower")) sleep(10);
	setMobile(MOBILE_DOWN_HOLD_POWER);
}

void backupToLine()
{
	//setDrive(-30, -30);
	//while (!gSensor[leftLine].value && !gSensor[rightLine].value) sleep(10);
	//if (gSensor[leftLine].value)
	//{
	//	setDrive(7, -30);
	//	while (!gSensor[rightLine].value) sleep(10);
	//}
	//else
	//{
	//	setDrive(-30, 7);
	//	while (!gSensor[leftLine].value) sleep(10);
	//}
	//setDrive(7, 7);
	//resetPositionFull(gPosition, gPosition.y, gPosition.x, 235);
	//sleep(200);
	//setDrive(0, 0);
}

void driverSkillsStart()
{
	//resetPositionFull(gPosition, 19.5, 44.5, 45);
	//setMobile(MOBILE_DOWN_POWER);
	//unsigned long coneTimeout = nPgmTime + 2500;
	//while (gSensor[mobilePoti].value > MOBILE_BOTTOM + 1000 && !TimedOut(coneTimeout, "skills 1")) sleep(10);
	//byte async = moveToTargetAsync(36, 61, 19.5, 44.5, 127, 6, 2, 2, false, false);
	//unsigned long driveTimeout = nPgmTime + 4000;
	//while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(coneTimeout, "skills 2")) sleep(10);
	//setMobile(MOBILE_DOWN_HOLD_POWER);
	//await(async, driveTimeout, "skills 3");
	//sleep(500);
	//scoreFirstExternal(gPosition.a);
}

void normalize(float& x, float& y, float m, float b)
{
	float _b = y + x / m;
	x = (_b - b) / (m + 1 / m);
	y = m * x + b;
}

void resetBlueRight()
{
	setDrive(-30, -30);
	unsigned long timeout = nPgmTime + 5000;
	sleep(500);
	timeoutWhileLessThanF(&gVelocity.y, -0.1, timeout);
	setDrive(-7, -7);
	int count = 0;
	unsigned long value = 0;
	for (int i = 0; i < 10 || !count; ++i)
	{
		word cur = gSensor[sonar].value;
		if (cur > 1500 && cur < 3200)
		{
			value += cur;
			++count;
		}
		sleep(20);
	}
	resetPositionFull(gPosition, 8.25, 53.5 + (float)value / count / 148, 0);
}

NEW_ASYNC_VOID_0(resetBlueRight);

void autoSkills()
{
	byte driveAsync;
	byte coneAsync;
	unsigned long driveTimeout;
	unsigned long coneTimeout;
	sSimpleConfig liftConfig;
	sSimpleConfig armConfig;
	float _x;
	float _y;

	gMobileCheckLift = true;

#ifdef SKILLS_RESET_AT_START
	trackPositionTaskKill();
	resetPositionFull(gPosition, 40, 16, 45);
	resetVelocity(gVelocity, gPosition);
	trackPositionTaskAsync();
#endif

	liftSet(liftResetEncoder);
	coneTimeout = nPgmTime + 1400;

	// 1
	driveAsync = moveToTargetSimpleAsync(71, 47, 40, 16, 127, 24, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	liftTimeoutWhile(liftResetEncoder, coneTimeout);
	configure(liftConfig, LIFT_MOBILE_THRESHOLD, 127, -10);
	liftSet(liftRaiseSimple, &liftConfig);
	coneTimeout = nPgmTime + 1000;
	timeoutWhileLessThanL(&gSensor[liftEnc].value, LIFT_MOBILE_THRESHOLD, coneTimeout);
	mobileSet(mobileBottom, -1);
	coneTimeout = nPgmTime + 1000;
	timeoutWhileGreaterThanL(&gSensor[mobilePoti].value, MOBILE_BOTTOM + 200, coneTimeout);
	await(driveAsync, driveTimeout, "skills 1-1");
	setDrive(0, 0);
	driveAsync = moveToTargetSimpleAsync(71, 47, gPosition.y, gPosition.x, 127, 4, stopSoft, true);
	driveTimeout = nPgmTime + 1000;
	await(driveAsync, driveTimeout, "skills 1-2");
	mobileSet(mobileTop, -1);
	coneTimeout = nPgmTime + 2000;
	timeoutWhileLessThanL(&gSensor[mobilePoti].value, MOBILE_TOP - 200, coneTimeout);

	// 2
	driveAsync = turnToTargetNewAsync(39, 14, ch, 0);
	driveTimeout = nPgmTime + 5000;
	configure(liftConfig, LIFT_BOTTOM, -127, 0);
	liftSet(liftLowerSimple, &liftConfig);
	await(driveAsync, driveTimeout, "skills 2-1");
	driveAsync = moveToTargetSimpleAsync(39, 14, gPosition.y, gPosition.x, 80, 2, stopSoft, true);
	driveTimeout = nPgmTime + 2000;
	await(driveAsync, driveTimeout, "skills 2-2");
	mobileSet(mobileBottom, -1);
	coneTimeout = nPgmTime + 2000;
	timeoutWhileGreaterThanL(&gSensor[mobilePoti].value, MOBILE_BOTTOM + 200, coneTimeout);

	// 3
	driveAsync = moveToTargetDisSimpleAsync(gPosition.a + PI, 8, gPosition.y, gPosition.x, -80, 0, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 3-1");
	driveAsync = turnToTargetNewAsync(12, 60, cw, PI);
	driveTimeout = nPgmTime + 3000;
	configure(liftConfig, LIFT_BOTTOM, -127, 0);
	liftSet(liftLowerSimple, &liftConfig);
	await(driveAsync, driveTimeout, "skills 3-2");
	driveAsync = moveToTargetSimpleAsync(12, 60, gPosition.y, gPosition.x, -127, 4, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 3-3");
	driveAsync = turnToAngleNewAsync(0, cw);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 3-4");
	driveAsync = resetBlueRightAsync();
	driveTimeout = nPgmTime + 1000;
	await(driveAsync, driveTimeout, "skills 3-5");

	// 4
	driveAsync = moveToTargetDisSimpleAsync(0, 18, 0, gPosition.x, 80, 0, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 2000;
	await(driveAsync, driveTimeout, "skills 4-1");
	driveAsync = turnToTargetNewAsync(16, 107, cw, 0);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 4-2");
	driveAsync = moveToTargetSimpleAsync(16, 107, gPosition.y, gPosition.x, 80, 6, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 2000;
	await(driveAsync, driveTimeout, "skills 4-3");
	mobileSet(mobileTop, -1);
	coneTimeout = nPgmTime + 2000;
	timeoutWhileLessThanL(&gSensor[mobilePoti].value, MOBILE_TOP - 200, coneTimeout);

	// 5
	driveAsync = turnToTargetNewAsync(30, 40, ch, PI);
	driveTimeout = nPgmTime + 3000;
	configure(liftConfig, LIFT_BOTTOM, -127, 0);
	liftSet(liftLowerSimple, &liftConfig);
	await(driveAsync, driveTimeout, "skills 5-1");
	driveAsync = moveToTargetSimpleAsync(30, 40, gPosition.y, gPosition.x, -127, 4, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 5-2");
	driveAsync = turnToAngleNewAsync(-135, cw);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 5-3");
	_x = gPosition.x;
	_y = gPosition.y;
	normalize(_x, _y, -1, 56);
	driveAsync = moveToTargetDisSimpleAsync(-3.0 / 4 * PI, 10.5, _y, _x, 60, 0, stopNone, false);
	driveTimeout = nPgmTime + 1500;
	await(driveAsync, driveTimeout, "skills 5-4");
	driveAsync = moveToTargetDisSimpleAsync(-3.0 / 4 * PI, 15, _y, _x, 30, 0, stopNone, false);
	driveTimeout = nPgmTime + 1500;
	await(driveAsync, driveTimeout, "skills 5-5");
	setDrive(7, 7);
	mobileSet(mobileDownToMiddle, -1);
	coneTimeout = nPgmTime + 1500;
	mobileTimeoutUntil(mobileMiddle, coneTimeout);
	sleep(300);
	driveAsync = moveToTargetDisSimpleAsync(PI / 4, 17, gPosition.y, gPosition.x, -60, 0, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	setMobile(-7);
	await(driveAsync, driveTimeout, "skills 5-6");
	mobileSet(mobileManaged, 0);

	// 6
	driveAsync = turnToTargetNewAsync(12, 60, cw, PI);
	driveTimeout = nPgmTime + 3000;
	mobileSet(mobileBottom, 0);
	configure(liftConfig, LIFT_BOTTOM, -127, 0);
	liftSet(liftLowerSimple, &liftConfig);
	await(driveAsync, driveTimeout, "skills 6-1");
	driveAsync = moveToTargetSimpleAsync(12, 60, gPosition.y, gPosition.x, -127, 4, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 6-2");
	driveAsync = turnToAngleNewAsync(0, cw);
	driveTimeout = nPgmTime + 3000;
	await(driveAsync, driveTimeout, "skills 6-3");
	driveAsync = resetBlueRightAsync();
	driveTimeout = nPgmTime + 1000;
	await(driveAsync, driveTimeout, "skills 6-4");
}


void autoStationaryCore(bool first, int liftUp, int liftDown, tTurnDir turnDir)
{
	//unsigned long driveTimeout, coneTimeout;
	//byte async;
	//if (first)
	//{
	//	//grabPreload();
	//	setClaw(CLAW_CLOSE_HOLD_POWER);
	//	sleep(200);
	//	setArm(-80);
	//	coneTimeout = nPgmTime + 2000;
	//	while (gSensor[armPoti].value > 1800 && !TimedOut(coneTimeout, "preload")) sleep(10);
	//	setArm(80);
	//	sleep(300);
	//	setArm(10);
	//}
	//else
	//{
	//	turnToTargetAsync(48, 48, gPosition.y, gPosition.x, turnDir, 60, 60, true, true, 0);
	//	driveTimeout = nPgmTime + 1500;
	//	coneTimeout = nPgmTime + 1000;
	//	setArm(127);
	//	while (gSensor[armPoti].value < gArmPositions[2] && !TimedOut(coneTimeout, "sg core 1")) sleep(10);
	//	setArm(15);
	//	await(async, driveTimeout, "sg core 2");
	//}
	//async = moveToTargetAsync(48, 48, gPosition.y, gPosition.x, 80, 4, 1, 12, true, true);
	//driveTimeout = nPgmTime + 3000;
	//gLiftTarget = liftUp;
	//liftUpAsync();
	//coneTimeout = nPgmTime + 1000;
	//while (!gLiftTargetReached && !TimedOut(coneTimeout, "sg core 3")) sleep(10);
	//setLift(0);
	//await(async, driveTimeout, "sg core 4");
	//sleep(500);
	//setArm(-90);
	//coneTimeout = nPgmTime + 500;
	//while (gSensor[armPoti].value > (first ? 1850 : 1750) && !TimedOut(coneTimeout, "sg core 5")) sleep(10);
	//setArm(10);
	////sleep(500);
	//setLift(-50);
	//coneTimeout = nPgmTime + 800;
	//while (gSensor[liftPoti].value > liftDown && !TimedOut(coneTimeout, "sg core 6")) sleep(10);
	//setLift(0);
	//setClaw(CLAW_OPEN_POWER);
	//coneTimeout = nPgmTime + 500;
	//while (gSensor[clawPoti].value < CLAW_OPEN && !TimedOut(coneTimeout, "sg core 7")) sleep(10);
	//setClaw(-15);
	//setArm(127);
	//coneTimeout = nPgmTime + 500;
	//while (gSensor[armPoti].value < ARM_TOP - 100 && !TimedOut(coneTimeout, "sg core 8")) sleep(10);
	//setArm(10);
	//sleep(200);*/
}

void autoStationaryBlueLeft()
{
	//gPosition.y = 50;
	//gPosition.x = 13.5;
	//gPosition.a = 0;
	//autoStationaryCore(true, 2000, 1600, cw);
	//byte async = moveToTargetAsync(45, 25, gPosition.y, gPosition.x, -60, 6, 4, 4, false, true);
	//unsigned long driveTimeout = nPgmTime + 2000;
	//await(async, driveTimeout, "sg lb 1");
	//dropArmAsync();
	//sleep(200);
	//async = turnToTargetAsync(69, 22, gPosition.y, gPosition.x, ccw, 50, 50, true, true, 10);
	//driveTimeout = nPgmTime + 1500;
	//await(async, driveTimeout, "sg lb 2");
	//setLift(-80);
	//unsigned long coneTimeout = nPgmTime + 1000;
	//while (!gSensor[limBottom].value && !TimedOut(coneTimeout, "sg lb 3")) sleep(10);
	//setLift(-10);
	//async = moveToTargetAsync(69, 22, gPosition.y, gPosition.x, 60, 8, 0.5, 8, false, true);
	//driveTimeout = nPgmTime + 3000;
	//await(async, driveTimeout, "sg lb 4");
	//async = moveToTargetAsync(69, 22, gPosition.y, gPosition.x, 40, 6, 0.5, 1, true, true);
	//driveTimeout = nPgmTime + 1500;
	//sleep(200);
	//setClaw(CLAW_CLOSE_POWER);
	//coneTimeout = nPgmTime + 1000;
	//while (gSensor[clawPoti].value > CLAW_CLOSE + 200 && !TimedOut(coneTimeout, "sg lb 5")) sleep(10);
	//setClaw(CLAW_CLOSE_HOLD_POWER);
	//sleep(300);
	//await(async, driveTimeout, "sg lb 6");
	////gPosition.x += 1;
	//autoStationaryCore(false, 2250, 2100, cw);
	//moveToTarget(61, 35, 48, 48, -50, 6, 3, 3, false, true);
}

void autoStationaryRedRight()
{
	//gPosition.y = 13.5;
	//gPosition.x = 50;
	//gPosition.a = PI / 2;
	//autoStationaryCore(true, 2000, 1600, ccw);
	//byte async = moveToTargetAsync(25, 45, gPosition.y, gPosition.x, -40, 6, 4, 4, false, true);
	//unsigned long driveTimeout = nPgmTime + 2000;
	//await(async, driveTimeout, "sg rr 1");
	//dropArmAsync();
	//sleep(200);
	//async = turnToTargetAsync(23, 69, gPosition.y, gPosition.x, cw, 50, 50, true, true, -10);
	//driveTimeout = nPgmTime + 1500;
	//await(async, driveTimeout, "sg rr 2");
	//dropArmAsync();
	//setLift(-80);
	//unsigned long coneTimeout = nPgmTime + 1000;
	//while (!gSensor[limBottom].value && !TimedOut(coneTimeout, "sg rr 3")) sleep(10);
	//async = moveToTargetAsync(23, 69, gPosition.y, gPosition.x, 60, 8, 0.5, 8, false, true);
	//driveTimeout = nPgmTime + 3000;
	//await(async, driveTimeout, "sg rr 4");
	//async = moveToTargetAsync(23, 69, gPosition.y, gPosition.x, 40, 6, 0.5, 1, true, true);
	//driveTimeout = nPgmTime + 1500;
	//sleep(200);
	//setClaw(CLAW_CLOSE_POWER);
	//coneTimeout = nPgmTime + 1000;
	//while (gSensor[clawPoti].value > CLAW_CLOSE + 200 && !TimedOut(coneTimeout, "sg rr 5")) sleep(10);
	//setClaw(CLAW_CLOSE_HOLD_POWER);
	//sleep(300);
	//await(async, driveTimeout, "sg rr 6");
	//sleep(100);
	////gPosition.x += 2.5;
	////gPosition.y += 4.5;
	//autoStationaryCore(false, 2250, 2100, ccw);
	//moveToTarget(35, 61, 48, 48, -50, 6, 3, 3, false, true);
}

void autoSideMobileLeft()
{
	//gPosition.y = 50;
	//gPosition.x = 13.5;
	//gPosition.a = 0;
	//setClaw(CLAW_CLOSE_HOLD_POWER);
	//byte async = moveToTargetAsync(96, 13.5, 50, 13.5, 60, 4, 1.5, 1.5, false, true);
	//unsigned long driveTimeout = nPgmTime + 5000;
	//setMobile(MOBILE_DOWN_POWER);
	//unsigned long mobileTimeout = nPgmTime + 2000;
	//while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(mobileTimeout, "sm L 1")) sleep(10);
	//setMobile(MOBILE_DOWN_HOLD_POWER);
	//await(async, driveTimeout, "sm L 2");
	//sleep(300);
	//scoreFirstExternal(0);
	//skillsRaiseMobile();
	////setMobile(MOBILE_UP_POWER);
	////mobileTimeout = nPgmTime + 3000;
	////while (gSensor[mobilePoti].value < 1600 && !TimedOut(mobileTimeout, "sm L 2")) sleep(10);
	////gArmDown = false;
	////startTask(dropArm);
	////unsigned long coneTimeout = nPgmTime + 1500;
	////while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(mobileTimeout, "sm L 3")) sleep(10);
	////setMobile(MOBILE_UP_HOLD_POWER);
	////while (!gArmDown && !TimedOut(coneTimeout, "sm L 7")) sleep(10);
	////moveToTarget(107, 15, 60, 2, 1.5, 1.5, false, false);
	////sleep(100);
	////stack(false);
	////sleep(200);
	//moveToTarget(72, 15, -127, 6, 1.5, 3, false, false);
	//turnToTarget(40, 36, ccw, 80, 10, false, true, 180);
	//moveToTarget(40, 36, -80, 6, 4, 6, true, true);
	//sleep(200);
	//turnToTarget(0, 0, ccw, 60, 60, true, true, 0);
	//setMobile(MOBILE_DOWN_POWER);
	//unsigned long mobileTimeout = nPgmTime + 2000;
	//while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(mobileTimeout, "sm L 1")) sleep(10);
	//setMobile(MOBILE_DOWN_HOLD_POWER);
	//moveToTargetAwait(driveTimeout);
	//sleep(300);
	//scoreFirstExternal(0);
	//skillsRaiseMobile();
	////setMobile(MOBILE_UP_POWER);
	////mobileTimeout = nPgmTime + 3000;
	////while (gSensor[mobilePoti].value < 1600 && !TimedOut(mobileTimeout, "sm L 2")) sleep(10);
	////gArmDown = false;
	////startTask(dropArm);
	////unsigned long coneTimeout = nPgmTime + 1500;
	////while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(mobileTimeout, "sm L 3")) sleep(10);
	////setMobile(MOBILE_UP_HOLD_POWER);
	////while (!gArmDown && !TimedOut(coneTimeout, "sm L 7")) sleep(10);
	////moveToTarget(107, 15, 60, 2, 1.5, 1.5, false, false);
	////sleep(100);
	////stack(false);
	////sleep(200);
	//moveToTarget(72, 15, -127, 6, 1.5, 3, false, false);
	//turnToTarget(40, 36, ccw, 80, 10, false, true, 180);
	//moveToTarget(40, 36, -80, 6, 4, 6, true, true);
	//sleep(200);
	//turnToTarget(0, 0, ccw, 60, 60, true, true, 0);
	////setMobile(MOBILE_DOWN_POWER);
	////mobileTimeout = nPgmTime + 1000;
	////while (gSensor[mobilePoti].value > MOBILE_MIDDLE_DOWN && !TimedOut(mobileTimeout, "sm L 4")) sleep(10);
	////setMobile(15);
	//setDrive(100, 100);
	//sleep(1000);
	//setDrive(-127, -127);
	//driveTimeout = nPgmTime + 1000;
	//sleep(300);
	//while (!gSensor[leftLine].value && !gSensor[rightLine].value && !TimedOut(driveTimeout, "sm L 5")) sleep(10);
	//setDrive(0, 0);
}

void autoSideMobileRight()
{
	//gPosition.y = 13.5;
	//gPosition.x = 50;
	//gPosition.a = PI / 2;
	//setClaw(CLAW_CLOSE_HOLD_POWER);
	//byte async = moveToTargetAsync(13.5, 95, 13.5, 50, 60, 4, 1.5, 1.5, false, true);
	//unsigned long driveTimeout = nPgmTime + 5000;
	//setMobile(MOBILE_DOWN_POWER);
	//unsigned long mobileTimeout = nPgmTime + 2000;
	//while (gSensor[mobilePoti].value > MOBILE_BOTTOM && !TimedOut(mobileTimeout, "sm R 1")) sleep(10);
	//setMobile(MOBILE_DOWN_HOLD_POWER);
	//await(async, driveTimeout, "sm R 2");
	//sleep(300);
	//scoreFirstExternal(PI / 2);
	//skillsRaiseMobile();
	////setMobile(MOBILE_UP_POWER);
	////mobileTimeout = nPgmTime + 3000;
	////while (gSensor[mobilePoti].value < 1600 && !TimedOut(mobileTimeout, "sm R 2")) sleep(10);
	////gArmDown = false;
	////startTask(dropArm);
	////unsigned long coneTimeout = nPgmTime + 1500;
	////while (gSensor[mobilePoti].value < MOBILE_TOP && !TimedOut(mobileTimeout, "sm R 3")) sleep(10);
	////setMobile(MOBILE_UP_HOLD_POWER);
	////while (!gArmDown && !TimedOut(coneTimeout, "sm L 7")) sleep(10);
	////moveToTarget(gPosition.y, 108, 60, 2, 1.5, 1.5, false, false);
	////sleep(100);
	////stack(false);
	//sleep(200);
	//turnToTarget(0, 0, cw, 60, 60, true, true, 0);
	//setMobile(MOBILE_DOWN_POWER);
	//mobileTimeout = nPgmTime + 1000;
	//while (gSensor[mobilePoti].value > MOBILE_MIDDLE_DOWN && !TimedOut(mobileTimeout, "sm R 4")) sleep(10);
	//setMobile(15);
	//setDrive(100, 100);
	//sleep(1000);
	//setDrive(-127, -127);
	//driveTimeout = nPgmTime + 1000;
	//sleep(300);
	//while (!gSensor[leftLine].value && !gSensor[rightLine].value && !TimedOut(driveTimeout, "sm R 5")) sleep(10);
	//setDrive(0, 0);
}

void autoTest()
{
	byte driveAsync;
	byte coneAsync;
	unsigned long driveTimeout;
	unsigned long coneTimeout;
	sSimpleConfig liftConfig;
	sSimpleConfig armConfig;
	float _x;
	float _y;

	gMobileCheckLift = true;

	_x = gPosition.x;
	_y = gPosition.y;
	normalize(_x, _y, -1, 56);
	driveAsync = moveToTargetDisSimpleAsync(-3.0 / 4 * PI, 10.5, _y, _x, 60, 0, stopNone, false);
	driveTimeout = nPgmTime + 1500;
	await(driveAsync, driveTimeout, "skills 4-4");
	driveAsync = moveToTargetDisSimpleAsync(-3.0 / 4 * PI, 16, _y, _x, 30, 0, stopNone, false);
	driveTimeout = nPgmTime + 1500;
	await(driveAsync, driveTimeout, "skills 4-5");
	setDrive(7, 7);
	mobileSet(mobileDownToMiddle, -1);
	coneTimeout = nPgmTime + 1500;
	mobileTimeoutUntil(mobileMiddle, coneTimeout);
	sleep(300);
	driveAsync = moveToTargetDisSimpleAsync(PI / 4, 13, gPosition.y, gPosition.x, -60, 0, stopSoft | stopHarsh, true);
	driveTimeout = nPgmTime + 3000;
	sleep(300);
	mobileSet(mobileBottom, -1);
	coneTimeout = nPgmTime + 2000;
	await(driveAsync, driveTimeout, "skills 4-6");
	return;
	//for (int i = 0; i < 20; ++i)
	//{
	trackPositionTaskKill();
	resetPositionFull(gPosition, 0, 0, 0);
	trackPositionTaskAsync();
	//setDrive(80, 80);
	//unsigned long timeout = nPgmTime + 2000;
	//while (gPosition.y < 23 && !TimedOut(timeout, "test 1")) sleep(10);
	//setDrive(-15, -15);
	//sleep(200);
	//mobileSet(mobileDownToMiddle);
	//sleep(200);
	//setDrive(10, 10);
	//sleep(300);
	//setDrive(-80, -80);
	//timeout = nPgmTime + 1500;
	//while (gPosition.y > 10 && !TimedOut(timeout, "test 2")) sleep(10);
	//setDrive(0, 0);

	//moveToTargetSimple(72, 0, 127, 0);
	//turnToTargetSimple(0, -10, ccw, 127, 127, 0);
	//trackPositionTaskAsync();

	//moveToTargetSimple(50, 0, 127);
	//turnToTargetSimple(0, 0, ccw, 127, 127);
	//moveToTargetSimple(0, 0, 127, 10);
	//turnToAngleSimple(0, ccw, 127, 127);

	//moveToTargetSimple(5, 0, -80, 0, stopSoft | stopHarsh, true);
	//moveToTargetDisSimple(0, 5, -80, 0, stopSoft | stopHarsh, true);

	//turnToTargetNew(0, 100, cw, 0);
	//turnToTargetNew(100, 100, cw, 0);
	//playSound(soundBlip);
	//sleep(1000);
	//writeDebugStreamLine("%f", radToDeg(gPosition.a - atan2(100 - gPosition.x, 100 - gPosition.y)));
	//writeDebugStreamLine("%f", radToDeg(gPosition.a) - 45);
//}
	moveToTargetSimple(72, 0, 127, 0, stopSoft | stopHarsh, true);
}
