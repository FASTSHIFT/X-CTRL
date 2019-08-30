#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化按键对象*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;

static ButtonEvent btDR_ST_UP;
static ButtonEvent btDR_ST_DOWN;
static ButtonEvent btDR_TH_UP;
static ButtonEvent btDR_TH_DOWN;

static ButtonEvent btTRIM_ST_UP;
static ButtonEvent btTRIM_ST_DOWN;
static ButtonEvent btTRIM_TH_UP;
static ButtonEvent btTRIM_TH_DOWN;

static ButtonEvent swSPDT;

int16_t DR_ST_Value = CtrlOutput_MaxValue;
int16_t DR_TH_Value = CtrlOutput_MaxValue;

static void BuzzRing_DR(bool isST)
{
    if(isST)
        BuzzTone(1000 + DR_ST_Value * 5, 20);
    else
        BuzzTone(1000 + DR_TH_Value * 5, 20);
}

#define ChangeStep 20

static void When_ButtonPress_DR()
{
    if (btDR_ST_UP >= ButtonEvent_Type::Press)
    {
        if(DR_ST_Value < CtrlOutput_MaxValue)
        {
            DR_ST_Value += ChangeStep;
            BuzzRing_DR(true);
        }
        else
            MotorVibrate(1.0f, 50);
    }
    if (btDR_ST_DOWN >= ButtonEvent_Type::Press)
    {
        if(DR_ST_Value > 0)
        {
            DR_ST_Value -= ChangeStep;
            BuzzRing_DR(true);
        }
        else
            MotorVibrate(1.0f, 50);
    }

    if (btDR_TH_UP >= ButtonEvent_Type::Press)
    {
        if(DR_TH_Value < CtrlOutput_MaxValue)
        {
            DR_TH_Value += ChangeStep;
            BuzzRing_DR(false);
        }
        else
            MotorVibrate(1.0f, 50);
    }
    if (btDR_TH_DOWN >= ButtonEvent_Type::Press)
    {
        if(DR_TH_Value > 0)
        {
            DR_TH_Value -= ChangeStep;
            BuzzRing_DR(false);
        }
        else
            MotorVibrate(1.0f, 50);
    }
}


static void When_ButtonLongPress_DR()
{
    __IntervalExecute(When_ButtonPress_DR(), 100);
}

static void When_ButtonPress_TRIM()
{
    BuzzTone(600, 20);
    if (btTRIM_ST_UP == ButtonEvent_Type::Press)
    {
        CTRL.Key |= BT_L2;
    }
    if (btTRIM_ST_DOWN == ButtonEvent_Type::Press)
    {
        CTRL.Key |= BT_R2;
    }
    if (btTRIM_TH_UP == ButtonEvent_Type::Press)
    {
        CTRL.Key |= BT_L1;
    }
    if (btTRIM_TH_DOWN == ButtonEvent_Type::Press)
    {
        CTRL.Key |= BT_R1;
    }
}

static void When_ButtonRelease_TRIM()
{
    BuzzTone(900, 20);
    if (btTRIM_ST_UP == ButtonEvent_Type::NoPress)
    {
        CTRL.Key &= ~BT_L2;
    }
    if (btTRIM_ST_DOWN == ButtonEvent_Type::NoPress)
    {
        CTRL.Key &= ~BT_R2;
    }
    if (btTRIM_TH_UP == ButtonEvent_Type::NoPress)
    {
        CTRL.Key &= ~BT_L1;
    }
    if (btTRIM_TH_DOWN == ButtonEvent_Type::NoPress)
    {
        CTRL.Key &= ~BT_R1;
    }
}

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
static void When_ButtonPress_Page()
{
    MotorVibrate(1.0f, 50);
    BuzzTone(500, 20);//播放操作音(500Hz, 持续20ms)

    /*置位对应的CTRL按键标志位*/
    if (btUP == ButtonEvent_Type::Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == ButtonEvent_Type::Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_OK;
    if (btBACK == ButtonEvent_Type::Press)
        CTRL.Key |= BT_BACK;

    page.PageEventTransmit(EVENT_ButtonPress);//向页面调度器发送一个按键按下事件
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
void When_ButtonLongPress_Page()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);//向页面调度器发送一个按键长摁事件
}

/**
  * @brief  按键释放事件
  * @param  无
  * @retval 无
  */
