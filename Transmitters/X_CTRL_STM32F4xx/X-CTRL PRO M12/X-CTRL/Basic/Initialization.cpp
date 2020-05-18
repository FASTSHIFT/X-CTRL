#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "BSP/IMU_Private.h"

/*遥控器结构体*/
CTRL_TypeDef CTRL;

/**
  * @brief  变量初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Value_Init()
{
    DEBUG_FUNC_LOG();
    uint8_t eep_ret = EEPROM_Init();
    
    EEPROM_REG_VALUE(CTRL);
    
    EEPROM_REG_VALUE(IMU_Axis.Pitch.Limit);
    EEPROM_REG_VALUE(IMU_Axis.Roll.Limit);
    EEPROM_REG_VALUE(IMU_Axis.Yaw.Limit);
    
    if(eep_ret != 0)
    {
        Serial.printf("EEPROM error! (0x%x)\r\n", eep_ret);
        return false;
    }

    if(!EEPROM_ReadAll())
    {
        CTRL.State.IMU = false;
        CTRL.State.Sound = true;
        CTRL.State.Vibrate = true;
        CTRL.State.BigVibrate = true;
        CTRL.State.Handshake = true;
        CTRL.State.Passback = true;
        CTRL.State.FHSS = false;
        Joystick_SetDefault();
        Com_ChannelSetDefault();
        IMU_LimitSetDefault();
        
        Serial.println("EEPROM data error! set data to default value");
        EEPROM_SaveAll();
        return false;
    }
    
    Serial.println("EEPROM init success");
    
    return true;
}

/**
  * @brief  遥控器初始化
  * @param  无
  * @retval 无
  */
void X_CTRL_Init()
{
    Serial.begin(115200);
    DEBUG_FUNC_LOG();
    
    I2C_Scan(false);
    Power_Init();
    Value_Init();
    Display_Init();
    Button_Init();
    Audio_Init();
    IMU_Init();
    Joystick_Init();
    Bluetooth_Init();
    Motor_Init();
    BigMotor_Init();

    Com_ChannelInit();
    
    if(Com_Init())
    {
        Serial.println("Communication init success");
        Audio_PlayMusic(MC_Type::MC_StartUp);
    }
    else
    {
        Serial.println("Communication error!");
        Audio_PlayMusic(MC_Type::MC_Error);
    }
}
