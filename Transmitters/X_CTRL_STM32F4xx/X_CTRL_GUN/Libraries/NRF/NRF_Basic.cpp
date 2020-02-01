#include "NRF.h"

#ifdef NRF_SPI_OBJECT
#  if defined (__AVR__)
#    define NRF_SPI_BEGIN() {NRF_SPI_OBJECT.begin(),NRF_SPI_OBJECT.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));}
#  elif defined(__STM32__)
#    define NRF_SPI_BEGIN() {NRF_SPI_OBJECT.begin(SPISettings(10000000, MSBFIRST, SPI_MODE0));}
#  endif
#  define SPI_Transfer(x) NRF_SPI_OBJECT.transfer(x)
#endif

#ifdef USE_FAST_IO
/*使用寄存器直接控制GPIO*/
#  define FAST_IO_HIGH(port,mask) (*port|=mask)
#  define FAST_IO_LOW(port,mask)  (*port&=~mask)
#  define FAST_IO_READ(port,mask) ((*port&mask)!=0)

#  define NRF_MISO_READ() FAST_IO_READ(misoport,misopinmask)
#  define NRF_MOSI_HIGH() FAST_IO_HIGH(mosiport,mosipinmask)
#  define NRF_MOSI_LOW()  FAST_IO_LOW(mosiport,mosipinmask)
#  define NRF_SCK_HIGH()  FAST_IO_HIGH(sckport,sckpinmask)
#  define NRF_SCK_LOW()   FAST_IO_LOW(sckport,sckpinmask)
#  define NRF_CE_HIGH()   FAST_IO_HIGH(ceport,cepinmask)
#  define NRF_CE_LOW()    FAST_IO_LOW(ceport,cepinmask)
#  define NRF_CSN_HIGH()  FAST_IO_HIGH(csnport,csnpinmask)
#  define NRF_CSN_LOW()   FAST_IO_LOW(csnport,csnpinmask)

#else
/*使用Arduino标准函数控制控制GPIO*/
#  define NRF_MISO_READ() digitalRead(MISO_Pin)
#  define NRF_MOSI_HIGH() digitalWrite(MOSI_Pin,1)
#  define NRF_MOSI_LOW()  digitalWrite(MOSI_Pin,0)
#  define NRF_SCK_HIGH()  digitalWrite(SCK_Pin,1)
#  define NRF_SCK_LOW()   digitalWrite(SCK_Pin,0)
#  define NRF_CE_HIGH()   digitalWrite(CE_Pin,1)
#  define NRF_CE_LOW()    digitalWrite(CE_Pin,0)
#  define NRF_CSN_HIGH()  digitalWrite(CSN_Pin,1)
#  define NRF_CSN_LOW()   digitalWrite(CSN_Pin,0)
#endif

/*寄存器结构体转u8变量*/
#define REG2U8(reg)   (*((uint8_t*)(&(reg))))
/*读写寄存器到结构体*/
#define REGREAD(reg)  REG2U8((REG_##reg))=SPI_Read(READ_REG+reg)
#define REGWRITE(reg) SPI_RW_Reg(WRITE_REG+reg,REG2U8(REG_##reg))
    

/**
  * @brief  构造函数，传入连接的引脚编号，软件SPI方式
  * @param  mosi: 连接MOSI的引脚编号
  * @param  miso: 连接MISO的引脚编号
  * @param  sck:  连接SCK 的引脚编号
  * @param  se:   连接CE  的引脚编号
  * @param  csn:  连接CSN的引脚编号
  * @retval 无
  */
NRF_Basic::NRF_Basic(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ce, uint8_t csn)
{
    /*不使用硬件SPI*/
    hwSPI = false;
    
    /*引脚编号传入*/
    MOSI_Pin = mosi;
    MISO_Pin = miso;
    SCK_Pin = sck;
    CE_Pin = ce;
    CSN_Pin = csn;

#ifdef USE_FAST_IO
    /*取GPIO寄存器地址，用于高速控制IO*/
    mosiport    = portOutputRegister(digitalPinToPort(MOSI_Pin));
    mosipinmask = digitalPinToBitMask(MOSI_Pin);
    misoport    = portInputRegister(digitalPinToPort(MISO_Pin));
    misopinmask = digitalPinToBitMask(MISO_Pin);
    sckport     = portOutputRegister(digitalPinToPort(SCK_Pin));
    sckpinmask  = digitalPinToBitMask(SCK_Pin);

    ceport  = portOutputRegister(digitalPinToPort(CE_Pin));
    cepinmask   = digitalPinToBitMask(CE_Pin);
    csnport = portOutputRegister(digitalPinToPort(CSN_Pin));
    csnpinmask  = digitalPinToBitMask(CSN_Pin);
#endif
}

