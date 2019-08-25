
/*页面模板*/

#include "filegroup.h"
#include "GUI_Private.h"
#include "math.h"
#include "buttons.h"

#include "resource/example.c"
#include "resource/backgound.c"
#include "resource/ui_editingacc.c"
#include "resource/ui_editingdec.c"
#include "resource/ui_editingspeed.c"
#include "resource/ui_fullfollow.c"
#include "resource/ui_halffollow.c"
#include "resource/ui_loading.c"
#include "resource/ui_lockmode.c"
#include "resource/ui_pauselogo.c"
#include "resource/ui_playlogo.c"
#include "resource/ui_playing.c"
#include "resource/ui_played.c"
#include "resource/ui_available.c"
#include "resource/ui_notavailable.c"

static bool IsFirst = 0;
static uint32_t LastMS = 0;

AttitudePoint_TypeDef NowAttitude;

static bool AttitudeCmp(AttitudePoint_TypeDef a, AttitudePoint_TypeDef b)
{
	if(
		abs(a.Yaw - b.Yaw) < 0.001 &&
		abs(a.Pitch - b.Pitch) < 0.001 &&
		abs(a.Roll - b.Roll) < 0.001
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* 返回一条以指定期望参数计算得到的实际加减速曲线的参数 */
static bool GetAccAnimationParams(
	AttitudePoint_TypeDef a,				// 起始位置
	AttitudePoint_TypeDef b,				// 终止位置
	float expectStartAcc,						// 期望加速段角加速度绝对值，单位 degree / (s^2)
	float expectStopAcc,						// 期望减速段角加速度绝对值，单位 degree / (s^2)
	float targetSpeed,							// 期望最大速度，单位 degree / s
	float *startDuration,						// 实际加速时间，单位 ms
	float *holdDuration,						// 实际匀速时间，单位 ms
	float *stopDuration,						// 实际减速时间，单位 ms
	float *maxSpeed									// 实际最大速度，单位 degree / s
)
{
	float change_yaw = a.Yaw - b.Yaw;
	float change_pitch = a.Pitch - b.Pitch;
	float change_roll = a.Roll - b.Roll;

	float distance = sqrt(change_yaw * change_yaw + change_pitch * change_pitch + change_roll * change_roll);

	float expectStartAcc_InMS = abs(expectStartAcc / 1000.0);
	float expectStopAcc_InMS = abs(expectStopAcc / 1000.0);

	float targetSpeedInDegreePerMS = targetSpeed / 1000.0;
	float targetStartDuration = targetSpeed / expectStartAcc_InMS;
	float targetStopDuration = targetSpeed / expectStopAcc_InMS;

	float targetAccDistance = (targetStartDuration + targetStopDuration) * targetSpeedInDegreePerMS / 2;

	float tmp_startDuration = 0;
	float tmp_stopDuration = 0;
	float tmp_maxSpeed = 0;
	float tmp_holdDuration = 0;

	if (targetAccDistance > distance)
		/* No holding time */
	{
		/* Triangle equals
		 */
		tmp_startDuration = sqrt(2 * distance * 1000 * expectStopAcc / (expectStartAcc * (expectStartAcc_InMS + expectStopAcc_InMS)));
		tmp_stopDuration = tmp_startDuration * expectStartAcc / expectStopAcc;
		tmp_maxSpeed = tmp_startDuration * expectStartAcc_InMS;

		*startDuration = tmp_startDuration;
		*stopDuration = tmp_stopDuration;
		*holdDuration = 0;
		*maxSpeed = tmp_maxSpeed;

		return false;
	}
	else
		/* Needs hold */
	{
		tmp_holdDuration = (distance - targetAccDistance) / targetSpeedInDegreePerMS;
		*startDuration = targetStartDuration;
		*stopDuration = targetStopDuration;
		*holdDuration = tmp_holdDuration;
		*maxSpeed = targetSpeed;

		return true;
	}
}

static int PushParams(int psize, void *p)
{
	int ret = 0;
	int pTimes = psize / 16;
	int pRemain = psize % 16;
	Client_Call("clearP", 0, NULL, NULL);
	for(int i = 0; i < pTimes; i++)
	{
		Client_Call("pushP", 16, &(((char*)p)[i * 16]), &ret);
	}
	if(pRemain)
	{
		Client_Call("pushP", pRemain, &(((char*)p)[pTimes * 16]), &ret);
	}
	return ret;
}

static void AnimationExample()
{
	int pCnt = 0;
	
	int playStatus = 0;
	
	/* 暂停当前播放的动画 */
	Client_Call("setPlayStatus", 1, &playStatus, NULL);
	
	/* 复位当前动画 */
	Client_Call("stop", 0, NULL, NULL);
	
	/* 清理资源文件 3项：点集、动画、序列 */
	Serial.printf("clearPoints");
	if(Client_Call("clearPoints", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	Serial.printf("clearAnimations");
	if(Client_Call("clearAnimations", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	Serial.printf("clearSequence");
	if(Client_Call("clearSequence", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	
	/* 准备播放轨迹的两个点位、注意这个点位是 TrackPoint */
	
	AttitudePoint_TypeDef pointA;
	pointA.Yaw = 0;
	pointA.Pitch = 0;
	pointA.Roll = 0;
	
	AttitudePoint_TypeDef pointB;
	pointB.Yaw = 60;
	pointB.Pitch = 20;
	pointB.Roll = 0;
	
	TrackPoint_TypeDef nodeA;
	nodeA.Attitude = pointA;					/* 节点位置使用 pointA 所提供的位置 */
	strcpy(nodeA.Name, "a");					/* 节点名称 "a" */
	nodeA.SettleTime = 1000;					/* 节点停留时间 1000ms */
	nodeA.TrackAnimationIndex = 0;		/* 运动到此节点使用的动画的索引号 0 */
	
	TrackPoint_TypeDef nodeB;
	nodeB.Attitude = pointB;					/* 节点位置使用 pointB 所提供的位置 */
	strcpy(nodeB.Name, "b");					/* 节点名称 "b" */
	nodeB.SettleTime = 1000;					/* 节点停留时间 1000ms */
	nodeB.TrackAnimationIndex = 0;		/* 运动到此节点使用的动画的索引号 0 */
	
	float startDuration = 0;
	float holdDuration = 0;
	float stopDuration = 0;
	float maxSpeed = 0;
	GetAccAnimationParams(pointA, pointB, 10, 10, 20, &startDuration, &holdDuration, &stopDuration, &maxSpeed);
	
	/* 准备动画 */
	TrackAnimation_TypeDef animation;
	animation.Duration = startDuration + holdDuration + stopDuration;
	strcpy(animation.Name, "animation1");
	animation.TrackTypeID = 1;
	animation.Params[0] = startDuration;
	animation.Params[1] = holdDuration;
	animation.Params[2] = stopDuration;
	animation.Params[3] = maxSpeed;
	
	/* 添加点位 A */
	int p_count = 0;
	int pushed = 0;
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackPoint_TypeDef), &nodeA));
	}
	while(pushed != 32);
	Client_Call("addPoint", 0, NULL, &p_count);
	Serial.printf("P Count:%d\r\n", p_count);
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackPoint_TypeDef), &nodeB));
	}
	while(pushed != 32);
	Client_Call("addPoint", 0, NULL, &p_count);
	Serial.printf("P Count:%d\r\n", p_count);
	
	int a_count = 0;
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackAnimation_TypeDef), &animation));
	}
	while(pushed != 32);
	Client_Call("addAnimation", 0, NULL, &a_count);
	Serial.printf("A Count:%d\r\n", a_count);
	
	/* 添加序列 */
	int s_count = 0;
	uint8_t sequences[] = { 0, 1, 0, 1, 0, 1, 0 };
	Client_Call("addSequence", sizeof(sequences), sequences, &s_count);
	delay(100);
	Serial.printf("S Count:%d\r\n", s_count);
	
	/* 设置播放最大 RPS */
	float rps = 5.0;
	Client_Call("setResourceRPS", sizeof(float), &rps, NULL);
	
	/* 加载资源 */
	int loadStatus = 0;
	do
	{
		loadStatus = 0x01;
		Client_Call("loadResource", 0, NULL, &loadStatus);
		if(!loadStatus)
		{
			Serial.printf("load succeed\r\n");
		}
		else
		{
			Serial.printf("load failed\r\n");
		}
	}
	while(loadStatus);
	
	/* 获取轨迹时长 */
	uint32_t duration = 0;
	Client_Call("getDuration", 0, NULL, &duration);
	Serial.printf("duration:%dms\r\n", duration);
	
	/* 切换到轨迹模式 */
	uint8_t mode = 1; 
	Client_Call("setMode", 1, &mode, NULL);
	
	/* 复位动画 */
	Client_Call("stop", 0, NULL, NULL);
	
	/* 开始播放 */
	playStatus = 1;
	Client_Call("setPlayStatus", 1, &playStatus, NULL);
	
	screen.setTextColor(screen.Black, screen.White);
	
	
	while(1)
	{
		AttitudePoint_TypeDef trackAttitude;
		Client_Call("getAttitude_t", 0, NULL, &trackAttitude);
		
		screen.setCursor(0, ContentArea_Y);
		screen.printf("YAW:%.0f   ", trackAttitude.Yaw);
		screen.setCursor(0, ContentArea_Y + TextHeight * 1);
		screen.printf("PIT:%.0f   ", trackAttitude.Pitch);
		screen.setCursor(0, ContentArea_Y + TextHeight * 2);
		screen.printf("ROL:%.0f   ", trackAttitude.Roll);
	}
}

