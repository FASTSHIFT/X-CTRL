/**********************************************************************************************
                           Designed By _VIFEXTech For NRF24L01
Finish  2017.8.16  v1.0
Upgrade 2017.12.5  v1.1 增加初始化函数，以及地址设置函数
Upgrade 2018.6.16  v1.2 支持双向半双工数据回传
Upgrade 2018.6.18  v1.3 加入模式设定合法性检测，支持使用指针获取模块地址
Upgrade 2018.6.19  v1.4 支持硬件SPI，更高效的IO操作
Upgrade 2018.8.17  v1.5 寄存器级别优化
Upgrade 2018.9.6   v1.6 优化SPI
Upgrade 2018.9.7   v1.7 将GPIO指针初始化放入构造函数
Upgrade 2018.9.21  v1.8 增加 IsDetect() 判断是否检测到NRF
Upgrade 2018.10.5  v1.9 支持设置速率、发射功率、频率
Upgrade 2018.10.11 v2.0 根据丢包率判断信号强度
Upgrade 2018.10.21 v2.1 优化信号强度检测
Upgrade 2018.11.17 v2.2 将宏定义常量转移到class的枚举类型中
Upgrade 2018.11.28 v2.3 增加注释，删除重复的初始化函数
Upgrade 2019.3.15  v2.4 增加GetSpeed(),GetFreqency(),GetPower(),SetPower(NRF_Power_TypeDef power)函数，
                        增加namespace NRF_TypeDef限制类型定义作用域
Upgrade 2019.3.22  v2.5 增加TranRecvSafe()，RecvTranSafe()函数，提升在较差通信环境时双向通信的稳定性
Upgrade 2019.3.26  v2.6 将MISO引脚配置成浮空输入
Upgrade 2019.7.8   v2.7 添加掉电模式
Upgrade 2019.9.7   v2.8 修复NRF初始化失败，更新TranRecvSafe、IsDetect函数
Upgrade 2019.9.16  v2.9 添加射频使能控制，调整SPI读写寄存器函数使用权限
Upgrade 2020.1.3   v3.0 分离半双工通信功能，添加FHSS功能支持
Upgrade 2020.1.20  v3.1 单/双向跳频协议完成
**************************************************************************************************/

#ifndef __NRF_H
#define __NRF_H

#define __NRF_LIB_VERSION "v3.1"

#include "Arduino.h"
#include "NRF_Reg.h"

/*取消注释以支持硬件SPI方式*/
//#define NRF_SPI_OBJECT SPI

#ifdef NRF_SPI_OBJECT
#include "SPI.h"
#endif

namespace NRF_Port
{
#if defined (__AVR__)
typedef volatile uint8_t PortReg_t;
typedef uint8_t PortMask_t;
#define USE_FAST_IO
#elif defined(__STM32__)
#if defined(__STM32F0__)
typedef volatile uint16_t PortReg_t;
#else
typedef volatile uint32_t PortReg_t;
#endif
typedef uint16_t PortMask_t;
#define USE_FAST_IO
#endif
}//end of namespace NRF

/**NRF24L01 引脚分布图(模块正面,俯视)
  *----------------+-------+
  *|               |       |
  *|        _______|_______|
  *|        |8:IRQ | 7:MISO|
  *|        |6:MOSI| 5:SCK |
  *|        |4:CSN | 3:CE  |
  *|        |2:VCC | 1:GND |
  *----------------+-------+
  */

/*NRF基础控制*/
class NRF_Basic
{
public:

    bool hwSPI;
    uint8_t MOSI_Pin, MISO_Pin, SCK_Pin, CE_Pin, CSN_Pin;
    NRF_Basic(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ce, uint8_t csn);
#ifdef NRF_SPI_OBJECT
    NRF_Basic(uint8_t ce, uint8_t csn);
#endif

    /*通信速率选项*/
    typedef enum
    {
        SPEED_1Mbps = B00,
        SPEED_2Mbps = B01,
        SPEED_250Kbps = B10,
    } Speed_Type;

    /*通信功率选项*/
    typedef enum
    {
        POWER_m18dBm = B00,
        POWER_m12dBm = B01,
        POWER_m6dBm = B10,
        POWER_0dBm  = B11
    } Power_Type;
    
    typedef enum
    {
        RX_MODE,  // 接收模式
        TX_MODE,  // 发送模式
        TXRX_MODE,// 发送/接收模式(半双工通信，用于主机)
        RXTX_MODE // 接收/发送模式(半双工通信，用于从机)
    } Mode_Type;//通信模式选项
    
    typedef enum
    {
        State_RX,
        State_TX,
        State_WAIT
    } State_Type;

    void SetDefault();
    void SetAddress(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4);
    void SetAddress(uint8_t* addr);
    void SetSpeed(Speed_Type speed);
    void SetSpeed(uint8_t speedIndex)
    {
        const Speed_Type speed[] = {
            SPEED_250Kbps,
            SPEED_1Mbps,
            SPEED_2Mbps
        };
        SetSpeed(speed[speedIndex % 3]);
    }
    void SetFreqency(uint8_t freq);
    void SetChannel(uint8_t ch){SetFreqency(ch);};
    void SetPower(Power_Type power);
    void SetPayloadWidth(uint8_t tx_payload, uint8_t rx_payload);
    void SetAutoRetry(uint8_t delay, uint8_t count);
    void SetAutoRetryTimeout(uint16_t timeMs);
    void SetAutoAck(bool en);
    void SetRF_Enable(bool enable);
    void SetPowerUp(bool en = true);
    uint8_t GetAddress(uint8_t addr);
    void GetAddress(uint8_t *addr, bool istx = true);
    uint8_t GetSpeed();
    uint8_t GetFreqency();
    uint8_t GetPower();
    uint8_t GetStatus();
    float GetRSSI();
    uint8_t GetRF_State()
    {
        return RF_State;
    }

