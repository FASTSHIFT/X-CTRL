#include "FileGroup.h"

extEEPROM eep(kbits_8, 1, 64, 0x50);//device size, number of devices, page size,I2C Address

#define StartAddr 0x00

uint32_t GetCheckSum(uint8_t* Array, uint32_t size)
{
    uint32_t sum = 0;
    for(uint32_t i = 0; i < size; i++)
    {
        sum += Array[i];
    }
    return sum;
}

typedef struct {
    Joystick_t JSL;
    Joystick_t JSR;
    RC_Info_t RC_Info;
	NRF_Config_TypeDef NRF_Cfg;
    uint8_t Pattern;
    bool SoundState;
    bool PassBackState;
    bool HMIState;
} SystemConfig_t;

typedef struct {
    SystemConfig_t EEPROM_Info;
    uint32_t CheckSum;
} EEPROM_DataPack_t;

EEPROM_DataPack_t EEDP;

bool EEPROM_Handle(EEPROM_Chs_t chs)
{
    mtm_Main.TaskStateCtrl(Task_MPU6050Read, OFF);
    bool retval = false;
    if(chs == ReadData)
    {
        for(uint16_t i = 0; i < sizeof(EEDP); i++)
        {
            ((uint8_t*)&EEDP)[i] = eep.read(StartAddr + i);
        }
        if(EEDP.CheckSum && GetCheckSum(((uint8_t*)(&(EEDP.EEPROM_Info))), sizeof(EEDP.EEPROM_Info)) == EEDP.CheckSum)
        {
            JS_L = EEDP.EEPROM_Info.JSL;
            JS_R = EEDP.EEPROM_Info.JSR;
            CTRL.Info = EEDP.EEPROM_Info.RC_Info;
            NRF_Cfg = EEDP.EEPROM_Info.NRF_Cfg;
            ConnectState.Pattern = EEDP.EEPROM_Info.Pattern;
            BuzzSound_State = EEDP.EEPROM_Info.SoundState;
            PassBack_State = EEDP.EEPROM_Info.PassBackState;
            HMI_State = EEDP.EEPROM_Info.HMIState;
            retval = true;
        }
    }
    else if(chs == SaveData)
    {
        EEDP.EEPROM_Info.JSL = JS_L;
        EEDP.EEPROM_Info.JSR = JS_R;
        EEDP.EEPROM_Info.RC_Info = CTRL.Info;  
		EEDP.EEPROM_Info.NRF_Cfg = NRF_Cfg;
        EEDP.EEPROM_Info.Pattern = ConnectState.Pattern;
        EEDP.EEPROM_Info.SoundState = BuzzSound_State;
        EEDP.EEPROM_Info.PassBackState = PassBack_State;
        EEDP.EEPROM_Info.HMIState = HMI_State;
		
        EEDP.CheckSum = GetCheckSum(((uint8_t*)(&(EEDP.EEPROM_Info))), sizeof(EEDP.EEPROM_Info));
        for(uint16_t i = 0; i < sizeof(EEDP); i++)
        {
            eep.update(StartAddr + i, ((uint8_t*)&EEDP)[i]);
        }
        retval = EEPROM_Handle(ReadData);
    }
    else if(chs == ClearData)
    {
		for(uint16_t i = 0; i < sizeof(EEDP); i++)
        {
            eep.write(StartAddr + i, 0);
        }
    }
    mtm_Main.TaskStateCtrl(Task_MPU6050Read, ON);
    return retval;
}
