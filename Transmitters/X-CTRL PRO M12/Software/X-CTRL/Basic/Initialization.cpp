#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"
#include "BSP/IMU_Private.h"
#include "Model/XC_Model.h"

/*遥控器结构体*/
X_CTRL_TypeDef CTRL;

/**
  * @brief  变量初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Value_Init()
{
    DEBUG_FUNC_LOG();
    uint8_t eep_ret = EEPROM_Init();
    
    EEPROM_REG_VALUE(CTRL.JS_L);
    EEPROM_REG_VALUE(CTRL.JS_R);
    EEPROM_REG_VALUE(CTRL.ModelIndex);
    EEPROM_REG_VALUE(XC_Model);
    
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
        X_CTRL_SetDefault();
        Serial.println("EEPROM data error! set data to default value");
        return false;
    }
    
    Serial.println("EEPROM init success");
    
    return true;
}

void X_CTRL_SetDefault()
{
    Model_SetDefault();
    CTRL.State->IMU = false;
    CTRL.State->Sound = true;
    CTRL.State->LRA_Vibrate = true;
    CTRL.State->ERM_Vibrate = true;
    CTRL.State->Handshake = true;
    CTRL.State->Passback = true;
    CTRL.State->FHSS = false;
    Joystick_SetDefault();
    Com_ChannelSetDefault();
    IMU_LimitSetDefault();
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
    Model_Init();
    Display_Init();
    Button_Init();
    Audio_Init();
    IMU_Init();
    Joystick_Init();
    Bluetooth_Init();
    MotorLRA_Init();
    MotorERM_Init();

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
