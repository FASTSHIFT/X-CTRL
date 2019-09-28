#ifndef __IP5018_H
#define __IP5018_H

#include "Wire.h"

#define IP5018_DEFATLT_ADDRESS 0x75

class IP5108{
public :
    IP5108();
    void begin(uint8_t addr = IP5018_DEFATLT_ADDRESS);

    typedef enum{
        SYS_CTL0 = 0x01,
        SYS_CTL1 = 0x02,
        SYS_CTL2 = 0x0C,
        SYS_CTL3 = 0x03,
        SYS_CTL4 = 0x04,
        SYS_CTL5 = 0x07,
        Charger_CTL1 = 0x22,
        Charger_CTL2 = 0x24,
        CHG_DIG_CTL4 = 0x26,
        CHG_DIG_CTL5 = 0x25,
        MFP_CTL0 = 0x51,
        MFP_CTL1 = 0x52,
        GPIO_CTL1 = 0x53,
        GPIO_CTL2 = 0x54,
        GPIO_CTL3 = 0x55,
        BATVADC_DAT0 = 0xA2,
        BATVADC_DAT1 = 0xA3,
        BATIADC_DAT0 = 0xA4,
        BATIADC_DAT1 = 0xA5,
        BATOCV_DAT0 = 0xA8,
        BATOCV_DAT1 = 0xA9,
        Reg_READ0 = 0x71,
        Reg_READ1 = 0x72,
        Reg_READ2 = 0x77,
    }REG_t;
    