#ifdef NRF_SPI_OBJECT
/**
  * @brief  构造函数，传入连接的引脚编号，硬件SPI方式
  * @param  se:   连接CE  的引脚编号
  * @param  csn:  连接CSN的引脚编号
  * @retval 无
  */
NRF_Basic::NRF_Basic(uint8_t ce, uint8_t csn)
{    
    /*使用硬件SPI*/
    hwSPI = true;

    /*引脚编号传入*/
    CE_Pin = ce;
    CSN_Pin = csn;
#ifdef USE_FAST_IO
    /*取GPIO寄存器地址，用于高速控制IO*/
    ceport = portOutputRegister(digitalPinToPort(CE_Pin));
    cepinmask = digitalPinToBitMask(CE_Pin);
    csnport = portOutputRegister(digitalPinToPort(CSN_Pin));
    csnpinmask = digitalPinToBitMask(CSN_Pin);
#endif
}
#endif

/**
  * @brief  初始化
  * @param  无
  * @retval 是否初始化成功
  */
bool NRF_Basic::Init()
{
#ifdef NRF_SPI_OBJECT
    if(hwSPI)
    {
        NRF_SPI_BEGIN();//硬件SPI初始化
    }
    else
    {
        pinMode(MOSI_Pin, OUTPUT);
        pinMode(MISO_Pin, INPUT);
        pinMode(SCK_Pin, OUTPUT);
    }
#else
    pinMode(MOSI_Pin, OUTPUT);
    pinMode(MISO_Pin, INPUT);
    pinMode(SCK_Pin, OUTPUT);
#endif

    pinMode(CE_Pin, OUTPUT);
    pinMode(CSN_Pin, OUTPUT);
    
    NRF_CE_LOW();
    NRF_CSN_HIGH();
    
    bool isRst = Reset();
    ClearFlag();
    SetDefault();
    
    return (IsDetect() && isRst);
}

/**
  * @brief  复位
  * @param  无
  * @retval true成功 false失败
  */
bool NRF_Basic::Reset()
{
    SetRF_Enable(false);
    SPI_RW_Reg(FLUSH_TX, 0);
    SPI_RW_Reg(FLUSH_RX, 0);
    uint8_t status1 = SPI_RW_Reg(NOP, 0);
    uint8_t status2 = GetStatus();
    SetPowerUp(false);
    return (status1 == status2 && (status1 & 0x0F) == 0x0E);
}

/**
  * @brief  设置默认参数
  * @param  无
  * @retval 无
  */
void NRF_Basic::SetDefault()
{
    /*默认参数*/
    RF_Speed = SPEED_1Mbps;//通信速率:1Mb/s
    RF_Power = POWER_0dBm;//通信功率:0dBm(最大)
    RF_Freq = 40;//通信频率40
    RF_AutoRetryDelay = 1;//自动重发延时(us) = 250 + n * 250
    RF_AutoRetryCount = 15;//自动重发次数15
    RF_AddressWidth = sizeof(RF_Address);//地址宽度5字节
    RF_TX_PloadWidth = 32;//发送数据包长度
    RF_RX_PloadWidth = 32;//接收数据包长度
    
    /*RF Disable*/
    SetRF_Enable(false);
    
    /*Configuration Register*/
    REG_CONFIG.MASK_RX_DR  = 0;
    REG_CONFIG.MASK_TX_DS  = 0;
    REG_CONFIG.MASK_MAX_RT = 0;
    REG_CONFIG.EN_CRC      = 1;
    REG_CONFIG.CRCO        = 1;
    REG_CONFIG.PWR_UP      = 1;
    REG_CONFIG.PRIM_RX     = 1;
    REGWRITE(CONFIG);
    
    /*Auto Acknowledgment*/
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);
    
    /*Enabled RX Addresses*/
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);
    
    /*Channel*/
    SetFreqency(RF_Freq);
    
    /*Rate*/
    SetSpeed(RF_Speed);
    
    /*RF Power*/
    SetPower(RF_Power);
    
    /*RF PayloadWidth*/
    SetPayloadWidth(RF_TX_PloadWidth, RF_RX_PloadWidth);
    
    /*TX*/
    SPI_Write_Buf(WRITE_REG + TX_ADDR, RF_Address, RF_AddressWidth);
    SetAutoRetry(RF_AutoRetryDelay, RF_AutoRetryCount);
    
    /*RX*/
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RF_Address, RF_AddressWidth);
    SPI_RW_Reg(WRITE_REG + RX_PW_P0, RF_RX_PloadWidth);
}

