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
    StorageDataReg(CTRL.Info);              //控制器基本信息
    StorageDataReg(NRF_Cfg);                //NRF配置信息
    StorageDataReg(State_BuzzSound);        //蜂鸣器使能控制
    StorageDataReg(State_PassBack);         //回传使能控制
    StorageDataReg(State_Handshake);        //握手使能控制
    StorageDataReg(State_Bluetooth);        //蓝牙使能控制
    StorageDataReg(Bluetooth_ConnectObject);//控制对象
	StorageDataReg(NRF_AddressConfig);		//NRF地址
	StorageDataReg(JS_L_SlopeStart);
	StorageDataReg(JS_L_SlopeEnd);
	StorageDataReg(JS_R_SlopeStart);
	StorageDataReg(JS_R_SlopeEnd);

    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)//读取信息，如果失败，使用默认参数初始化
    {
        CTRL.Info.CtrlObject = X_COMMON;    //通用对象
        ConnectState.Pattern = Pattern_NRF; //使用NRF遥控

        JS_L.Xmin = 0;
        JS_L.Xmid = ADC_MaxValue / 2;
        JS_L.Xmax = ADC_MaxValue;
        
        JS_L.Ymin = 1703;
        JS_L.Ymid = 2020;
        JS_L.Ymax = 2929;

        JS_R.Xmin = 324;
        JS_R.Xmid = 2017;
        JS_R.Xmax = 3917;
        
        JS_R.Ymin = 0;
        JS_R.Ymid = ADC_MaxValue / 2;
        JS_R.Ymax = ADC_MaxValue;
        return false;
    }
    return true;
}

/**
  * @brief  HC05(蓝牙)初始化
  * @param  无
  * @retval 无
  */
void Init_HC05()
{
    hc05.Init(115200);          //波特率115200
    hc05.Power(State_Bluetooth);//蓝牙使能
}

/**
  * @brief  遥控器初始化
  * @param  无
  * @retval 无
  */
void Init_X_CTRL()
{
    Serial.begin(115200);
    Serial3.begin(115200);
    Init_Display();         //初始化显示器
    Init_Buzz();            //初始化蜂鸣器
//    Init_Motor();           //初始化振动电机
//    Init_MPU6050();         //初始化MPU6050
    Init_Value();           //初始化变量
    Init_BottonEvent();     //初始化按键事件
//    Init_EncoderEvent();    //初始化旋转编码器事件
//    Init_HC05();            //初始化蓝牙

//    IS_KEY_PRESSED(KEY_DOWN_Pin, State_BuzzSound = OFF);//按下旋转编码器按钮静音启动

    if(ConnectState.Pattern == Pattern_NRF)//是否选择使用NRF遥控
    {
        bool NRF_IsDetect = false;
        __LoopExecute(NRF_IsDetect = Init_NRF(), 100);//NRF初始化100次
        NRF_IsDetect ? BuzzMusic(MC_Type::MC_StartUp) : BuzzMusic(MC_Type::MC_Error);//判断是否成功初始化NRF，并播放对应的提示音
    }
}