#define TitleInfo_EditingSPD		1
#define TitleInfo_EditingACC		2
#define TitleInfo_EditingDEC		3
#define TitleInfo_FullFollow		4
#define TitleInfo_HalfFollow		5
#define TitleInfo_Loading				6
#define TitleInfo_LockMode			7
#define TitleInfo_Played				8
#define TitleInfo_Playing				9

static void UpdateTitle(uint32_t titleInfo)
{
	switch(titleInfo)
	{
		case TitleInfo_EditingACC:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_editingAcc, 128, TaskBarHeight);
			break;
		case TitleInfo_EditingDEC:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_editingDec, 128, TaskBarHeight);
			break;
		case TitleInfo_EditingSPD:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_editingSpeed, 128, TaskBarHeight);
			break;
		case TitleInfo_FullFollow:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_fullFollow, 128, TaskBarHeight);
			break;
		case TitleInfo_HalfFollow:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_halfFollow, 128, TaskBarHeight);
			break;
		case TitleInfo_Loading:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_loading, 128, TaskBarHeight);
			break;
		case TitleInfo_LockMode:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_lockMode, 128, TaskBarHeight);
			break;
		case TitleInfo_Played:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_played, 128, TaskBarHeight);
			break;
		case TitleInfo_Playing:
			screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_ui_playing, 128, TaskBarHeight);
			break;
	}
	Page_DrawBattery();
}