/**
  * @brief  更新寄存器到结构体
  * @param  无
  * @retval 无
  */
void NRF_Basic::UpdateRegs()
{
    REGREAD(CONFIG);
    REGREAD(EN_AA);
    REGREAD(EN_RXADDR);
    REGREAD(SETUP_AW);
    REGREAD(SETUP_RETR);
    REGREAD(RF_CH);
    REGREAD(RF_SETUP);
    REGREAD(STATUS);
    REGREAD(OBSERVE_TX);
}

/**
  * @brief  设置通信地址(5字节)
  * @param  addr0:地址第0个字节
  * @param  addr1:地址第1个字节
  * @param  addr2:地址第2个字节
  * @param  addr3:地址第3个字节
  * @param  addr4:地址第4个字节
  * @retval 无
  */
void NRF_Basic::SetAddress(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4)
{
    NRF_CE_LOW();
    RF_Address[0] = addr0;
    RF_Address[1] = addr1;
    RF_Address[2] = addr2;
    RF_Address[3] = addr3;
    RF_Address[4] = addr4;

    SPI_Write_Buf(WRITE_REG + TX_ADDR, RF_Address, RF_AddressWidth);
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RF_Address, RF_AddressWidth);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置通信地址(5字节)
  * @param  addr: 一个包含5个字节的数组的指针
  * @retval 无
  */
void NRF_Basic::SetAddress(uint8_t* addr)
{
    NRF_CE_LOW();
    RF_Address[0] = addr[0];
    RF_Address[1] = addr[1];
    RF_Address[2] = addr[2];
    RF_Address[3] = addr[3];
    RF_Address[4] = addr[4];

    SPI_Write_Buf(WRITE_REG + TX_ADDR, RF_Address, RF_AddressWidth);
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RF_Address, RF_AddressWidth);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置发送功率
  * @param  power: 功率值(见NRF_Power_TypeDef)
  * @retval 无
  */
