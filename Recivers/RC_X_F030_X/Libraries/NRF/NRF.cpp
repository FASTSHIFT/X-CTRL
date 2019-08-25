#include "NRF.h"

NRF::NRF(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ce, uint8_t csn)
{
    MOSI_Pin = mosi;
    MISO_Pin = miso;
    SCK_Pin = sck;
    CE_Pin = ce;
    CSN_Pin = csn;
    hwSPI = false;

    RF_Speed = SPEED_1Mbps;
    RF_Power = POWER_0dBm;
    RF_Freq = 40;
    Retry_Cnt = 0;
	ADR_WIDTH = 5;

#ifdef USE_FAST_IO
    mosiport	= portOutputRegister(digitalPinToPort(MOSI_Pin));
    mosipinmask	= digitalPinToBitMask(MOSI_Pin);
    misoport	= portInputRegister(digitalPinToPort(MISO_Pin));
    misopinmask	= digitalPinToBitMask(MISO_Pin);
    sckport		= portOutputRegister(digitalPinToPort(SCK_Pin));
    sckpinmask	= digitalPinToBitMask(SCK_Pin);

    ceport	= portOutputRegister(digitalPinToPort(CE_Pin));
    cepinmask	= digitalPinToBitMask(CE_Pin);
    csnport	= portOutputRegister(digitalPinToPort(CSN_Pin));
    csnpinmask	= digitalPinToBitMask(CSN_Pin);
#endif
}

#ifdef NRF_SPI_OBJECT
NRF::NRF(uint8_t ce, uint8_t csn)
{
    CE_Pin = ce;
    CSN_Pin = csn;
    hwSPI = true;

    RF_Speed = SPEED_1Mbps;
    RF_Power = POWER_0dBm;
    RF_Freq = 40;
    Retry_Cnt = 0;
	ADR_WIDTH = 5;

#ifdef USE_FAST_IO
    ceport	= portOutputRegister(digitalPinToPort(CE_Pin));
    cepinmask	= digitalPinToBitMask(CE_Pin);
    csnport	= portOutputRegister(digitalPinToPort(CSN_Pin));
    csnpinmask	= digitalPinToBitMask(CSN_Pin);
#endif
}
#endif

void NRF::init(NRF_Mode_TypeDef mode, uint8_t RX_length)
{
    init(RX_MODE, 0, 0, RX_length);
}

void NRF::init(NRF_Mode_TypeDef mode, uint8_t *buff, uint8_t TX_length)
{
    init(TX_MODE, buff, TX_length, 0);
}

void NRF::init(NRF_Mode_TypeDef mode, uint8_t *buff, uint8_t TX_length, uint8_t RX_length)
{
#ifdef NRF_SPI_OBJECT
    if(hwSPI)NRF_SPI_BEGIN();
#else
    pinMode(MOSI_Pin, OUTPUT);
    pinMode(MISO_Pin, INPUT_PULLDOWN);
    pinMode(SCK_Pin, OUTPUT);
#endif

    pinMode(CE_Pin, OUTPUT);
    pinMode(CSN_Pin, OUTPUT);

    NRF_CE_LOW();
    NRF_CSN_HIGH();

    switch(mode)
    {
    case RX_MODE:
        RX_PLOAD_WIDTH = RX_length;
        RX_Mode();
        break;

    case TX_MODE:
        TX_PLOAD_WIDTH = TX_length;
        TX_Mode(buff);
        break;

    case TXRX_MODE:
        RX_PLOAD_WIDTH = RX_length;
        TX_PLOAD_WIDTH = TX_length;
        NowState = TX_MODE;
        TX_Mode(buff);
        break;

    case RXTX_MODE:
        RX_PLOAD_WIDTH = RX_length;
        TX_PLOAD_WIDTH = TX_length;
        NowState = RX_MODE;
        RX_Mode();
        break;
    }
}

void NRF::SetAddress(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4)
{
    ADDRESS[0] = addr0;
    ADDRESS[1] = addr1;
    ADDRESS[2] = addr2;
    ADDRESS[3] = addr3;
    ADDRESS[4] = addr4;

    NRF_CE_LOW();
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDRESS, ADR_WIDTH);
    NRF_CE_HIGH();
}

void NRF::SetAddress(uint8_t* addr)
{
    ADDRESS[0] = addr[0];
    ADDRESS[1] = addr[1];
    ADDRESS[2] = addr[2];
    ADDRESS[3] = addr[3];
    ADDRESS[4] = addr[4];

    NRF_CE_LOW();
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDRESS, ADR_WIDTH);
    NRF_CE_HIGH();
}

void NRF::SetSpeed(NRF_Speed_TypeDef speed)
{
    RF_Speed = speed;

    NRF_CE_LOW();
    SPI_RW_Reg(WRITE_REG + RF_SETUP, RF_Speed);//250K:0x27, 1M:0x07, 2M:0x0F
    NRF_CE_HIGH();
}

void NRF::SetFreqency(uint8_t freq)
{
    RF_Freq = freq % 128;

    NRF_CE_LOW();
    SPI_RW_Reg(WRITE_REG + RF_CH, RF_Freq);//0~127
    NRF_CE_HIGH();
}

uint8_t NRF::GetAddress(uint8_t addr)
{
    if(addr < 5)return ADDRESS[addr];
    else return 0;
}

