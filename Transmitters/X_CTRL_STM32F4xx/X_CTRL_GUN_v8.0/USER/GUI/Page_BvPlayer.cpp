#include "FileGroup.h"
#include "FileSystem.h"
#include "GUI_Private.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_BvPlayer(2, true);

String BvFilePath;
static int16_t PlayIntervalTime = 20;

/*ProgressBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint8_t style)*/
static LightGUI::ProgressBar<SCREEN_CLASS> BvPrg(&screen, 5, screen.height() - 10, screen.width() - 10, 6, 0, 0);

static void Task_UpdateProgress()
{
    BvPrg.setProgress((float)BvPlayer.NowFrame/BvPlayer.EndFrame);
}

static void Task_UpdateBvState()
{
    BvPlayer.UpdatePlayerState();
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    memset(BV_Buffer, 0, sizeof(BV_Buffer));
    if(!BvPlayer.OpenVideo(BvFilePath))
    {
        return;
    }
    BvPlayer.BaseX = (screen.width() - BvPlayer.Head.Width) * 0.5f;
    BvPlayer.BaseY = (screen.height() - StatusBar_POS - BvPlayer.Head.Height) * 0.5f + StatusBar_POS;
    BvPlayer.SetPlayerState(BvPlayer.Always);
    
    mtm_BvPlayer.TaskRegister(0, Task_UpdateProgress, 100);
    mtm_BvPlayer.TaskRegister(1, Task_UpdateBvState, PlayIntervalTime);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    mtm_BvPlayer.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    BvPlayer.CloseVideo();
    ClearPage();
}

/************ 事件处理 **********/
/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void ButtonPressEvent()
{
    if(btOK)
    {
        if(BvPlayer == BvPlayer.Run)
        {
            BvPlayer.SetPlayerState(BvPlayer.Pause);
        }
        else
        {
            BvPlayer.SetPlayerState(BvPlayer.Run);
        }
    }
    if(btUP)
    {
        if(PlayIntervalTime > 0)
            PlayIntervalTime -= 2;
        
        mtm_BvPlayer.TaskSetIntervalTime(1, PlayIntervalTime);
    }
    if(btDOWN)
    {
        if(PlayIntervalTime < 50)
            PlayIntervalTime += 2;
        
        mtm_BvPlayer.TaskSetIntervalTime(1, PlayIntervalTime);
    }

    if(btBACK)
    {
        page.PageChangeTo(PAGE_FileExplorer);
    }
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
    if(btUP)
    {
        if(PlayIntervalTime > 0)
            PlayIntervalTime -= 2;
        
        mtm_BvPlayer.TaskSetIntervalTime(1, PlayIntervalTime);
    }
    if(btDOWN)
    {
        if(PlayIntervalTime < 50)
            PlayIntervalTime += 2;
        
        mtm_BvPlayer.TaskSetIntervalTime(1, PlayIntervalTime);
    }
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
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_BvPlayer(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);

    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