    typedef enum{
        /*************************** SYS_CTL0 ***************************/
        SYS_CTL0_BIT_FlashLight = 1 << 4,/*手电筒检测是否使能  1：enable 0：disable*/
        SYS_CTL0_BIT_Light      = 1 << 3,/*Light enable        1：enable 0：disable*/
        SYS_CTL0_BIT_Boost      = 1 << 2,/*Boost enable        1：enable 0：disable*/
        SYS_CTL0_BIT_Charger    = 1 << 1,/*Charger enable      1：enable 0：disable*/
        
        /*************************** SYS_CTL1 ***************************/
        /*轻载关机使能（0x0c 可设定轻载关机阈值）
        1:使能 IBATLOW 轻载关机功能
        0:关闭 IBATLOW 轻载关机功能*/
        SYS_CTL1_BIT_LowLoadOff = 1 << 1,
        
        /*负载插入自动开机
        1:使能
        0:关闭*/
        SYS_CTL1_BIT_LoadInsertAutoStartUp = 1 << 0,
        
        /*************************** SYS_CTL2 ***************************/
        /*轻载关机电流阈值设定
        n * 12mA
        当 BAT 电流小于设定阈值时，持续 32s 关机
        注意：此电流设定阈值需要大于 100mA*/
        SYS_CTL2_BIT_CurrentThreshold = B00011111,
        
        /*************************** SYS_CTL3 ***************************/
        /*长按按键时间选择
        00：1S
        01：2S
        10：3S
        11：4S*/
        SYS_CTL3_BIT_LongPressTimeSet = B00000011,
        
        /*1：连续两次短按（两次短按在 1s 内）
        关机功能使能
        0：连续两次短按（两次短按在 1s 内）
        关机功能关闭*/
        SYS_CTL3_BIT_DoubleShortPress = 1 << 5,
        
        /*************************** SYS_CTL4 ***************************/
        /*关机时间设定
        11:64s
        10:32s
        01:16s
        00: 8s*/
        SYS_CTL4_BIT_ShutdownTimeSet = B00000011,
        
        /*VIN 拔出是否开启 BOOST
        1：开启
        0：不开启*/
        SYS_CTL4_BIT_VIN_PullOutBoost = 1 << 5,
        
        /*************************** SYS_CTL5 ***************************/
        /*NTC 功能使能
        0：使能
        1：关闭*/
        SYS_CTL5_BIT_NTC_Function = 1 << 6,
        
        /*按键开关 WLED 手电筒方式选择：
        0：长按 2S
        1：短按两次按键*/
        SYS_CTL5_BIT_WLED_Set = 1 << 1,
        
        /*按键关机方式选择：
        0：短按两次按键
        1：长按 2S*/
        SYS_CTL5_BIT_KeyShutdownSet = 1 << 0,

        /*************************** Charger_CTL1 ***************************/
        /*充电欠压环设定（充电时输出端 VOUT 的电压）
        11:4.83V
        10:4.73V
        01:4.63V
        00:4.53V
        注：在充电的时候 IC 会检测输出 VOUT
        的电压来自动调整充电电流，当 VOUT 的
        电压大于设置值时就以最大电流对充电
        充电，小于设定值时就自动减小充电电流
        以维持此电压；
        如果客户要求边充边放状态下可在输出
        端加采样电阻检测边充边放状态输出端
        的负载电流大于 100mA 时可把欠压环设
        置为最高，优先对外部负载充电。*/
        Charger_CTL1_BIT_ChargeUndervoltageRingSet = B00110000,
        
        /*************************** Charger_CTL2 ***************************/
        /*BAT 电池类型选择
        11：RESERVED
        10：4.35V 电池
        01：4.3V 电池
        00: 4.2V 电池*/
        Charger_CTL2_BIT_BatteryTypeSet = B00000110,
        
        /*恒压充电电压加压设置
        11：加压 42mV
        10：加压 28mV
        01：加压 14mV
        00: 不加压
        注：4.30V/4.35V 建议加压 14mV；
        4.2V 建议加压 28mV；
        如果客户需要支持 4.4V 的电芯，可以在
        4.35V 电池的基础上选择加压 48mV，充
        饱由 MCU 检测到电池电压大于 4.4V，电
        流小于 200MA 才认为是电芯充饱了；*/
        Charger_CTL2_BIT_ConstantVoltageForcingSet = B01100000,
        
        /*************************** CHG_DIG_CTL4 ***************************/
        /*电池类型内部寄存器设定还是外部 Vset PIN 设定选择
        1：外部 VSET PIN 设置
        0：内部寄存器设置
        如果是该 bit 为 0，可通过 0x24 寄存器的 bit6：5
        来设定电池类型*/
        CHG_DIG_CTL4_BIT_BatteryTypeInternalSet = 1 << 6,
        
        /*************************** CHG_DIG_CTL5 ***************************/
        /*充电电流设置(设置为电池端电流)：
        Ibat=b0*0.1+b1*0.2+b2*0.4+b3*0.8+b4*1.6A
        注：默认值为 10111 2.3A 左右*/
        CHG_DIG_CTL5_BIT_ChargeCurrentSet = B11111000,
        
        /*************************** MFP_CTL0 ***************************/
        /*LIGHT 功能选择
        00：WLED
        01：GPIO2
        10: VREF
        11 : Reserved*/
        MFP_CTL0_BIT_LIGHT_Sel = B00001100,
        
        /*L4 的功能选择
        00：L4
        01：GPIO1
        10: Reserved
        11: Reserved*/
        MFP_CTL0_BIT_L4_Sel = B00110000,
        
        /*L3的功能选择
        00：L3
        01：GPIO0
        10: Reserved
        11 : Reserved*/
        MFP_CTL0_BIT_L3_Sel = B11000000,
        
        /*************************** MFP_CTL1 ***************************/
        /*VSET功能选择
        00：电池电压选择PIN
        01：GPIO4
        10: Reserved
        11: Reserved*/
        MFP_CTL1_BIT_VSET_Sel = B00110000,
        
        /*RSET功能选择
        00：电池内阻选择PIN
        01：GPIO3
        10: Reserved
        11: Reserved*/
        MFP_CTL1_BIT_RSET_Sel = B11000000,
        
        /*************************** GPIO_CTL1 ***************************/
        /*GPIO[4:0]input enable
        0：Disable
        1：Enable*/
        GPIO_CTL1_BIT_GPIO_INEN = B11111000,
        
        /*************************** GPIO_CTL2 ***************************/
        /*GPIO[4:0]output enable
        0：Disable
        1：Enable
        在开启 Output 之前，需要先将 data 配好*/
        GPIO_CTL2_BIT_GPIO_OUTEN = B11111000,
        
        /*************************** GPIO_CTL3 ***************************/
        /*GPIO[4:0] DATA */
        GPIO_CTL3_BIT_GPIO_DAT = B11111000,
        
        /*************************** BATVADC_DAT0 ***************************/
        /*BATVADC 数据的低 8bit*/
        BATVADC_DAT0_BIT_BATVADC_Low_8Bit = B11111111,
        
        /*BATVADC 数据的高 6bit
        VBAT=BATVADC*0.26855mv+2.6V*/
        BATVADC_DAT0_BIT_BATVADC_High_6Bit = B11111100,
        
        /*************************** BATIADC_DAT0 ***************************/
        /*BATIADC 数据的低 8bit */
        BATIADC_DAT0_BIT_BATVADC_Low_8Bit = B11111111,
        
        /*BATIADC 数据的高 6bit
        VBAT=BATiADC*0.745985mA*/
        BATIADC_DAT0_BIT_BATVADC_High_6Bit = B11111100,
        
        /*************************** BATOCV_DAT0 ***************************/
        /*BATOCV 数据的低 8bit*/
        BATOCV_DAT0_BIT_BATOCV_Low_8Bit = B11111111,
        
        /***************************  BATOCV_DAT1 ***************************/
        /*BATOCV 数据的高 6bit
        OCV=BATOCV*0.26855mv+2.6V*/
        BATOCV_DAT1_BIT_BATOCV_High_6Bit = B11111100,
        
        /***************************  Reg_READ0 ***************************/
        /*充电状态指示标志
        000: idle
        001: 涓流充电状态
        010: 恒流充电状态
        011: 恒压充电状态
        100: 恒压停止充电检测
        101: 充饱状态
        110: 充电超时状态*/
        Reg_READ0_BIT_ChargeStatusFlags = B00000111,
        
        /*Chgop
        0：
        1*/
        Reg_READ0_BIT_Chgop = 1 << 4,
        
        /*充电结束标志
        0:
        1: 充电结束*/
        Reg_READ0_BIT_ChargeFinishFlag = 1 << 3,
        
        /*恒压超时标志
        0：
        1：恒压超时*/
        Reg_READ0_BIT_ConstantVoltageTimeoutFlag = 1 << 2,
        
        /*充电超时
        0：
        1：充电超时*/
        Reg_READ0_BIT_ChargeTimeoutFlag = 1 << 1,
        
        /*涓流充电超时
        0：
        1：涓流充电超时*/
        Reg_READ0_BIT_TrickleChargeTimeoutFlag = 1 << 0,
        
        /*************************** Reg_READ1 ***************************/
        /*Light LED 是否有接灯
        0：无 light LED 灯
        1：有 light LED 灯*/
        Reg_READ1_BIT_HaveLightLED = 1 << 7,
        
        /*轻载标志位
        0：重载状态(输出端电流大于 75mA)
        1：轻载状态(输出端电流小于 75mA)*/
        Reg_READ1_BIT_LowLoadFlag = 1 << 6,
        
        /*输入过压状态
        0：VIN 电压小于 5.6V
        1：VIN 电压大于 5.6V，输入过压状态*/
        Reg_READ1_BIT_InputOverVoltageFlag = 1 << 5,
        
        /*************************** Reg_READ2 ***************************/
        /*按键按键标志
        0：当前按键没有按键
        1：当前按键按下*/
        Reg_READ2_BIT_KeyPressFlag = 1 << 3,
        
        /*按键长按标志位
        0：
        1：onoff 发生长按*/
        Reg_READ2_BIT_KeyLongPressFlag = 1 << 1,
        
        /*按键短按标志位
        0：
        1：onoff 发生短按*/
        Reg_READ2_BIT_KeyClickPressFlag = 1 << 0
        
    }REG_BIT_t;

    uint8_t readReg(REG_t reg);
    bool readRegBit(REG_t reg, REG_BIT_t bit);
    void writeReg(REG_t reg, uint8_t val);
    void writeRegBit(REG_t reg, REG_BIT_t bit, bool val);
    
    float getBattVoltage();
    float getBattCurrent();
    float getBattOcVoltage();

private:
    uint8_t Address;
};

#endif
