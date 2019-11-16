#include "FileGroup.h"
#include "GUI_Private.h"

#include "common.h"

/*用于检测CPU占用率*/
static MillisTaskManager mtm_Game(1);

#if ARDUBOY_LIB_VER != 10101
#error
#endif // It may work even if you use other version.

/*  Defines  */

enum MODE
{
    LOGO_MODE = 0,
    TITLE_MODE,
    GAME_MODE
};

/*  Typedefs  */

typedef struct
{
    void (*initFunc)(void);
    bool (*updateFunc)(void);
    void (*drawFunc)(void);
} MODULE_FUNCS;

/*  Global Variables  */

MyArduboy arduboy;

/*  Local Variables  */

static const MODULE_FUNCS moduleTable[] =
{
    { initLogo,  updateLogo,  drawLogo },
    { initTitle, updateTitle, drawTitle },
    { initGame,  updateGame,  drawGame },
};

static MODE mode = LOGO_MODE;

/*  For Debugging  */

#ifdef DEBUG
bool    dbgPrintEnabled = true;
char    dbgRecvChar = '\0';
uint8_t dbgCaptureMode = 0;

static void dbgCheckSerialRecv(void)
{
    int recv;
    while ((recv = Serial.read()) != -1)
    {
        switch (recv)
        {
        case 'd':
            dbgPrintEnabled = !dbgPrintEnabled;
            Serial.print("Debug output ");
            Serial.println(dbgPrintEnabled ? "ON" : "OFF");
            break;
        case 'c':
            dbgCaptureMode = 1;
            break;
        case 'C':
            dbgCaptureMode = 2;
            break;
        }
        if (recv >= ' ' && recv <= '~') dbgRecvChar = recv;
    }
}

static void dbgScreenCapture()
{
    if (dbgCaptureMode)
    {
        Serial.write((const uint8_t *)arduboy.getBuffer(), WIDTH * HEIGHT / 8);
        if (dbgCaptureMode == 1) dbgCaptureMode = 0;
    }
}
#endif

static void Task_GameRunning()
{
#ifdef DEBUG
    dbgCheckSerialRecv();
#endif
    if (!(arduboy.nextFrame())) return;
    bool isDone = moduleTable[mode].updateFunc();
    moduleTable[mode].drawFunc();
#ifdef DEBUG
    dbgScreenCapture();
    dbgRecvChar = '\0';
#endif
    arduboy.display();
    if (isDone)
    {
        mode = (mode == TITLE_MODE) ? GAME_MODE : TITLE_MODE;
        moduleTable[mode].initFunc();
        dprint("mode=");
        dprintln(mode);
    }
}

/*---------------------------------------------------------------------------*/

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    mode = LOGO_MODE;
#ifdef DEBUG
    Serial.begin(115200);
#endif
    arduboy.beginNoLogo();
    arduboy.setFrameRate(60);
    moduleTable[LOGO_MODE].initFunc();

    mtm_Game.TaskRegister(0, Task_GameRunning, 5);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    mtm_Game.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
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

}

/**
  * @brief  按键长按事件
  * @param  无
  * @retval 无
  */
static void ButtonLongPressEvent()
{
    if(btBACK && mode == TITLE_MODE)
    {
        page.PageChangeTo(PAGE_About);
    }
}

/**
  * @brief  页面注册
  * @param  ThisPage:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_GameHopper(uint8_t ThisPage)
{
    /*基本*/
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);

    /*事件*/
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
}