uint8_t NRF::SPI_RW(uint8_t Data)
{
    if(hwSPI)
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
            if(Data & 0x80) NRF_MOSI_HIGH();
            else NRF_MOSI_LOW();

            NRF_SCK_HIGH();
            Data <<= 1;
            if(NRF_MISO_READ()) Data |= 1;
            NRF_SCK_LOW();
        }
        return Data;
    }
}

uint8_t NRF::SPI_RW_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status_;

    NRF_CSN_LOW();
    status_ = SPI_RW(reg);
    SPI_RW(value);
    NRF_CSN_HIGH();
    return status_;
}

uint8_t NRF::SPI_Read(uint8_t reg)
{
    uint8_t reg_val;
    NRF_CSN_LOW();
    SPI_RW(reg);
    reg_val = SPI_RW(0);
    NRF_CSN_HIGH();
    return reg_val;
}

uint8_t NRF::Status(void)
{
    return SPI_Read(STATUS);
}

bool NRF::IsDetect()
{
    uint8_t status = SPI_Read(STATUS);
    if(status != 0x00 && status != 0xFF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t NRF::SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status_;

    NRF_CSN_LOW();
    status_ = SPI_RW(reg);
    for(uint8_t i = 0; i < bytes; i++) SPI_RW(*pBuf++);
    NRF_CSN_HIGH();
    return status_;
}

uint8_t NRF::SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
    uint8_t status_;

    NRF_CSN_LOW();
    status_ = SPI_RW(reg);
    for(uint8_t i = 0; i < bytes; i++) pBuf[i] = SPI_RW(0);
    NRF_CSN_HIGH();
    return status_;
}

void NRF::TX_Mode(uint8_t *txbuff)
{
    NRF_CE_LOW();
    SPI_Write_Buf(WRITE_REG + TX_ADDR, ADDRESS, ADR_WIDTH);
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDRESS, ADR_WIDTH);

    SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0E);
    SPI_RW_Reg(WRITE_REG + RF_CH, RF_Freq);
    SPI_RW_Reg(WRITE_REG + RF_SETUP, RF_Speed | RF_Power);
    SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1A);
    SPI_Write_Buf(WR_TX_PLOAD, txbuff, TX_PLOAD_WIDTH);

    NRF_CE_HIGH();
}

void NRF::RX_Mode()
{
    NRF_CE_LOW();
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDRESS, ADR_WIDTH);
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);
    SPI_RW_Reg(WRITE_REG + RF_CH, RF_Freq);
    SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);
    SPI_RW_Reg(WRITE_REG + RF_SETUP, RF_Speed | RF_Power);
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0F);
    NRF_CE_HIGH();
}

uint8_t NRF::Tran(uint8_t *txbuff)
{
    uint8_t status_ = SPI_Read(STATUS);
    if(status_ & TX_DS)
    {
		TranSuccess_Cnt ++;
		Tran_Cnt ++;
        SPI_RW_Reg(FLUSH_TX, 0);
        SPI_Write_Buf(WR_TX_PLOAD, txbuff, TX_PLOAD_WIDTH);
    }
    if(status_ & MAX_RT)
    {
		Tran_Cnt ++;
        SPI_RW_Reg(FLUSH_TX, 0);
        SPI_Write_Buf(WR_TX_PLOAD, txbuff, TX_PLOAD_WIDTH);
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status_);

    return status_;
}

uint8_t NRF::Recv(uint8_t *rxbuff)
{
    uint8_t status_ = SPI_Read(STATUS);
    if(status_ & RX_DR)
    {
        SPI_Read_Buf(RD_RX_PLOAD, rxbuff, RX_PLOAD_WIDTH);
        SPI_RW_Reg(FLUSH_RX, 0);
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status_);

    return status_;
}

uint8_t NRF::TranRecv(uint8_t *txbuff, uint8_t *rxbuff)
{
    if(NowState == TX_MODE)
    {
        uint8_t TX_State = Tran(txbuff);
        if(TX_State & TX_DS || TX_State & MAX_RT)
        {
            NowState = RX_MODE;
            RX_Mode();
        }
    }
    else
    {
        uint8_t RX_State = Recv(rxbuff);
        if(RX_State & RX_DR || Retry_Cnt == 1)
        {
			Retry_Cnt = 0;
            NowState = TX_MODE;
            TX_Mode(txbuff);
        }
        else Retry_Cnt++;
    }
    return NowState;
}

uint8_t NRF::RecvTran(uint8_t *rxbuff, uint8_t *txbuff)
{
    if(NowState == RX_MODE)
    {
        uint8_t RX_State = Recv(rxbuff);
        if(RX_State & RX_DR)
        {
            NowState = TX_MODE;
            TX_Mode(txbuff);
        }
    }
    else
    {
        uint8_t TX_State = Tran(txbuff);
        if(TX_State & TX_DS || TX_State & MAX_RT || Retry_Cnt == 1)
        {
			Retry_Cnt = 0;
            NowState = RX_MODE;
            RX_Mode();
        }
		else Retry_Cnt++;
    }
    return NowState;
}

float NRF::GetSignalStrength()
{
    float SignalStrength = (float)TranSuccess_Cnt * 100.0 / Tran_Cnt;
    TranSuccess_Cnt = 0;
    Tran_Cnt = 0;
    return SignalStrength;
}