#define STATE_NormalMode				0
#define STATE_TrackMode					1

uint32_t NowState = 0;

AttitudePoint_TypeDef RecordPoints[9];
AttitudePoint_TypeDef PointFrom;
uint32_t PointTo   = 0;
bool PointsAvailable[9];

bool IsEditing = false;
uint32_t EditingIndex = 0;
uint32_t EditingFlashCnt = 0;

uint32_t StabMode = 0;
uint32_t PlayStatus = 0;
uint32_t Position = 0;
uint32_t Duration = 0;
float TrackDefaultAccel = 10;
float TrackDefaultSpeed = 20;
float TrackDefaultDecel = 10;

#define TrackDefault_MaxAccel		50
#define TrackDefault_MinAccel		5
#define TrackDefault_MaxDecel		50
#define TrackDefault_MinDecel		5
#define TrackDefault_MaxSpeed		100
#define TrackDefault_MinSpeed		5


static void BeginAnimation(int to)
{
	int pCnt = 0;
	
	int playStatus = 0;
	
	/* 暂停当前播放的动画 */
	Client_Call("setPlayStatus", 1, &playStatus, NULL);
	
	/* 复位当前动画 */
	Client_Call("stop", 0, NULL, NULL);
	
	/* 清理资源文件 3项：点集、动画、序列 */
	Serial.printf("clearPoints");
	if(Client_Call("clearPoints", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	Serial.printf("clearAnimations");
	if(Client_Call("clearAnimations", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	Serial.printf("clearSequence");
	if(Client_Call("clearSequence", 0, NULL, &pCnt))
	{
		Serial.printf("\tok\r\n");
	}
	else
	{
		Serial.printf("\tfailed\r\n");
	}
	
	/* 准备播放轨迹的两个点位、注意这个点位是 TrackPoint */
	
	AttitudePoint_TypeDef pointA = NowAttitude;
	
	AttitudePoint_TypeDef pointB = RecordPoints[to];
	
	TrackPoint_TypeDef nodeA;
	nodeA.Attitude = pointA;					/* 节点位置使用 pointA 所提供的位置 */
	strcpy(nodeA.Name, "a");					/* 节点名称 "a" */
	nodeA.SettleTime = 0;							/* 节点停留时间 0 */
	nodeA.TrackAnimationIndex = 0;		/* 运动到此节点使用的动画的索引号 0 */
	
	TrackPoint_TypeDef nodeB;
	nodeB.Attitude = pointB;					/* 节点位置使用 pointB 所提供的位置 */
	strcpy(nodeB.Name, "b");					/* 节点名称 "b" */
	nodeB.SettleTime = 0;							/* 节点停留时间 0 */
	nodeB.TrackAnimationIndex = 0;		/* 运动到此节点使用的动画的索引号 0 */
	
	float startDuration = 0;
	float holdDuration = 0;
	float stopDuration = 0;
	float maxSpeed = 0;
	GetAccAnimationParams(pointA, pointB, TrackDefaultAccel, TrackDefaultDecel, TrackDefaultSpeed, &startDuration, &holdDuration, &stopDuration, &maxSpeed);
	
	/* 准备动画 */
	TrackAnimation_TypeDef animation;
	animation.Duration = startDuration + holdDuration + stopDuration;
	strcpy(animation.Name, "animation1");
	animation.TrackTypeID = 1;
	animation.Params[0] = startDuration;
	animation.Params[1] = holdDuration;
	animation.Params[2] = stopDuration;
	animation.Params[3] = maxSpeed;
	
	/* 添加点位 A */
	int p_count = 0;
	int pushed = 0;
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackPoint_TypeDef), &nodeA));
	}
	while(pushed != 32);
	Client_Call("addPoint", 0, NULL, &p_count);
	Serial.printf("P Count:%d\r\n", p_count);
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackPoint_TypeDef), &nodeB));
	}
	while(pushed != 32);
	Client_Call("addPoint", 0, NULL, &p_count);
	Serial.printf("P Count:%d\r\n", p_count);
	
	int a_count = 0;
	do
	{
		pushed = 0;
		Serial.printf("push:%d\r\n", pushed = PushParams(sizeof(TrackAnimation_TypeDef), &animation));
	}
	while(pushed != 32);
	Client_Call("addAnimation", 0, NULL, &a_count);
	Serial.printf("A Count:%d\r\n", a_count);
	
	/* 添加序列 */
	int s_count = 0;
	uint8_t sequences[] = { 0, 1 };
	Client_Call("addSequence", sizeof(sequences), sequences, &s_count);
	delay(100);
	Serial.printf("S Count:%d\r\n", s_count);
	
	/* 设置播放最大 RPS */
	float rps = 5.0;
	Client_Call("setResourceRPS", sizeof(float), &rps, NULL);
	
	/* 加载资源 */
	int loadStatus = 0;
	do
	{
		loadStatus = 0x01;
		Client_Call("loadResource", 0, NULL, &loadStatus);
		if(!loadStatus)
		{
			Serial.printf("load succeed\r\n");
		}
		else
		{
			Serial.printf("load failed\r\n");
		}
	}
	while(loadStatus);
	
	/* 获取轨迹时长 */
	uint32_t duration = 0;
	Client_Call("getDuration", 0, NULL, &duration);
	Serial.printf("duration:%dms\r\n", duration);
	
	/* 切换到轨迹模式 */
	uint8_t mode = 1; 
	Client_Call("setMode", 1, &mode, NULL);
	
	/* 复位动画 */
	Client_Call("stop", 0, NULL, NULL);
	
	/* 开始播放 */
	playStatus = 1;
	Client_Call("setPlayStatus", 1, &playStatus, NULL);
}