    bool Init();
    bool Reset();
    bool IsDetect();
    void ClearFlag();
    void Tran(void* txbuff);
    int8_t TranCheck();
    bool Recv(void* rxbuff);
    void TX_Mode(bool rfDelay = false);
    void RX_Mode(bool rfDelay = false);
    
    uint8_t SPI_RW(uint8_t Data);
    uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value);
    uint8_t SPI_Read(uint8_t reg);
    uint8_t SPI_Write_Buf(uint8_t reg, uint8_t* pBuf, uint8_t bytes);
    uint8_t SPI_Read_Buf(uint8_t reg, uint8_t* pBuf, uint8_t bytes);

    /*预指令*/
    enum COMMAND
    {
        READ_REG = 0x00,
        WRITE_REG = 0x20,
        R_RX_PLOAD = 0x61,
        W_TX_PLOAD = 0xA0,
        FLUSH_TX = 0xE1,
        FLUSH_RX = 0xE2,
        REUSE_TX_PL = 0xE3,
        R_RX_PL_WID = 0x60,
        W_ACK_PAYLOAD = 0xA8,
        W_TX_PAYLOAD_NOACK = 0xB0,
        NOP = 0xFF
    };
    
    /*状态标志位*/
    enum STATUS_FLAG
    {
        RX_DR = 0x40,
        TX_DS = 0x20,
        MAX_RT = 0x10
    };

    /*寄存器地址*/
    enum REGISTER_ADDR
    {
        CONFIG = 0x00,
        EN_AA = 0x01,
        EN_RXADDR = 0x02,
        SETUP_AW = 0x03,
        SETUP_RETR = 0x04,
        RF_CH = 0x05,
        RF_SETUP = 0x06,
        STATUS = 0x07,
        OBSERVE_TX = 0x08,
        CD = 0x09,
        RX_ADDR_P0 = 0x0A,
        RX_ADDR_P1 = 0x0B,
        RX_ADDR_P2 = 0x0C,
        RX_ADDR_P3 = 0x0D,
        RX_ADDR_P4 = 0x0E,
        RX_ADDR_P5 = 0x0F,
        TX_ADDR = 0x10,
        RX_PW_P0 = 0x11,
        RX_PW_P1 = 0x12,
        RX_PW_P2 = 0x13,
        RX_PW_P3 = 0x14,
        RX_PW_P4 = 0x15,
        RX_PW_P5 = 0x16,
        FIFO_STATUS = 0x17,
        DYNPD = 0x1C,
        FEATURE = 0x1D
    };

    bool    RF_Enabled;
    uint8_t RF_Freq;
    uint8_t RF_AutoRetryCount;
    uint8_t RF_AutoRetryDelay;
    uint16_t RF_TimeoutUs;
    uint8_t RF_Address[5];
    uint8_t RF_AddressWidth;
    uint8_t RF_TX_PloadWidth;
    uint8_t RF_RX_PloadWidth;
    State_Type RF_State;
    Speed_Type RF_Speed;
    Power_Type RF_Power;

    uint32_t Tran_Cnt, TranSuccess_Cnt;
#if defined(USE_FAST_IO)
    NRF_Port::PortReg_t *mosiport, *misoport, *sckport, *ceport, *csnport;
    NRF_Port::PortMask_t mosipinmask, misopinmask, sckpinmask, cepinmask, csnpinmask;
#endif
    
    NRF_Reg::CONFIG_t     REG_CONFIG;
    NRF_Reg::EN_AA_t      REG_EN_AA;
    NRF_Reg::EN_RXADDR_t  REG_EN_RXADDR;
    NRF_Reg::SETUP_AW_t   REG_SETUP_AW;
    NRF_Reg::SETUP_RETR_t REG_SETUP_RETR;
    NRF_Reg::RF_CH_t      REG_RF_CH;
    NRF_Reg::RF_SETUP_t   REG_RF_SETUP;
    NRF_Reg::STATUS_t     REG_STATUS;
    NRF_Reg::OBSERVE_TX_t REG_OBSERVE_TX;
    void UpdateRegs();
};

/*NRF半双工通信*/
class NRF_TRM
{
public:
    NRF_TRM(NRF_Basic* nrf)
    {
        Basic = nrf;
    }
    NRF_Basic* Basic;
    void TranRecv(void* txbuff, void* rxbuff);
    void RecvTran(void* rxbuff, void* txbuff);
};

/*NRF跳频+单工/半双工通信*/
class NRF_FHSS
{
public:
    NRF_FHSS(NRF_Basic* nrf)
    {
        Basic = nrf;
        FH_Enable = false;
        FH_List = NULL;
        FH_List_Length = 0;
        FH_List_Index = 0;
        LastRxTime = 0;
        InterruptTime = 10;
    }
    NRF_Basic* Basic;

    void SetFreqHoppingList(uint8_t* list, uint16_t length)
    {
        FH_List = list;
        FH_List_Length = length;
    }
    void TxProcess(void* txbuff);
    void TxProcess(void* txbuff, void* rxbuff);
    
    void RxProcess(void* rxbuff);
    void RxProcess(void* rxbuff, void* txbuff);
    
    uint16_t InterruptTime;
    uint32_t LastRxTime;
    
private:
    bool FH_Enable;
    const uint8_t* FH_List;
    uint16_t FH_List_Length;
    uint16_t FH_List_Index;
    void FH_Process();
};

#endif
