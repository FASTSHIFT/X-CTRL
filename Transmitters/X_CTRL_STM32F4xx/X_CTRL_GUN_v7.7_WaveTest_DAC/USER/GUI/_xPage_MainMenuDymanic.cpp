#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_MainMenu(3);

/*XFS*/
static uint32_t StayPageTimePoint;
static bool IsSpoke;

/*UI*/

typedef struct{
	int8_t Now;
	int8_t Last;
	int8_t Next;
}State_TypeDef;

typedef enum{
	Play_1,
	Play_2,
	Stop
}PlayerState_Type;

State_TypeDef PlayerState_Ctrl;
State_TypeDef PlayerState_Option;
State_TypeDef CursorSelect;

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_CursorUpdate()
{
	screen.setCursor(0,120);
	screen.setTextColor(screen.White,screen.Black);
	screen.print(CursorSelect.Now);
}

static void Task_PlayVideoFast()
{
	switch(PlayerState_Ctrl.Now)
	{
		case Play_1:
			playerCtrl_1.UpdatePlayerState();
			if(playerCtrl_1 == playerCtrl_1.Stop)
			{
				PlayerState_Ctrl.Last = Play_1;
				PlayerState_Ctrl.Now = Stop;
				PlayerState_Ctrl.Next = Play_2;
			}
			break;
			
		case Play_2:
			playerCtrl_2.UpdatePlayerState();
			if(playerCtrl_2 == playerCtrl_2.Stop)
			{
				PlayerState_Ctrl.Last = Play_2;
				PlayerState_Ctrl.Now = Stop;
				PlayerState_Ctrl.Next = Play_1;
			}
			break;
			
		case Stop:
			
			break;
	}
	
	switch(PlayerState_Option.Now)
	{
		case Play_1:
			playerOption_1.UpdatePlayerState();
			if(playerOption_1 == playerOption_1.Stop)
			{
				PlayerState_Option.Last = Play_1;
				PlayerState_Option.Now = Stop;
				PlayerState_Option.Next = Play_2;
			}
			break;
			
		case Play_2:
			playerOption_2.UpdatePlayerState();
			if(playerOption_2 == playerOption_2.Stop)
			{
				PlayerState_Option.Last = Play_2;
				PlayerState_Option.Now = Stop;
				PlayerState_Option.Next = Play_1;
			}
			break;
			
		case Stop:
			
			break;
	}
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    StayPageTimePoint = millis();//记录页面进入时间戳

    IsSpoke = false;

	PlayerState_Ctrl.Now = Play_1;
    PlayerState_Option.Now = Play_1;
	playerCtrl_1.SetPlayerState(playerCtrl_1.Run);
	playerOption_1.SetPlayerState(playerOption_1.Run);
    /*任务注册*/
    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 50);
    mtm_MainMenu.TaskRegister(1, Task_PlayVideoFast, 30);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
	/*XFS*/
    if(!IsSpoke && millis() - StayPageTimePoint > 10000)
    {
        if(BattUsage < 30)
            XFS_Speak("警告：电池电量低。");

        XFS_Speak(
            "当前电池电压%0.1f伏，剩余电量%0.0f%%",
            BattVoltage,
            fmap(BattVoltage, BattEmptyVoltage, BattFullVoltage, 0.0, 100.0)
        );

        if(BattUsage < 20)
            XFS_Speak("请立即充电");
        else if(BattUsage < 30)
            XFS_Speak("请及时充电");

        IsSpoke = true;
    }
	
	mtm_MainMenu.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btUP && CursorSelect.Now == 0)
	{
		CursorSelect.Now = 1;
	}
    if(btDOWN && CursorSelect.Now == 1)
	{
		CursorSelect.Now = 0;
	}
	
	if(CursorSelect.Now != CursorSelect.Last)
	{
		if(CursorSelect.Now == 0)
		{
			playerCtrl_2.SetPlayerState(playerCtrl_2.Run);
			PlayerState_Ctrl.Now = Play_2;
			playerOption_1.SetPlayerState(playerOption_1.Run);
			PlayerState_Option.Now = Play_1;
		}
		else
		{
			playerCtrl_1.SetPlayerState(playerCtrl_1.Run);
			PlayerState_Ctrl.Now = Play_1;
			playerOption_2.SetPlayerState(playerOption_2.Run);
			PlayerState_Option.Now = Play_2;
		}
		
		CursorSelect.Last = CursorSelect.Now;
	}

    if(btEcd || btOK)
    {
        if(CursorSelect.Now == 0)
        {
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorSelect.Now == 1)
        {
            page.PageChangeTo(PAGE_Options);
        }
    }

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
}

/**
  * @brief  主菜单页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