static void DrawAttitude(int type, AttitudePoint_TypeDef p)
{
	int xpos = 16;
	switch(type)
	{
		case 0:
			xpos = 16;
		break;
		case 1:
			xpos = 53;
		break;
		case 2:
			xpos = 90;
		break;
	}
	screen.SetFontX(Fonts_BoldLight11px);
	screen.setTextColor(screen.Black, screen.White);
	screen.setCursor(xpos, 37);
	screen.printfX("%6.1f", p.Pitch);
	screen.setCursor(xpos, 49);
	screen.printfX("%6.1f", p.Roll);
	screen.setCursor(xpos, 61);
	screen.printfX("%6.1f", p.Yaw);
}
static void UpdatePointStatus()
{
	int x = 19;
	int y = 81;
	int dist = 17;
	for(int i = 0; i < 9; i++)
	{
		screen.drawRGBBitmap(x + (i % 3) * dist, y + (i / 3) * dist, (uint16_t *)(PointsAvailable[i] ? gImage_ui_available : gImage_ui_notAvailable), 9, 9);
	}
}
static void UpdateAttitude()
{
	switch(NowState)
	{
		case STATE_NormalMode:
			Client_Call("getAttitude_n", 0, NULL, &NowAttitude);
			DrawAttitude(0, PointFrom = NowAttitude);
			DrawAttitude(1, RecordPoints[PointTo]);
			DrawAttitude(2, NowAttitude);
		break;
		case STATE_TrackMode:
			Client_Call("getAttitude_t", 0, NULL, &NowAttitude);
			Client_Call("getPlayStatus", 0, NULL, &PlayStatus);
			if(PlayStatus)
			{
				DrawAttitude(0, PointFrom);
				DrawAttitude(1, RecordPoints[PointTo]);
				DrawAttitude(2, NowAttitude);
			}
			else
			{
				uint8_t mode = 0;
				DrawAttitude(0, PointFrom = NowAttitude);
				DrawAttitude(1, RecordPoints[PointTo]);
				DrawAttitude(2, NowAttitude);
				Client_Call("stop", 0, NULL, NULL);
				if(Client_Call("setMode", 1, &mode, NULL))
				{
					UpdateTitle(TitleInfo_LockMode);
					NowState = STATE_NormalMode;
				}
			}
			break;
		break;
	}
}