void NRF_Basic::SetPower(Power_Type power)
{
    NRF_CE_LOW();
    REGREAD(RF_SETUP);
    REG_RF_SETUP.RF_PWR = RF_Power = power;
    REGWRITE(RF_SETUP);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置通信速率
  * @param  speed:速率(SPEED_250Kbps,SPEED_1Mbps,SPEED_2Mbps)
  * @retval 无
  */
void NRF_Basic::SetSpeed(Speed_Type speed)
{
    NRF_CE_LOW();
    /* 250Kbps:  31250 Byte/s */
    /*   1Mbps: 125000 Byte/s */
    /*   2Mbps: 250000 Byte/s */
    RF_Speed = speed;
    REGREAD(RF_SETUP);
    REG_RF_SETUP.RF_DR_LOW  = (RF_Speed & B10) ? 1 : 0;
    REG_RF_SETUP.RF_DR_HIGH = (RF_Speed & B01) ? 1 : 0;
    REGWRITE(RF_SETUP);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置通信频率
  * @param  freq:频率(0~125 -> 2.400GHz～2.525GHz)
  * @retval 无
  */
void NRF_Basic::SetFreqency(uint8_t freq)
{
    NRF_CE_LOW();
    /*Channel:0~125*/
    REG_RF_CH.RF_CH = RF_Freq = freq % 126;
    REGWRITE(RF_CH);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置自动重发总时间
  * @param  timeMs:时间
  * @retval 无
  */
void NRF_Basic::SetAutoRetryTimeout(uint16_t timeMs)
{
    /* 250Kbps: 1Byte 32us */
    uint8_t byteTimeUs = 32;
    /* 1Mbps: 1Byte 8us*/
    if(RF_Speed == SPEED_1Mbps)
    {
        byteTimeUs = 8;
    }
    /* 2Mbps: 1 Byte 4us */
    else if(RF_Speed == SPEED_2Mbps)
    {
        byteTimeUs = 4;
    }
    uint32_t packTimeUs = RF_TX_PloadWidth * byteTimeUs;
    uint16_t retryCount = (timeMs * 1000) / (packTimeUs + (RF_AutoRetryDelay + 1) * 250);
    if(retryCount > 15) retryCount = 15;
    SetAutoRetry(RF_AutoRetryDelay, retryCount);
}

/**
  * @brief  设置自动重发
  * @param  delay:自动重发延时
  * @param  count:自动重发次数
  * @retval 无
  */
void NRF_Basic::SetAutoRetry(uint8_t delay, uint8_t count)
{
    NRF_CE_LOW();
    REGREAD(SETUP_RETR);
    REG_SETUP_RETR.ARD = RF_AutoRetryDelay = delay;
    REG_SETUP_RETR.ARC = RF_AutoRetryCount = count;
    REGWRITE(SETUP_RETR);
    
    /* 250Kbps: 1Byte 32us */
    uint8_t byteTimeUs = 32;
    /* 1Mbps: 1Byte 8us*/
    if(RF_Speed == SPEED_1Mbps)
    {
        byteTimeUs = 8;
    }
    /* 2Mbps: 1 Byte 4us */
    else if(RF_Speed == SPEED_2Mbps)
    {
        byteTimeUs = 4;
    }
    uint32_t packTimeUs = RF_TX_PloadWidth * byteTimeUs;
    
    RF_TimeoutUs = (packTimeUs + 250 + delay * 250) * count;
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置自动返回ACK
  * @param  en:使能
  * @retval 无
  */
void NRF_Basic::SetAutoAck(bool en)
{
    NRF_CE_LOW();
    REGREAD(EN_AA);
    REG_EN_AA.ENAA_P0 = en;
    REG_EN_AA.ENAA_P1 = en;
    REG_EN_AA.ENAA_P2 = en;
    REG_EN_AA.ENAA_P3 = en;
    REG_EN_AA.ENAA_P4 = en;
    REG_EN_AA.ENAA_P5 = en;
    REGWRITE(EN_AA);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置数据包长度
  * @param  tx_payload:发送数据包长度
  * @param  rx_payload:接收数据包长度
  * @retval 无
  */
void NRF_Basic::SetPayloadWidth(uint8_t tx_payload, uint8_t rx_payload)
{
    NRF_CE_LOW();
    RF_TX_PloadWidth = tx_payload;
    RF_RX_PloadWidth = rx_payload;
    SPI_RW_Reg(WRITE_REG + RX_PW_P0, RF_RX_PloadWidth);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置射频使能
  * @param  enable:使能控制
  * @retval 无
  */
void NRF_Basic::SetRF_Enable(bool enable)
{
    enable ? NRF_CE_HIGH() : NRF_CE_LOW();
    RF_Enabled = enable;
}

/**
  * @brief  获取当前通信地址
  * @param  addr:第几位地址
  * @retval addr对应地址
  */
uint8_t NRF_Basic::GetAddress(uint8_t addr)
{
    return RF_Address[addr % sizeof(RF_Address)];
}

/**
  * @brief  获取当前通信地址
  * @param  addr:地址
  * @param  istx:是否为发送地址
  * @retval 无
  */
void NRF_Basic::GetAddress(uint8_t *addr, bool istx)
{
    if(istx)
        SPI_Read_Buf(READ_REG + TX_ADDR, addr, RF_AddressWidth);
    else
        SPI_Read_Buf(READ_REG + RX_ADDR_P0, addr, RF_AddressWidth);
}

/**
  * @brief  获取当前通信速度
  * @param  无
  * @retval 0~2 -> 250Kbps~2Mbps
  */
uint8_t NRF_Basic::GetSpeed()
{
    /*SPEED_250Kbps*/
    uint8_t speed = 0;

    if(RF_Speed == SPEED_1Mbps)
        speed = 1;
    else if(RF_Speed == SPEED_2Mbps)
        speed = 2;
    
    return speed;
}

/**
  * @brief  获取当前通信频率
  * @param  无
  * @retval 频率值(+2400MHz)
  */
uint8_t NRF_Basic::GetFreqency()
{
    REGREAD(RF_CH);
    RF_Freq = REG_RF_CH.RF_CH;
    return RF_Freq;
}

/**
  * @brief  SPI读写
  * @param  Data:数据
  * @retval SPI返回的数据
  */
uint8_t NRF_Basic::SPI_RW(uint8_t Data)
{
    if(hwSPI)//如果使用硬件SPI
    {
#ifdef NRF_SPI_OBJECT
        return (SPI_Transfer(Data));
#else
        return 0;
#endif
    }
    else
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            (Data & 0x80) ? NRF_MOSI_HIGH() : NRF_MOSI_LOW();
            NRF_SCK_HIGH();
            Data <<= 1;
            if(NRF_MISO_READ()) Data |= 1;
            NRF_SCK_LOW();
        }
        return Data;
    }
}

/**
  * @brief  读写寄存器
  * @param  reg:寄存器
  * @param  value:值
  * @retval 寄存器的状态
  */
uint8_t NRF_Basic::SPI_RW_Reg(uint8_t reg, uint8_t value)
{
    NRF_CSN_LOW();
    uint8_t status = SPI_RW(reg);
    SPI_RW(value);
    NRF_CSN_HIGH();
    return status;
}

/**
  * @brief  读寄存器
  * @param  reg:寄存器
  * @retval 寄存器的状态
  */
uint8_t NRF_Basic::SPI_Read(uint8_t reg)
{
    NRF_CSN_LOW();
    SPI_RW(reg);
    uint8_t reg_val = SPI_RW(0);
    NRF_CSN_HIGH();
    return reg_val;
}

/**
  * @brief  读状态
  * @param  无
  * @retval 状态寄存器的值
  */
uint8_t NRF_Basic::GetStatus(void)
{
    REGREAD(STATUS);
    return REG2U8(REG_STATUS);
}

/**
  * @brief  清状态寄存器标志位
  * @param  无
  * @retval 无
  */
void NRF_Basic::ClearFlag()
{
    REGREAD(STATUS);
    REG_STATUS.RX_DR = 1;
    REG_STATUS.TX_DS = 1;
    REG_STATUS.MAX_RT = 1;
    REGWRITE(STATUS);
}

/**
  * @brief  判断硬件连线是否正确
  * @param  无
  * @retval true 已正确连接; false 连接不正确
  */
bool NRF_Basic::IsDetect()
{
    uint8_t detect = true;
    uint8_t addr[sizeof(RF_Address)] = {0};
    memcpy(addr, RF_Address, sizeof(addr));
    SetAddress(0, 1, 2, 3, 4);
    GetAddress(RF_Address);
    for(uint8_t i = 0;i < RF_AddressWidth;i++)
    {
        if(RF_Address[i] != i)
        {
            detect = false;
        }
    }
    SetAddress(addr);
    
    return detect;
}

/**
  * @brief  写入一串数据到寄存器
  * @param  reg:寄存器
  * @param  *pBuf:数组头地址
  * @param  bytes:数据字节数
  * @retval 寄存器的状态
  */
uint8_t NRF_Basic::SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status;

    NRF_CSN_LOW();
    status = SPI_RW(reg);
    for(uint8_t i = 0; i < bytes; i++) SPI_RW(*pBuf++);
    NRF_CSN_HIGH();
    return status;
}

/**
  * @brief  读入一串数据到NRF_Basic寄存器
  * @param  reg:寄存器
  * @param  *pBuf:数组头地址
  * @param  bytes:数据字节数
  * @retval 寄存器的状态
  */
uint8_t NRF_Basic::SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    NRF_CSN_LOW();
    uint8_t status = SPI_RW(reg);
    for(uint8_t i = 0; i < bytes; i++) pBuf[i] = SPI_RW(0);
    NRF_CSN_HIGH();
    return status;
}

/**
  * @brief  设置为发送模式
  * @param  rfDelay:是否延时
  * @retval 无
  */
void NRF_Basic::TX_Mode(bool rfDelay)
{
    RF_State = State_TX;
    
    NRF_CE_LOW();
    ClearFlag();
    REG_CONFIG.PRIM_RX = 0;
    REGWRITE(CONFIG);
    
    if(rfDelay)
    {
        delayMicroseconds(130);
    }
    
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置为接收模式
  * @param  无
  * @retval 无
  */
void NRF_Basic::RX_Mode(bool rfDelay)
{
    RF_State = State_RX;
    
    NRF_CE_LOW();
    ClearFlag();
    REG_CONFIG.PRIM_RX = 1;
    REGWRITE(CONFIG);
    
    if(rfDelay)
    {
        delayMicroseconds(130);
    }
    
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  设置上电
  * @param  无
  * @retval 无
  */
void NRF_Basic::SetPowerUp(bool en)
{
    NRF_CE_LOW();
    REGREAD(CONFIG);
    if(en)
    {
        REG_CONFIG.PWR_UP = 1;
    }
    else
    {
        REG2U8(REG_CONFIG) = 0;
        REG_CONFIG.EN_CRC = 1;
        RF_Enabled = false;
    }
    REGWRITE(CONFIG);
    SetRF_Enable(RF_Enabled);
}

/**
  * @brief  发送缓冲区数据
  * @param  *txbuff:发送缓冲区地址
  * @retval 无
  */
void NRF_Basic::Tran(void* txbuff)
{  
    /*清标志位*/
    ClearFlag();
    /*清缓冲区*/
    SPI_RW_Reg(FLUSH_TX, 0);
    /*发送数据*/
    SPI_Write_Buf(W_TX_PLOAD, (uint8_t*)txbuff, RF_TX_PloadWidth);
    /*发送计数++*/
    Tran_Cnt++;
}

/**
  * @brief  发送检查
  * @param  无
  * @retval 1:发送成功 -1:发送超时
  */
int8_t NRF_Basic::TranCheck()
{
    int8_t retval = 0;
    /*读取状态寄存器*/
    REGREAD(STATUS);
    /*是否发送成功或发送超时*/
    if(REG_STATUS.TX_DS || REG_STATUS.MAX_RT)
    {
        /*是否超时*/
        if(REG_STATUS.MAX_RT)
        {
            retval = -1;
        }
        /*是否发送成功*/
        else if(REG_STATUS.TX_DS)
        {
            /*发送成功计数++*/
            TranSuccess_Cnt ++;
            retval = 1;
        }
        /*清状态寄存器标志位*/
        REGWRITE(STATUS);
    }
    return retval;
}

/**
  * @brief  接收数据到接收缓冲区
  * @param  *rxbuff:接收缓冲区地址
  * @retval 是否收到数据
  */
bool NRF_Basic::Recv(void* rxbuff)
{
    bool retval = false;
    /*读取状态寄存器*/
    REGREAD(STATUS);
    
    /*是否接收成功*/
    if(REG_STATUS.RX_DR)
    {
        /*从缓冲区取出数据*/
        SPI_Read_Buf(R_RX_PLOAD, (uint8_t*)rxbuff, RF_RX_PloadWidth);
        /*清接收缓冲区*/
        SPI_RW_Reg(FLUSH_RX, 0);
        /*清状态寄存器标志位*/
        REGWRITE(STATUS);
        
        retval = true;
    }    
    return retval;
}

/**
  * @brief  获取信号强度(发送成功率)
  * @param  无
  * @retval 信号强度(0.0~100.0%)
  */
float NRF_Basic::GetRSSI()
{
    if(Tran_Cnt == 0) return 0.0;

    /*发送成功次数 除以 发送总次数 * 100% */
    float rssi = (float)TranSuccess_Cnt * 100.0f / Tran_Cnt;
    
    if(rssi > 100.0f)
        rssi = 100.0f;
    
    TranSuccess_Cnt = Tran_Cnt = 0;
    return rssi;
}