void When_ButtonRelease_Page()
{
    BuzzTone(700, 20);//播放操作音(700Hz, 持续20ms)

    /*清除对应的CTRL按键标志位*/
    if (btUP == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_BACK;

    page.PageEventTransmit(EVENT_ButtonRelease);
}

/**
  * @brief  开关状态改变事件
  * @param  无
  * @retval 无
  */
void When_ChangeSPDT()
{
    if(IS_EnableCtrl())//判断开关状态
    {
        BuzzTone(500, 300); //播放操作音(500Hz, 持续300ms)
        XFS_Clear();        //清除语音缓存队列
        XFS_Speak("遥控器解锁");//加入语音缓存队列
    }
    else
    {
        BuzzTone(200, 100); //播放操作音(200Hz, 持续100ms)
        XFS_Clear();        //清除语音缓存队列
        XFS_Speak("遥控器锁定");//加入语音缓存队列
    }
}

/**
  * @brief  初始化按键事件
  * @param  无
  * @retval 无
  */
void Init_BottonEvent()
{
    /*按键初始化*/
    pinMode(KEY_CH_Pin, INPUT_PULLUP);
    pinMode(KEY_SEL_Pin, INPUT_PULLUP);
    pinMode(KEY_UP_Pin, INPUT_PULLUP);
    pinMode(KEY_DOWN_Pin, INPUT_PULLUP);

    /*开关引脚初始化*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLUP);

#ifdef USE_Encoder
    /*旋转编码器按键引脚初始化*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);
#endif

    /*HC165引脚初始化*/
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);

    /*按键按下事件关联*/
    btUP.   EventAttach_Press(When_ButtonPress_Page);
    btDOWN. EventAttach_Press(When_ButtonPress_Page);
    btOK.   EventAttach_Press(When_ButtonPress_Page);
    btBACK. EventAttach_Press(When_ButtonPress_Page);
    btEcd.  EventAttach_Press(When_ButtonPress_Page);

    btDR_ST_UP.     EventAttach_Press(When_ButtonPress_DR);
    btDR_ST_DOWN.   EventAttach_Press(When_ButtonPress_DR);
    btDR_TH_UP.     EventAttach_Press(When_ButtonPress_DR);
    btDR_TH_DOWN.   EventAttach_Press(When_ButtonPress_DR);

    btTRIM_ST_UP.   EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_ST_DOWN. EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_TH_UP.   EventAttach_Press(When_ButtonPress_TRIM);
    btTRIM_TH_DOWN. EventAttach_Press(When_ButtonPress_TRIM);

    /*按键长摁事件关联*/
    btUP.   EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN. EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.   EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK. EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.  EventAttach_LongPress(When_ButtonLongPress_Page);

    btDR_ST_UP.     EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_ST_DOWN.   EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_TH_UP.     EventAttach_LongPress(When_ButtonLongPress_DR);
    btDR_TH_DOWN.   EventAttach_LongPress(When_ButtonLongPress_DR);


    /*按键释放事件关联*/
    btUP.   EventAttach_Release(When_ButtonRelease_Page);
    btDOWN. EventAttach_Release(When_ButtonRelease_Page);
    btOK.   EventAttach_Release(When_ButtonRelease_Page);
    btBACK. EventAttach_Release(When_ButtonRelease_Page);
    btEcd.  EventAttach_Release(When_ButtonRelease_Page);

    btTRIM_ST_UP.   EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_ST_DOWN. EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_TH_UP.   EventAttach_Release(When_ButtonRelease_TRIM);
    btTRIM_TH_DOWN. EventAttach_Release(When_ButtonRelease_TRIM);

    /*开关状态改变事件关联*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

/**
  * @brief  获取HC165的值
  * @param  无
  * @retval 无
  */
static uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

uint8_t ButtonValue;
/**
  * @brief  按键事件监视
  * @param  无
  * @retval 无
  */
void ButtonEventMonitor()
{
    ButtonValue = Get_HC165_Value();//获取HC165八个引脚的电平值
    /*enum KeyBit {
        BIT_ST_DRP = 0x80,
        BIT_ST_DRM = 0x10,
        BIT_TH_DRP = 0x40,
        BIT_TH_DRM = 0x20,
        BIT_TH_TRIMP = 0x04,
        BIT_TH_TRIMM = 0x08,
        BIT_ST_TRIMP = 0x01,
        BIT_ST_TRIMM = 0x02
    };*/
    enum KeyBit
    {
        BIT_ST_DRP = 0x01,
        BIT_ST_DRM = 0x02,
        BIT_TH_DRP = 0x04,
        BIT_TH_DRM = 0x08,
        BIT_TH_TRIMP = 0x80,
        BIT_TH_TRIMM = 0x10,
        BIT_ST_TRIMP = 0x40,
        BIT_ST_TRIMM = 0x20
    };

    /*按键事件监视*/
    btUP.   EventMonitor(!digitalRead(KEY_UP_Pin));
    btDOWN. EventMonitor(!digitalRead(KEY_DOWN_Pin));
    btOK.   EventMonitor(!digitalRead(KEY_CH_Pin));
    btBACK. EventMonitor(!digitalRead(KEY_SEL_Pin));

    btDR_ST_UP.     EventMonitor(ButtonValue & BIT_ST_DRP);
    btDR_ST_DOWN.   EventMonitor(ButtonValue & BIT_ST_DRM);
    btDR_TH_UP.     EventMonitor(ButtonValue & BIT_TH_DRP);
    btDR_TH_DOWN.   EventMonitor(ButtonValue & BIT_TH_DRM);

    btTRIM_ST_UP.     EventMonitor(ButtonValue & BIT_ST_TRIMP);
    btTRIM_ST_DOWN.   EventMonitor(ButtonValue & BIT_ST_TRIMM);
    btTRIM_TH_UP.     EventMonitor(ButtonValue & BIT_TH_TRIMP);
    btTRIM_TH_DOWN.   EventMonitor(ButtonValue & BIT_TH_TRIMM);

    swSPDT. EventMonitor(IS_EnableCtrl());
}