static void Stop()
{
	if(NowState == STATE_TrackMode)
	{
		Client_Call("stop", 0, NULL, NULL);
		UpdateTitle(TitleInfo_LockMode);
	}
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	Page_ClearScreen(screen.White);
	Page_DrawTaskBar();
	Page_DrawBattery();
	
	LastMS = millis();
	
	Client_Call("getAttitude_n", 0, NULL, &NowAttitude);
	
	screen.drawRGBBitmap(0, 0, (uint16_t *)gImage_backgound, 128, 160);
	
	UpdateTitle(TitleInfo_LockMode);
	//AnimationExample();
}

#define PITCH_RANGE		90
#define YAW_RANGE			9999
#define ROLL_RANGE		80

static void UpdateEditingTitle()
{
	if(IsEditing)
	{
		UpdateTitle(TitleInfo_EditingSPD + EditingIndex);
	}
	else
	{
		UpdateTitle(TitleInfo_LockMode);
	}
}
static void UpdateNormalMode()
{
	static uint32_t pitchStatusUseForEditing = 0;
	static uint32_t rollStatusUseForEditing = 0;
	uint32_t nowMS = millis();
	uint32_t interval = nowMS - LastMS;
	LastMS = nowMS;
	
	float PitchSpeed = 30;
	float YawSpeed = 20;
	float RollSpeed = 20;
	
	float PitchDelta = 0;
	float YawDelta = 0;
	float RollDelta = 0;
	
	if(abs(JS_R.X) > 100)
	{
		YawDelta -= interval * YawSpeed / 1000.0 * JS_R.X / 1000.0;
	}
	if(abs(JS_L.X) > 100)
	{
		RollDelta = interval * RollSpeed / 1000.0 * JS_L.X / 1000.0;
	}
	if(abs(JS_R.Y) > 100)
	{
		PitchDelta = interval * PitchSpeed / 1000.0 * JS_R.Y / 1000.0;
	}
	
	if(IsEditing)
	{
		switch(EditingIndex)
		{
			case 0:
				TrackDefaultSpeed -= YawDelta;
			break;
			case 1:
				TrackDefaultAccel -= YawDelta;
			break;
			case 2:
				TrackDefaultDecel -= YawDelta;
			break;
		}
		EditingFlashCnt = !YawDelta * EditingFlashCnt;
		if(pitchStatusUseForEditing == 0)
		{
			if(JS_R.Y > 800)
			{
				EditingIndex = (EditingIndex + 2) % 3;
				UpdateEditingTitle();
				pitchStatusUseForEditing = 1;
				
			}
			else if(JS_R.Y < -800)
			{
				EditingIndex = (EditingIndex + 1) % 3;
				UpdateEditingTitle();
				pitchStatusUseForEditing = 2;
			}
		}
		else if(pitchStatusUseForEditing == 1)
		{
			if(JS_R.Y < 300)
			{
				pitchStatusUseForEditing = 0;
			}
		}
		else if(pitchStatusUseForEditing == 2)
		{
			if(JS_R.Y > -300)
			{
				pitchStatusUseForEditing = 0;
			}
		}
		if(rollStatusUseForEditing == 0)
		{
			if(abs(JS_L.X) + abs(JS_L.Y) > 800)
			{
				rollStatusUseForEditing = 1;
			}
		}
		else if(rollStatusUseForEditing == 1)
		{
			if(abs(JS_L.X) + abs(JS_L.Y) < 100)
			{
				rollStatusUseForEditing = 0;
				IsEditing = false;
				UpdateEditingTitle();
			}
		}
	}
	else
	{
		NowAttitude.Yaw = constrain(NowAttitude.Yaw + YawDelta, -YAW_RANGE, YAW_RANGE);
		NowAttitude.Pitch = constrain(NowAttitude.Pitch + PitchDelta, -PITCH_RANGE, PITCH_RANGE);
		NowAttitude.Roll = constrain(NowAttitude.Roll + RollDelta, -ROLL_RANGE, ROLL_RANGE);
	}
	if(NowState)
	{
		
	}
	else
	{
		Client_Call("setAttitude_n", sizeof(AttitudePoint_TypeDef), &NowAttitude, NULL);
	}
}

