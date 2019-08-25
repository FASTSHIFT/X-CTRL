#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化按键对象*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btEcd;
ButtonEvent btA;
ButtonEvent btB;

static ButtonEvent swSPDT;

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
    if (btA == ButtonEvent_Type::Press)
        CTRL.Key |= BT_L2;
	if (btB == ButtonEvent_Type::Press)
        CTRL.Key |= BT_R2;

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
	if (btA == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_L2;
	if (btB == ButtonEvent_Type::NoPress)
        CTRL.Key &= ~BT_R2;
    
    page.PageEventTransmit(EVENT_ButtonRelease);
}

/**
  * @brief  开关状态改变事件
  * @param  无
  * @retval 无
  */
void When_ChangeSPDT()
{
    if(digitalRead(SPDT_Switch_Pin))//判断开关状态
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
    pinMode(KEY_OK_Pin, INPUT_PULLUP);
    pinMode(KEY_BACK_Pin, INPUT_PULLUP);
    pinMode(KEY_UP_Pin, INPUT_PULLUP);
    pinMode(KEY_DOWN_Pin, INPUT_PULLUP);
	pinMode(KEY_A_Pin, INPUT_PULLUP);
    pinMode(KEY_B_Pin, INPUT_PULLUP);

    /*开关引脚初始化*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLUP);

    /*旋转编码器按键引脚初始化*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);

    /*按键按下事件关联*/
    btUP.	EventAttach_Press(When_ButtonPress_Page);
    btDOWN.	EventAttach_Press(When_ButtonPress_Page);
    btOK.	EventAttach_Press(When_ButtonPress_Page);
    btBACK.	EventAttach_Press(When_ButtonPress_Page);
    btEcd.	EventAttach_Press(When_ButtonPress_Page);
	btA.	EventAttach_Press(When_ButtonPress_Page);
	btB.	EventAttach_Press(When_ButtonPress_Page);

    /*按键长按事件关联*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btOK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress_Page);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress_Page);
	btA.	EventAttach_LongPress(When_ButtonLongPress_Page);
	btB.	EventAttach_LongPress(When_ButtonLongPress_Page);

    
   /*按键释放事件关联*/
    btUP.	EventAttach_Release(When_ButtonRelease_Page);
    btDOWN.	EventAttach_Release(When_ButtonRelease_Page);
    btOK.	EventAttach_Release(When_ButtonRelease_Page);
    btBACK.	EventAttach_Release(When_ButtonRelease_Page);
    btEcd.	EventAttach_Release(When_ButtonRelease_Page);
	btA.	EventAttach_Release(When_ButtonRelease_Page);
	btB.	EventAttach_Release(When_ButtonRelease_Page);

    /*开关状态改变事件关联*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

/**
  * @brief  按键事件监视
  * @param  无
  * @retval 无
  */
void ButtonEventMonitor()
{ 
    /*按键事件监视*/
    btUP.   EventMonitor(!digitalRead(KEY_UP_Pin));
    btDOWN. EventMonitor(!digitalRead(KEY_DOWN_Pin));
    btOK.   EventMonitor(!digitalRead(KEY_OK_Pin));
    btBACK. EventMonitor(!digitalRead(KEY_BACK_Pin));
	btA.	EventMonitor(!digitalRead(KEY_A_Pin));
	btB.	EventMonitor(!digitalRead(KEY_B_Pin));

    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
}
