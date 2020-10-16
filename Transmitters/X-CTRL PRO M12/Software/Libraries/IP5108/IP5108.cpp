#include "IP5108.h"

IP5108::IP5108(uint8_t addr)
{
    Address = addr;
}

void IP5108::begin()
{
    writeRegBit(SYS_CTL0, SYS_CTL0_BIT_FlashLight, false);
    writeRegBit(SYS_CTL0, SYS_CTL0_BIT_Light, false);
    writeRegBit(SYS_CTL1, SYS_CTL1_BIT_LowLoadOff, false);
    writeRegBit(SYS_CTL1, SYS_CTL1_BIT_LoadInsertAutoStartUp, false);
    writeRegBit(SYS_CTL4, SYS_CTL4_BIT_VIN_PullOutBoost, true);     //  VIN 拔出开启 BOOST
    writeRegBit(SYS_CTL5, SYS_CTL5_BIT_KeyShutdownSet, true);      //  长按关机
    writeRegBits(MFP_CTL0, MFP_CTL0_BIT_LIGHT_Sel, MFP_CTL0_BIT_LIGHT_Sel_WLED);
    writeRegBits(MFP_CTL0, MFP_CTL0_BIT_L4_Sel, MFP_CTL0_BIT_L4_Sel_L4);
    writeRegBits(MFP_CTL0, MFP_CTL0_BIT_L3_Sel, MFP_CTL0_BIT_L3_Sel_L3);
    writeRegBit(MFP_CTL1, MFP_CTL1_BIT_VSET_Sel, false);
    writeRegBit(MFP_CTL1, MFP_CTL1_BIT_RSET_Sel, false);
    //writeRegBit(CHG_DIG_CTL4, CHG_DIG_CTL4_BIT_BatteryTypeInternalSet, true); //  外部 VSET PIN 设置电池类型
    writeRegBit(CHG_DIG_CTL4, CHG_DIG_CTL4_BIT_BatteryTypeInternalSet, false);  //  内部寄存器设置 设置电池类型
    writeRegBits(Charger_CTL2, Charger_CTL2_BIT_BatteryTypeSet, Charger_CTL2_BIT_BatteryTypeSet_4V2);   //  设置电池类型为4.2V
}

uint8_t IP5108::readReg(REG_t reg)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(Address, 1);
    uint8_t value = Wire.read();
    Wire.endTransmission();

    return value;
}

bool IP5108::readRegBit(REG_t reg, REG_BIT_t bit)
{
    return (readReg(reg) & bit);
}


void IP5108::writeReg(REG_t reg, uint8_t val)
{
    Wire.beginTransmission(Address);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void IP5108::writeRegBit(REG_t reg, REG_BIT_t bit, bool val)
{
    uint8_t nval = readReg(reg);
    val ? nval |= bit : nval &= ~bit;
    writeReg(reg, nval);
}

void IP5108::writeRegBits(REG_t reg, REG_BIT_t bit, REG_BIT_t val)
{
    uint8_t nval = readReg(reg);

    for(int i = 0; i < 8; i++)
    {
        if( bit & (1 << i) )  (val & (1 << i)) ? nval |= (1 << i) : nval &= ~(1 << i);
    }

    writeReg(reg, nval);
}

float IP5108::getBattVoltage()
{
    float batVol;
    uint8_t BATVADC_VALUE_low = readReg(BATVADC_DAT0); //low 8bit
    uint8_t BATVADC_VALUE_high = readReg(BATVADC_DAT1); //high 6bit
    if((BATVADC_VALUE_high & 0x20) == 0x20) //补码
    {
        batVol = 2600 - ((~BATVADC_VALUE_low) + (~(BATVADC_VALUE_high & 0x1F)) * 256 + 1) * 0.26855;
    }
    else //原码
    {
        batVol = 2600 + (BATVADC_VALUE_low + BATVADC_VALUE_high * 256) * 0.26855; //mv 为单位
    }
    return batVol;
}

float IP5108::getBattCurrent()
{
    float batCur;
    uint8_t BATIADC_VALUE_low = readReg(BATIADC_DAT0);
    uint8_t BATIADC_VALUE_high = readReg(BATIADC_DAT1);
    if((BATIADC_VALUE_high & 0x20) == 0x20) //负值
    {
        char a = ~BATIADC_VALUE_low;
        char b = (~(BATIADC_VALUE_high & 0x1F) & 0x1f);
        int c = b * 256 + a + 1;
        batCur = -c * 0.745985;
//BATCUR=-(int)(((~BATIADC_VALUE_low)+(~(BATIADC_VALUE_high & 0x1F))*256+1)*0.745985);
    }
    else //正值
    {
        batCur = (BATIADC_VALUE_high * 256 + BATIADC_VALUE_low) * 0.745985; //mA 为单位
    }
    return batCur;
}

float IP5108::getBattOcVoltage()
{
    float outVol;
    uint8_t BATOCVADC_VALUE_low = readReg(BATOCV_DAT0); //low 8bit
    uint8_t BATOCVADC_VALUE_high = readReg(BATOCV_DAT1); //high 6bit
    if((BATOCVADC_VALUE_high & 0x20) == 0x20) //补码
    {
        outVol = 2600 - ((~ BATOCVADC_VALUE_low) + (~( BATOCVADC_VALUE_high & 0x1F)) * 256 + 1) * 0.26855;
    }
    else //原码
    {
        outVol = 2600 + (BATOCVADC_VALUE_low + BATOCVADC_VALUE_high * 256) * 0.26855; //mv 为单位
    }
    return outVol;
}

void IP5108::update()
{
    current = getBattCurrent();
    voltage = getBattVoltage();
    voltageOc = getBattOcVoltage();
}
