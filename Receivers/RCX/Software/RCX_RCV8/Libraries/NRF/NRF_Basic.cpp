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
/*ʹ�üĴ���ֱ�ӿ���GPIO*/
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
/*ʹ��Arduino��׼�������ƿ���GPIO*/
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

/*�Ĵ����ṹ��תu8����*/
#define REG2U8(reg)   (*((uint8_t*)(&(reg))))
/*��д�Ĵ������ṹ��*/
#define REGREAD(reg)  REG2U8((REG_##reg))=SPI_Read(READ_REG+reg)
#define REGWRITE(reg) SPI_RW_Reg(WRITE_REG+reg,REG2U8(REG_##reg))
    

/**
  * @brief  ���캯�����������ӵ����ű�ţ����SPI��ʽ
  * @param  mosi: ����MOSI�����ű��
  * @param  miso: ����MISO�����ű��
  * @param  sck:  ����SCK �����ű��
  * @param  se:   ����CE  �����ű��
  * @param  csn:  ����CSN�����ű��
  * @retval ��
  */
NRF_Basic::NRF_Basic(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ce, uint8_t csn)
{
    /*��ʹ��Ӳ��SPI*/
    hwSPI = false;
    
    /*���ű�Ŵ���*/
    MOSI_Pin = mosi;
    MISO_Pin = miso;
    SCK_Pin = sck;
    CE_Pin = ce;
    CSN_Pin = csn;

#ifdef USE_FAST_IO
    /*ȡGPIO�Ĵ�����ַ�����ڸ��ٿ���IO*/
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
  * @brief  ���캯�����������ӵ����ű�ţ�Ӳ��SPI��ʽ
  * @param  se:   ����CE  �����ű��
  * @param  csn:  ����CSN�����ű��
  * @retval ��
  */
NRF_Basic::NRF_Basic(uint8_t ce, uint8_t csn)
{    
    /*ʹ��Ӳ��SPI*/
    hwSPI = true;

    /*���ű�Ŵ���*/
    CE_Pin = ce;
    CSN_Pin = csn;
#ifdef USE_FAST_IO
    /*ȡGPIO�Ĵ�����ַ�����ڸ��ٿ���IO*/
    ceport = portOutputRegister(digitalPinToPort(CE_Pin));
    cepinmask = digitalPinToBitMask(CE_Pin);
    csnport = portOutputRegister(digitalPinToPort(CSN_Pin));
    csnpinmask = digitalPinToBitMask(CSN_Pin);
#endif
}
#endif

/**
  * @brief  ��ʼ��
  * @param  ��
  * @retval �Ƿ��ʼ���ɹ�
  */
bool NRF_Basic::Init()
{
#ifdef NRF_SPI_OBJECT
    if(hwSPI)
    {
        NRF_SPI_BEGIN();//Ӳ��SPI��ʼ��
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
  * @brief  ��λ
  * @param  ��
  * @retval true�ɹ� falseʧ��
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
  * @brief  ����Ĭ�ϲ���
  * @param  ��
  * @retval ��
  */
void NRF_Basic::SetDefault()
{
    /*Ĭ�ϲ���*/
    RF_Speed = SPEED_1Mbps;//ͨ������:1Mb/s
    RF_Power = POWER_0dBm;//ͨ�Ź���:0dBm(���)
    RF_Freq = 40;//ͨ��Ƶ��40
    RF_AutoRetryDelay = 1;//�Զ��ط���ʱ(us) = 250 + n * 250
    RF_AutoRetryCount = 15;//�Զ��ط�����15
    RF_AddressWidth = sizeof(RF_Address);//��ַ���5�ֽ�
    RF_TX_PloadWidth = 32;//�������ݰ�����
    RF_RX_PloadWidth = 32;//�������ݰ�����
    
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
  * @brief  ���¼Ĵ������ṹ��
  * @param  ��
  * @retval ��
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
  * @brief  ����ͨ�ŵ�ַ(5�ֽ�)
  * @param  addr0:��ַ��0���ֽ�
  * @param  addr1:��ַ��1���ֽ�
  * @param  addr2:��ַ��2���ֽ�
  * @param  addr3:��ַ��3���ֽ�
  * @param  addr4:��ַ��4���ֽ�
  * @retval ��
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
  * @brief  ����ͨ�ŵ�ַ(5�ֽ�)
  * @param  addr: һ������5���ֽڵ������ָ��
  * @retval ��
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
  * @brief  ���÷��͹���
  * @param  power: ����ֵ(��NRF_Power_TypeDef)
  * @retval ��
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
  * @brief  ����ͨ������
  * @param  speed:����(SPEED_250Kbps,SPEED_1Mbps,SPEED_2Mbps)
  * @retval ��
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
  * @brief  ����ͨ��Ƶ��
  * @param  freq:Ƶ��(0~125 -> 2.400GHz��2.525GHz)
  * @retval ��
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
  * @brief  �����Զ��ط���ʱ��
  * @param  timeMs:ʱ��
  * @retval ��
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
  * @brief  �����Զ��ط�
  * @param  delay:�Զ��ط���ʱ
  * @param  count:�Զ��ط�����
  * @retval ��
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
  * @brief  �����Զ�����ACK
  * @param  en:ʹ��
  * @retval ��
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
  * @brief  �������ݰ�����
  * @param  tx_payload:�������ݰ�����
  * @param  rx_payload:�������ݰ�����
  * @retval ��
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
  * @brief  ������Ƶʹ��
  * @param  enable:ʹ�ܿ���
  * @retval ��
  */
void NRF_Basic::SetRF_Enable(bool enable)
{
    enable ? NRF_CE_HIGH() : NRF_CE_LOW();
    RF_Enabled = enable;
}

/**
  * @brief  ��ȡ��ǰͨ�ŵ�ַ
  * @param  addr:�ڼ�λ��ַ
  * @retval addr��Ӧ��ַ
  */
uint8_t NRF_Basic::GetAddress(uint8_t addr)
{
    return RF_Address[addr % sizeof(RF_Address)];
}

/**
  * @brief  ��ȡ��ǰͨ�ŵ�ַ
  * @param  addr:��ַ
  * @param  istx:�Ƿ�Ϊ���͵�ַ
  * @retval ��
  */
void NRF_Basic::GetAddress(uint8_t *addr, bool istx)
{
    if(istx)
        SPI_Read_Buf(READ_REG + TX_ADDR, addr, RF_AddressWidth);
    else
        SPI_Read_Buf(READ_REG + RX_ADDR_P0, addr, RF_AddressWidth);
}

/**
  * @brief  ��ȡ��ǰͨ���ٶ�
  * @param  ��
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
  * @brief  ��ȡ��ǰͨ��Ƶ��
  * @param  ��
  * @retval Ƶ��ֵ(+2400MHz)
  */
uint8_t NRF_Basic::GetFreqency()
{
    REGREAD(RF_CH);
    RF_Freq = REG_RF_CH.RF_CH;
    return RF_Freq;
}

/**
  * @brief  SPI��д
  * @param  Data:����
  * @retval SPI���ص�����
  */
uint8_t NRF_Basic::SPI_RW(uint8_t Data)
{
    if(hwSPI)//���ʹ��Ӳ��SPI
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
  * @brief  ��д�Ĵ���
  * @param  reg:�Ĵ���
  * @param  value:ֵ
  * @retval �Ĵ�����״̬
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
  * @brief  ���Ĵ���
  * @param  reg:�Ĵ���
  * @retval �Ĵ�����״̬
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
  * @brief  ��״̬
  * @param  ��
  * @retval ״̬�Ĵ�����ֵ
  */
uint8_t NRF_Basic::GetStatus(void)
{
    REGREAD(STATUS);
    return REG2U8(REG_STATUS);
}

/**
  * @brief  ��״̬�Ĵ�����־λ
  * @param  ��
  * @retval ��
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
  * @brief  �ж�Ӳ�������Ƿ���ȷ
  * @param  ��
  * @retval true ����ȷ����; false ���Ӳ���ȷ
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
  * @brief  д��һ�����ݵ��Ĵ���
  * @param  reg:�Ĵ���
  * @param  *pBuf:����ͷ��ַ
  * @param  bytes:�����ֽ���
  * @retval �Ĵ�����״̬
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
  * @brief  ����һ�����ݵ�NRF_Basic�Ĵ���
  * @param  reg:�Ĵ���
  * @param  *pBuf:����ͷ��ַ
  * @param  bytes:�����ֽ���
  * @retval �Ĵ�����״̬
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
  * @brief  ����Ϊ����ģʽ
  * @param  rfDelay:�Ƿ���ʱ
  * @retval ��
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
  * @brief  ����Ϊ����ģʽ
  * @param  ��
  * @retval ��
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
  * @brief  �����ϵ�
  * @param  ��
  * @retval ��
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
  * @brief  ���ͻ���������
  * @param  *txbuff:���ͻ�������ַ
  * @retval ��
  */
void NRF_Basic::Tran(void* txbuff)
{  
    /*���־λ*/
    ClearFlag();
    /*�建����*/
    SPI_RW_Reg(FLUSH_TX, 0);
    /*��������*/
    SPI_Write_Buf(W_TX_PLOAD, (uint8_t*)txbuff, RF_TX_PloadWidth);
    /*���ͼ���++*/
    RF_TranCnt++;
}

/**
  * @brief  ���ͼ��
  * @param  ��
  * @retval 1:���ͳɹ� -1:���ͳ�ʱ
  */
int8_t NRF_Basic::TranCheck()
{
    int8_t retval = 0;
    /*��ȡ״̬�Ĵ���*/
    REGREAD(STATUS);
    /*�Ƿ��ͳɹ����ͳ�ʱ*/
    if(REG_STATUS.TX_DS || REG_STATUS.MAX_RT)
    {
        /*�Ƿ�ʱ*/
        if(REG_STATUS.MAX_RT)
        {
            retval = -1;
        }
        /*�Ƿ��ͳɹ�*/
        else if(REG_STATUS.TX_DS)
        {
            /*���ͳɹ�����++*/
            RF_TranSuccessCnt ++;
            retval = 1;
        }
        /*��״̬�Ĵ�����־λ*/
        REGWRITE(STATUS);
    }
    return retval;
}

/**
  * @brief  �������ݵ����ջ�����
  * @param  *rxbuff:���ջ�������ַ
  * @retval �Ƿ��յ�����
  */
bool NRF_Basic::Recv(void* rxbuff)
{
    bool retval = false;
    /*��ȡ״̬�Ĵ���*/
    REGREAD(STATUS);
    
    /*�Ƿ���ճɹ�*/
    if(REG_STATUS.RX_DR)
    {
        /*�ӻ�����ȡ������*/
        SPI_Read_Buf(R_RX_PLOAD, (uint8_t*)rxbuff, RF_RX_PloadWidth);
        /*����ջ�����*/
        SPI_RW_Reg(FLUSH_RX, 0);
        /*��״̬�Ĵ�����־λ*/
        REGWRITE(STATUS);
        
        retval = true;
    }    
    return retval;
}

/**
  * @brief  ��ȡ�ź�ǿ��(���ͳɹ���)
  * @param  ��
  * @retval �ź�ǿ��(0~100%)
  */
uint8_t NRF_Basic::GetRSSI()
{
    if(RF_TranCnt == 0) return 0;

    /*���ͳɹ����� ���� �����ܴ��� * 100% */
    int rssi = RF_TranSuccessCnt * 100 / RF_TranCnt;
    
    if(rssi > 100)
        rssi = 100;
    
    RF_TranSuccessCnt = RF_TranCnt = 0;
    RF_RSSI = rssi;
    return rssi;
}
