#include "FileGroup.h"
#include "ComPrivate.h"

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))//注册储存变量

/**
  * @brief  变量初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_Value()
{               
    /*注册需要掉电储保存的变量*/
    StorageDataReg(JS_L);                   //左摇杆信息
    StorageDataReg(JS_R);                   //右摇杆信息
    StorageDataReg(State_BuzzSound);        //蜂鸣器使能控制
	
    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//读取信息，如果失败，使用默认参数初始化
    {
        JS_L.Xmin = 900;
        JS_L.Xmid = 2100;
        JS_L.Xmax = 3235;
        
        JS_L.Ymin = 930;
        JS_L.Ymid = 2084;
        JS_L.Ymax = 3393;

        JS_R.Xmin = 804;
        JS_R.Xmid = 2044;
        JS_R.Xmax = 3140;
        
        JS_R.Ymin = 862;
        JS_R.Ymid = 2072;
        JS_R.Ymax = 3354;
        return false;
    }
    return true;
}

/**
  * @brief  遥控器初始化
  * @param  无
  * @retval 无
  */
void Init_X_CTRL()
{
	Init_Display();         //初始化显示器
	Init_Buzz();            //初始化蜂鸣器
	Init_Value();           //初始化变量
	Init_BottonEvent();     //初始化按键事件
	Init_EncoderEvent();    //初始化旋转编码器事件
	Init_Client();	
	
	Serial.begin(115200);

}
