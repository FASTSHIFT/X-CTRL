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

static void DR_EventHandler(ButtonEvent* btn, int event)
{
    const uint8_t ChangeStep = 20;
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if (*btn == btDR_ST_UP)
        {
            if(DR_ST_Value < CtrlOutput_MaxValue)
            {
                DR_ST_Value += ChangeStep;
                BuzzRing_DR(true);
            }
            else
                MotorVibrate(1.0f, 50);
        }
        else if (*btn == btDR_ST_DOWN)
        {
            if(DR_ST_Value > 0)
            {
                DR_ST_Value -= ChangeStep;
                BuzzRing_DR(true);
            }
            else
                MotorVibrate(1.0f, 50);
        }

        else if (*btn == btDR_TH_UP)
        {
            if(DR_TH_Value < CtrlOutput_MaxValue)
            {
                DR_TH_Value += ChangeStep;
                BuzzRing_DR(false);
            }
            else
                MotorVibrate(1.0f, 50);
        }
        else if (*btn == btDR_TH_DOWN)
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

}

static void TRIM_EventHandler(ButtonEvent* btn, int event)
{
    if(event == EVENT_ButtonPress)
    {
        BuzzTone(600, 20);
        if (*btn == btTRIM_ST_UP)
        {
            CTRL.Key |= BT_L2;
        }
        if (*btn == btTRIM_ST_DOWN)
        {
            CTRL.Key |= BT_R2;
        }
        if (*btn == btTRIM_TH_UP)
        {
            CTRL.Key |= BT_L1;
        }
        if (*btn == btTRIM_TH_DOWN)
        {
            CTRL.Key |= BT_R1;
        }
    }
    else if(event == EVENT_ButtonRelease)
    {
        BuzzTone(900, 20);
        if (*btn == btTRIM_ST_UP)
        {
            CTRL.Key &= ~BT_L2;
        }
        if (*btn == btTRIM_ST_DOWN)
        {
            CTRL.Key &= ~BT_R2;
        }
        if (*btn == btTRIM_TH_UP)
        {
            CTRL.Key &= ~BT_L1;
        }
        if (*btn == btTRIM_TH_DOWN)
        {
            CTRL.Key &= ~BT_R1;
        }
    }
}

/**
  * @brief  页面按键事件
  * @param  无
  * @retval 无
  */
static void Page_EventHandler(ButtonEvent* btn, int event)
{
    if(event == EVENT_ButtonPress)
    {
        MotorVibrate(1.0f, 50);
        BuzzTone(500, 20);//播放操作音(500Hz, 持续20ms)

        /*置位对应的CTRL按键标志位*/
        if (*btn == btUP)
            CTRL.Key |= BT_UP;
        if (*btn == btDOWN)
            CTRL.Key |= BT_DOWN;
        if (*btn == btOK)
            CTRL.Key |= BT_OK;
        if (*btn == btBACK)
            CTRL.Key |= BT_BACK;
    }
    else if(event == EVENT_ButtonRelease)
    {
        BuzzTone(700, 20);//播放操作音(700Hz, 持续20ms)

        /*清除对应的CTRL按键标志位*/
        if (*btn == btUP)
            CTRL.Key &= ~BT_UP;
        if (*btn == btDOWN)
            CTRL.Key &= ~BT_DOWN;
        if (*btn == btOK)
            CTRL.Key &= ~BT_OK;
        if (*btn == btBACK)
            CTRL.Key &= ~BT_BACK;
    }
    /*传递到页面事件*/
    page.PageEventTransmit(event, btn);
}

/**
  * @brief  开关事件
  * @param  无
  * @retval 无
  */
void SPDT_EventHandler(ButtonEvent* btn, int event)
{
    if(*btn == swSPDT && event == EVENT_ButtonChange)
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

    /*按键事件关联*/
    btUP.   EventAttach(Page_EventHandler);
    btDOWN. EventAttach(Page_EventHandler);
    btOK.   EventAttach(Page_EventHandler);
    btBACK. EventAttach(Page_EventHandler);
    btEcd.  EventAttach(Page_EventHandler);

    btDR_ST_UP.     EventAttach(DR_EventHandler);
    btDR_ST_DOWN.   EventAttach(DR_EventHandler);
    btDR_TH_UP.     EventAttach(DR_EventHandler);
    btDR_TH_DOWN.   EventAttach(DR_EventHandler);

    btTRIM_ST_UP.   EventAttach(TRIM_EventHandler);
    btTRIM_ST_DOWN. EventAttach(TRIM_EventHandler);
    btTRIM_TH_UP.   EventAttach(TRIM_EventHandler);
    btTRIM_TH_DOWN. EventAttach(TRIM_EventHandler);

    /*开关状态改变事件关联*/
    swSPDT.EventAttach(SPDT_EventHandler);
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