static void DrawPlayPosition(uint32_t ms1, uint32_t ms2)
{
	screen.SetFontX(Fonts_BoldLight11px);
	screen.setTextColor(screen.White, screen.Black);
	screen.setCursor(14, 132);
	screen.printfX("%02d:%02d.%02d", constrain(ms1 / 1000 / 60, 0, 99), ms1 / 1000 % 60, ms1 / 10 % 100);
	screen.setCursor(14, 145);
	screen.printfX("%02d:%02d.%02d", constrain(ms2 / 1000 / 60, 0, 99), ms2 / 1000 % 60, ms2 / 10 % 100);
}

static void UpdatePlayPosition()
{
	switch(NowState)
	{
		case STATE_NormalMode:
			DrawPlayPosition(0, 0);
		break;
		case STATE_TrackMode:
			Client_Call("getPosition", 0, NULL, &Position);
			Client_Call("getDuration", 0, NULL, &Duration);
			DrawPlayPosition(Position, Duration);
		break;
	}
}

static void UpdatePlayStatus()
{
	switch(NowState)
	{
		case STATE_NormalMode:
			screen.drawRGBBitmap(66, 78, (uint16_t *)gImage_ui_pauseLogo, 62, 16);
		break;
		case STATE_TrackMode:
			screen.drawRGBBitmap(66, 78, (uint16_t *)(PlayStatus ? gImage_ui_playLogo : gImage_ui_pauseLogo), 62, 16);
		break;
	}
}

