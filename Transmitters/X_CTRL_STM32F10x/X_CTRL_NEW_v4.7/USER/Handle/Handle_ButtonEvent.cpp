#include "FileGroup.h"
#include "GUI_Private.h"
#include "ComPrivate.h"

/*实例化按键对象*/
ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btL1;
ButtonEvent btL2;
ButtonEvent btR1;
ButtonEvent btR2;
ButtonEvent btEcd(1, 500);
ButtonEvent swSPDT;

/**
  * @brief  按键按下事件
  * @param  无
  * @retval 无
  */
void When_ButtonPress()
{
    BuzzTone(500, 20);//播放操作音(500Hz, 持续20ms)

    /*置位对应的CTRL按键标志位*/
    if (btUP == btUP.Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == btDOWN.Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == btOK.Press)
        CTRL.Key |= BT_OK;
    if (btBACK == btBACK.Press)
        CTRL.Key |= BT_BACK;
//    if (btL1 == btL1.Press)
//        CTRL.Key |= BT_L1;
//    if (btR1 == btR1.Press)
//        CTRL.Key |= BT_R1;
    if (btL2 == btL2.Press)
        CTRL.Key |= BT_L2;
    if (btR2 == btR2.Press)
        CTRL.Key |= BT_R2;

    page.PageEventTransmit(EVENT_ButtonPress);//向页面调度器发送一个按键按下事件
}

/**
  * @brief  按键长摁事件
  * @param  无
  * @retval 无
  */
void When_ButtonLongPress()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);//向页面调度器发送一个按键长摁事件
}

/**
  * @brief  按键释放事件
  * @param  无
  * @retval 无
  */
void When_ButtonRelease()
{
    BuzzTone(700, 20);//播放操作音(700Hz, 持续20ms)

    /*清除对应的CTRL按键标志位*/
    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == btBACK.NoPress)
        CTRL.Key &= ~BT_BACK;
//    if (btL1 == btL1.NoPress)
//        CTRL.Key &= ~BT_L1;
//    if (btR1 == btR1.NoPress)
//        CTRL.Key &= ~BT_R1;
    if (btL2 == btL2.NoPress)
        CTRL.Key &= ~BT_L2;
    if (btR2 == btR2.NoPress)
        CTRL.Key &= ~BT_R2;
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
    /*开关引脚初始化*/
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);
    
    /*旋转编码器按键引脚初始化*/
    pinMode(EncoderKey_Pin, INPUT_PULLUP);

    /*HC165引脚初始化*/
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
    
    /*按键按下事件关联*/
    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);
    btBACK.	EventAttach_Press(When_ButtonPress);
    //btL1.	EventAttach_Press(When_ButtonPress);
    btL2.	EventAttach_Press(When_ButtonPress);
    //btR1.	EventAttach_Press(When_ButtonPress);
    btR2.	EventAttach_Press(When_ButtonPress);
    btEcd.	EventAttach_Press(When_ButtonPress);

    /*按键长摁事件关联*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress);
    //btL1.	EventAttach_LongPress(When_ButtonLongPress);
    btL2.	EventAttach_LongPress(When_ButtonLongPress);
    //btR1.	EventAttach_LongPress(When_ButtonLongPress);
    btR2.	EventAttach_LongPress(When_ButtonLongPress);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress);

    /*按键释放事件关联*/
    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
    btBACK.	EventAttach_Release(When_ButtonRelease);
    //btL1.	EventAttach_Release(When_ButtonRelease);
    btL2.	EventAttach_Release(When_ButtonRelease);
    //btR1.	EventAttach_Release(When_ButtonRelease);
    btR2.	EventAttach_Release(When_ButtonRelease);
    btEcd.	EventAttach_Release(When_ButtonRelease);

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

/**
  * @brief  按键事件监视
  * @param  无
  * @retval 无
  */
void ButtonEventMonitor()
{
    uint8_t ButtonValue = Get_HC165_Value();//获取HC165八个引脚的电平值

    /*按键事件监视，读取字节对应位*/
    btUP.   EventMonitor(bitRead(ButtonValue, 4));
    btDOWN. EventMonitor(bitRead(ButtonValue, 7));
    btOK.   EventMonitor(bitRead(ButtonValue, 6));
    btBACK. EventMonitor(bitRead(ButtonValue, 5));
    btL1.   EventMonitor(bitRead(ButtonValue, 1));
    btL2.   EventMonitor(bitRead(ButtonValue, 0));
    btR1.   EventMonitor(bitRead(ButtonValue, 2));
    btR2.   EventMonitor(bitRead(ButtonValue, 3));
    btEcd.  EventMonitor(digitalRead(EncoderKey_Pin));
    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
}
