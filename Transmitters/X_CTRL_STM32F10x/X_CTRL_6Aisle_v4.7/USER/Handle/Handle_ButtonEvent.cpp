#include "FileGroup.h"
#include "ComPrivate.h"
#include "GUI_Private.h"

/*实例化按键对象*/
ButtonEvent btUP(1, 500);
ButtonEvent btDOWN(1, 500);
ButtonEvent btOK(1, 500);
ButtonEvent btBACK(1, 500);
ButtonEvent btL1(1, 500);
ButtonEvent btR1(1, 500);
ButtonEvent btL2(1, 500);
ButtonEvent btR2(1, 500);
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
    if (btL1 == btL1.Press)
        CTRL.Key |= BT_L1;
    if (btR1 == btR1.Press)
        CTRL.Key |= BT_R1;
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
    page.PageEventTransmit(EVENT_ButtonLongPress);
}

/**
  * @brief  按键释放事件
  * @param  无
  * @retval 无
  */
void When_ButtonRelease()
{
    BuzzTone(700, 20);

    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == btBACK.NoPress)
        CTRL.Key &= ~BT_BACK;
    if (btL1 == btL1.NoPress)
        CTRL.Key &= ~BT_L1;
    if (btR1 == btR1.NoPress)
        CTRL.Key &= ~BT_R1;
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
    if(digitalRead(SPDT_Switch_Pin))
    {
        BuzzTone(500, 300);
    }
    else
    {
        BuzzTone(200, 100);
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
    
    /*按键引脚初始化*/
    pinMode(Button_OK_Pin, INPUT_PULLUP);
    pinMode(Button_BACK_Pin, INPUT_PULLUP);
    pinMode(Button_UP_Pin, INPUT_PULLUP);
    pinMode(Button_DOWN_Pin, INPUT_PULLUP);
    pinMode(Button_L_Pin, INPUT_PULLUP);
    pinMode(Button_R_Pin, INPUT_PULLUP);
    
    /*按键按下事件关联*/
    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);
    btBACK.	EventAttach_Press(When_ButtonPress);
    btL1.	EventAttach_Press(When_ButtonPress);
    btL2.	EventAttach_Press(When_ButtonPress);
    btR1.	EventAttach_Press(When_ButtonPress);
    btR2.	EventAttach_Press(When_ButtonPress);
    btEcd.	EventAttach_Press(When_ButtonPress);

    /*按键长摁事件关联*/
    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress);
    btL1.	EventAttach_LongPress(When_ButtonLongPress);
    btL2.	EventAttach_LongPress(When_ButtonLongPress);
    btR1.	EventAttach_LongPress(When_ButtonLongPress);
    btR2.	EventAttach_LongPress(When_ButtonLongPress);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress);

    /*按键释放事件关联*/
    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
    btBACK.	EventAttach_Release(When_ButtonRelease);
    btL1.	EventAttach_Release(When_ButtonRelease);
    btL2.	EventAttach_Release(When_ButtonRelease);
    btR1.	EventAttach_Release(When_ButtonRelease);
    btR2.	EventAttach_Release(When_ButtonRelease);
    btEcd.	EventAttach_Release(When_ButtonRelease);

    /*开关状态改变事件关联*/
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

void ButtonEventMonitor()
{
    /*开关事件监视*/
    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
    
    /*按键事件监视*/
    btUP.   EventMonitor(digitalRead(Button_UP_Pin));
    btDOWN. EventMonitor(digitalRead(Button_DOWN_Pin));
    btOK.   EventMonitor(digitalRead(Button_OK_Pin));
    btBACK. EventMonitor(digitalRead(Button_BACK_Pin));
    btL2.   EventMonitor(digitalRead(Button_L_Pin));
    btR2.   EventMonitor(digitalRead(Button_R_Pin));
}