static void DrawTrackParams()
{
	int y0 = 99;
	int y1 = 110;
	int y2 = 122;
	
	uint32_t nowMS = millis();
	static uint32_t lastMS = nowMS;
	uint32_t interval = nowMS - lastMS;
	lastMS = nowMS;
	if(IsEditing)
	{
		EditingFlashCnt += interval;
	}
	else
	{
		EditingFlashCnt = 0;
	}
	
	TrackDefaultSpeed = constrain(TrackDefaultSpeed, TrackDefault_MinSpeed, TrackDefault_MaxSpeed);
	TrackDefaultAccel = constrain(TrackDefaultAccel, TrackDefault_MinAccel, TrackDefault_MaxAccel);
	TrackDefaultDecel = constrain(TrackDefaultDecel, TrackDefault_MinDecel, TrackDefault_MaxDecel);
	
	screen.drawFastHLine(69, y0-2, 36, screen.White);
	screen.drawFastHLine(69, y0-1, 36, screen.White);
	screen.drawFastHLine(69, y0  , 36, screen.Black);
	screen.drawFastHLine(69, y0+1, 36, screen.White);
	screen.drawFastHLine(69, y0+2, 36, screen.White);
	if(!IsEditing || EditingIndex != 0 || EditingFlashCnt % 600 < 300)
	{
		screen.drawFastVLine(map(TrackDefaultSpeed, TrackDefault_MinSpeed, TrackDefault_MaxSpeed, 69, 103), y0-2, 5, screen.Black);
		screen.drawFastVLine(map(TrackDefaultSpeed, TrackDefault_MinSpeed, TrackDefault_MaxSpeed, 70, 104), y0-2, 5, screen.Black);
	}
	
	screen.drawFastHLine(69, y1-2, 36, screen.White);
	screen.drawFastHLine(69, y1-1, 36, screen.White);
	screen.drawFastHLine(69, y1  , 36, screen.Black);
	screen.drawFastHLine(69, y1+1, 36, screen.White);
	screen.drawFastHLine(69, y1+2, 36, screen.White);
	if(!IsEditing || EditingIndex != 1 || EditingFlashCnt % 600 < 300)
	{
		screen.drawFastVLine(map(TrackDefaultAccel, TrackDefault_MinAccel, TrackDefault_MaxAccel, 69, 103), y1-2, 5, screen.Black);
		screen.drawFastVLine(map(TrackDefaultAccel, TrackDefault_MinAccel, TrackDefault_MaxAccel, 70, 104), y1-2, 5, screen.Black);
	}
		
	screen.drawFastHLine(69, y2-2, 36, screen.White);
	screen.drawFastHLine(69, y2-1, 36, screen.White);
	screen.drawFastHLine(69, y2  , 36, screen.Black);
	screen.drawFastHLine(69, y2+1, 36, screen.White);
	screen.drawFastHLine(69, y2+2, 36, screen.White);
	if(!IsEditing || EditingIndex != 2 || EditingFlashCnt % 600 < 300)
	{
		screen.drawFastVLine(map(TrackDefaultDecel, TrackDefault_MinDecel, TrackDefault_MaxDecel, 69, 103), y2-2, 5, screen.Black);
		screen.drawFastVLine(map(TrackDefaultDecel, TrackDefault_MinDecel, TrackDefault_MaxDecel, 70, 104), y2-2, 5, screen.Black);
	}
	
//	if(IsEditing)
//	{
//		switch(EditingIndex)
//		{
//			case 0:
//				UpdateTitle(TitleInfo_EditingSPD);
//			break;
//			case 1:
//				UpdateTitle(TitleInfo_EditingACC);
//			break;
//			case 2:
//				UpdateTitle(TitleInfo_EditingDEC);
//			break;
//		}
//	}
	
	screen.setTextColor(screen.Black, screen.White);
	
	screen.SetFontX(Fonts_BoldLight11px);
	screen.setCursor(107, 94);
	screen.printfX("%3.0f", TrackDefaultSpeed);
	
	screen.SetFontX(Fonts_BoldLight11px);
	screen.setCursor(107, 106);
	screen.printfX("%3.0f", TrackDefaultAccel);
	
	screen.SetFontX(Fonts_BoldLight11px);
	screen.setCursor(107, 118);
	screen.printfX("%3.0f", TrackDefaultDecel);
}
static void UpdateTrackParams()
{
	DrawTrackParams();
}

