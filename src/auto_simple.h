#define DATALOG_TURN -1
#define DATALOG_SWEEP -1

/* Enumerations */
typedef enum _stopType
{
	stopNone =		0b00000000,
	stopSoft =		0b00000001,
	stopHarsh =		0b00000010
} tStopType;

typedef enum _mttMode
{
	mttSimple,
	mttProportional,
	mttCascading
} tMttMode;

/* Variables */
sVector gTargetLast;

/* Functions */
void moveToTargetSimple(float y, float x, float ys, float xs, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
void moveToTargetDisSimple(float a, float d, float ys, float xs, byte power, byte startPower, float maxErrX, float decelEarly, byte decelPower, float dropEarly = 0, tStopType stopType = stopSoft | stopHarsh, tMttMode mode = mttProportional);
void turnToAngleNewAlg(float a, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true);
void turnToTargetNewAlg(float y, float x, tTurnDir turnDir, float fullRatio, byte coastPower, float stopOffsetDeg, bool mogo = false, bool harshStop = true, float offset = 0);
void sweepTurnToTarget(float y, float x, float a, float r, tTurnDir turnDir, byte power, bool slow = true);


ADD_FUNCS_TO_MACHINE_5(drive, moveToTargetSimple, moveToTargetDisSimple, turnToAngleNewAlg, turnToTargetNewAlg, sweepTurnToTarget);

PREP_FUNC_STATE_12(moveToTargetSimple, float, float, float, float, byte, byte, float, float, byte, float, tStopType, tMttMode);
PREP_FUNC_STATE_12(moveToTargetDisSimple, float, float, float, float, byte, byte, float, float, byte, float, tStopType, tMttMode);
PREP_FUNC_STATE_7(turnToAngleNewAlg, float, tTurnDir, float, byte, float, bool, bool);
PREP_FUNC_STATE_9(turnToTargetNewAlg, float, float, tTurnDir, float, byte, float, bool, bool, float);
PREP_FUNC_STATE_7(sweepTurnToTarget, float, float, float, float, tTurnDir, byte, bool);