static void UpdatePointRecord()
{
	for(int i = 0; i < 9; i++)
	{
		uint8_t clicked = Buttons_IsClicked(i);
		if(Buttons_IsLongPressed(i))
		{
			bool equ = false;
			for(int a = 0; a < 9; a++)
			{
				if(PointsAvailable[a])
				{
					if(AttitudeCmp(RecordPoints[a], NowAttitude))
					{
						equ = true;
						break;
					}
				}
			}
			if(!equ)
			{
				RecordPoints[i] = NowAttitude;
				PointsAvailable[i] = true;
			}
		}
		else if(clicked == 2)
		{
			PointsAvailable[i] = false;
		}
		else if(clicked == 1)
		{
			if(PointsAvailable[i])
			{
				UpdateTitle(TitleInfo_Loading);
				Page_DrawBattery();
				BeginAnimation(i);
				NowState = STATE_TrackMode;
				UpdateTitle(TitleInfo_Playing);
			}
		}
	}
	uint8_t longPressed = Buttons_IsLongPressed(9);
	if(longPressed)
	{
		Stop();
	}
	else
	{
		uint8_t clickTimes = Buttons_IsClicked(9);
		if(clickTimes == 1)
		{
			
		}
		else if(clickTimes == 2)
		{
			IsEditing = true;
			UpdateEditingTitle();
		}
	}
}

static void Loop()
{
	UpdateNormalMode();
	Page_DrawBattery();
	
	UpdateAttitude();
	Page_DrawBattery();
	
	UpdatePointRecord();
	Page_DrawBattery();
	
	UpdatePointStatus();
	Page_DrawBattery();
	
	UpdatePlayPosition();
	Page_DrawBattery();
	
	UpdatePlayStatus();
	Page_DrawBattery();
	
	UpdateTrackParams();
	Page_DrawBattery();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
}

/************ 事件处理 **********/
/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
	
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
}

/**
  * @brief  旋转编码器事件
  * @param  无
  * @retval 无
  */
static void EncoderEvent()
{
	if(ecd>0)
	{
		
	}
	else
	{
		
	}
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Main(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    
    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
