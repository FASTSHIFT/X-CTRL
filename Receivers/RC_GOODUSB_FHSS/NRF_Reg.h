#ifndef __NRF_REG_H
#define __NRF_REG_H

#include <stdint.h>

namespace NRF_Reg
{
/*Configuration Register*/
typedef struct
{
    uint8_t PRIM_RX     : 1;/*  RX/TX control
                                1: PRX, 0: PTX */
    uint8_t PWR_UP      : 1;/*  1: POWER UP, 0:POWER DOWN*/
    uint8_t CRCO        : 1;/*  CRC encoding scheme
                                '0' - 1 byte
                                '1' �C 2 bytes */
    uint8_t EN_CRC      : 1;/*  Enable CRC. Forced high if one of the bits in the EN_AA is high*/
    uint8_t MASK_MAX_RT : 1;/*  Mask interrupt caused by MAX_RT
                                1: Interrupt not reflected on the IRQ pin
                                0: Reflect MAX_RT as active low interrupt on the IRQ pin*/
    uint8_t MASK_TX_DS  : 1;/*  Mask interrupt caused by TX_DS
                                1: Interrupt not reflected on the IRQ pin
                                0: Reflect TX_DS as active low interrupt on the IRQ pin*/
    uint8_t MASK_RX_DR  : 1;/*  Mask interrupt caused by RX_DR
                                1: Interrupt not reflected on the IRQ pin
                                0: Reflect RX_DR as active low interrupt on the IRQ pin*/
    uint8_t Reserved    : 1;/*  Only '0' allowed*/
} CONFIG_t;

/*Enable ��Auto Acknowledgment�� Function Disable
this functionality to be compatible with nRF2401, */
typedef struct
{
    uint8_t ENAA_P0  : 1;/*Enable auto acknowledgement data pipe 0*/
    uint8_t ENAA_P1  : 1;/*Enable auto acknowledgement data pipe 1*/
    uint8_t ENAA_P2  : 1;/*Enable auto acknowledgement data pipe 2*/
    uint8_t ENAA_P3  : 1;/*Enable auto acknowledgement data pipe 3*/
    uint8_t ENAA_P4  : 1;/*Enable auto acknowledgement data pipe 4*/
    uint8_t ENAA_P5  : 1;/*Enable auto acknowledgement data pipe 5*/
    uint8_t Reserved : 2;/*Only '00' allowed*/
} EN_AA_t;

/*Enabled RX Addresses*/
typedef struct
{
    uint8_t ERX_P0   : 1;/*Enable data pipe 0.*/
    uint8_t ERX_P1   : 1;/*Enable data pipe 1.*/
    uint8_t ERX_P2   : 1;/*Enable data pipe 2.*/
    uint8_t ERX_P3   : 1;/*Enable data pipe 3.*/
    uint8_t ERX_P4   : 1;/*Enable data pipe 4.*/
    uint8_t ERX_P5   : 1;/*Enable data pipe 5.*/
    uint8_t Reserved : 2;/*Only '00' allowed*/
} EN_RXADDR_t;

/*Setup of Address Widths
(common for all data pipes)*/
typedef struct
{
    uint8_t AW       : 2;/*  RX/TX Address field width
                             '00' - Illegal
                             '01' - 3 bytes
                             '10' - 4 bytes
                             '11' �C 5 bytes
                             LSByte is used if address width is below 5 bytes*/
    uint8_t Reserved : 6;/*  Only '000000' allowed*/
} SETUP_AW_t;

/*Setup of Automatic Retransmission*/
typedef struct
{
    uint8_t ARC : 4;/*  Auto Retransmit Delay
                        '0000' �C Wait 250��S
                        '0001' �C Wait 500��S
                        '0010' �C Wait 750��S
                        ...
                        '1111' �C Wait 4000��S
                        (Delay defined from end of transmission to start of next transmission)*/
    uint8_t ARD : 4;/*  Auto Retransmit Count
                        '0000�� �CRe-Transmit disabled
                        '0001' �C Up to 1 Re-Transmit on fail of AA
                        ...
                        '1111' �C Up to 15 Re-Transmit on fail of AA*/
} SETUP_RETR_t;

/*RF Channel*/
typedef struct
{
    uint8_t RF_CH    : 7;/* Sets the frequency channel nRF24L01+ operates on*/
    uint8_t Reserved : 1;/* Only '0' allowed*/
} RF_CH_t;

/*RF Setup Register*/
typedef struct
{
    uint8_t Obsolete   : 1;/*Don��t care*/
    uint8_t RF_PWR     : 2;/*Set RF output power in TX mode
                            '00' �C -18dBm
                            '01' �C -12dBm
                            '10' �C -6dBm
                            '11' �C 0dBm*/
    uint8_t RF_DR_HIGH : 1;/*Select between the high speed data rates. This bit
                            is don��t care if RF_DR_LOW is set.
                            Encoding:
                            [RF_DR_LOW, RF_DR_HIGH]:
                            '00' �C 1Mbps
                            '01' �C 2Mbps
                            '10' �C 250kbps
                            '11' �C Reserved*/
    uint8_t PLL_LOCK   : 1;/*Force PLL lock signal. Only used in test*/
    uint8_t RF_DR_LOW  : 1;/*Set RF Data Rate to 250kbps. See RF_DR_HIGH for encoding.*/
    uint8_t Reserved   : 1;/*Only '0' allowed*/
    uint8_t CONT_WAVE  : 1;/*Enables continuous carrier transmit when high.*/
}RF_SETUP_t;

/*Status Register (In parallel to the SPI command
word applied on the MOSI pin, the STATUS register
is shifted serially out on the MISO pin)*/
typedef struct
{
    uint8_t TX_FULL  : 1;/* TX FIFO full flag.
                            1: TX FIFO full.
                            0: Available locations in TX FIFO*/
    uint8_t RX_P_NO  : 3;/* Data pipe number for the payload available for
                            reading from RX_FIFO
                            000-101: Data Pipe Number
                            110: Not Used
                            111: RX FIFO Empty*/
    uint8_t MAX_RT   : 1;/* Maximum number of TX retransmits interrupt
                            Write 1 to clear bit. If MAX_RT is asserted it must
                            be cleared to enable further communication. */
    uint8_t TX_DS    : 1;/* Data Sent TX FIFO interrupt. Asserted when
                            packet transmitted on TX. If AUTO_ACK is activated, this bit is set high only when ACK is
                            received.
                            Write 1 to clear bit.*/
    uint8_t RX_DR    : 1;/* Data Ready RX FIFO interrupt. Asserted when
                            new data arrives RX FIFOc.
                            Write 1 to clear bit.*/
    uint8_t Reserved : 1;/* Only '0' allowed*/
}STATUS_t;

/*Transmit observe register*/
typedef struct
{
    uint8_t ARC_CNT  : 4;/* Count retransmitted packets. The counter is reset
                            when transmission of a new packet starts*/
    uint8_t PLOS_CNT : 4;/* Count lost packets. The counter is overflow protected to 15, and discontinues at max until reset.
                            The counter is reset by writing to RF_CH. */
}OBSERVE_TX_t;

/*Received Power Detector.*/
typedef struct
{
    uint8_t RPD      : 1;/* Received Power Detector. This register is called
                            CD (Carrier Detect) in the nRF24L01. The name is
                            different in nRF24L01+ due to the different input
                            power level threshold for this bit. */
    uint8_t Reserved : 7;
}RPD_t;

typedef struct
{
    uint64_t RX_ADDR_P0 : 40;/* Receive address data pipe 1. 5 Bytes maximum
                                length. (LSByte is written first. Write the number of
                                bytes defined by SETUP_AW)*/
    uint32_t Reserved   : 24;
}RX_ADDR_P0_t;

typedef struct
{
    uint64_t RX_ADDR_P1 : 40;/* Receive address data pipe 1. 5 Bytes maximum
                                length. (LSByte is written first. Write the number of
                                bytes defined by SETUP_AW)*/
    uint32_t Reserved   : 24;
}RX_ADDR_P1_t;

/*Receive address data pipe 2. Only LSB. MSBytes
are equal to RX_ADDR_P1[39:8]*/
typedef uint8_t RX_ADDR_P2_t;

/*Receive address data pipe 3. Only LSB. MSBytes
are equal to RX_ADDR_P1[39:8]*/
typedef uint8_t RX_ADDR_P3_t;

/*Receive address data pipe 4. Only LSB. MSBytes
are equal to RX_ADDR_P1[39:8]*/
typedef uint8_t RX_ADDR_P4_t;

/*Receive address data pipe 5. Only LSB. MSBytes
are equal to RX_ADDR_P1[39:8]*/
typedef uint8_t RX_ADDR_P5_t;

typedef struct
{
    uint64_t TX_ADDR    : 40;/* Transmit address. Used for a PTX device only.
                                (LSByte is written first)
                                Set RX_ADDR_P0 equal to this address to handle
                                automatic acknowledge if this is a PTX device with
                                Enhanced ShockBurst? enabled. */
    uint32_t Reserved   : 24;
}TX_ADDR_P1_t;

typedef struct
{
    uint8_t RX_PW_P0 : 6;/* Number of bytes in RX payload in data pipe 0 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P0_t;

typedef struct
{
    uint8_t RX_PW_P1 : 6;/* Number of bytes in RX payload in data pipe 1 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P1_t;

typedef struct
{
    uint8_t RX_PW_P2 : 6;/* Number of bytes in RX payload in data pipe 2 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P2_t;

typedef struct
{
    uint8_t RX_PW_P3 : 6;/* Number of bytes in RX payload in data pipe 3 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P3_t;

typedef struct
{
    uint8_t RX_PW_P4 : 6;/* Number of bytes in RX payload in data pipe 4 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P4_t;

typedef struct
{
    uint8_t RX_PW_P5 : 6;/* Number of bytes in RX payload in data pipe 5 (1 to
                            32 bytes).
                            0 Pipe not used
                            1 = 1 byte
                            ��
                            32 = 32 bytes*/
    uint8_t Reserved : 2;/* Only '00' allowed */
}RX_PW_P5_t;

/*FIFO Status Register*/
typedef struct
{
    uint8_t RX_EMPTY  : 1;/*RX FIFO empty flag.
                            1: RX FIFO empty.
                            0: Data in RX FIFO.*/
    uint8_t RX_FULL   : 1;/*RX FIFO full flag.
                            1: RX FIFO full.
                            0: Available locations in RX FIFO*/
    uint8_t Reserved1 : 2;/*Only '00' allowed*/
    uint8_t TX_EMPTY  : 1;/*TX FIFO empty flag.
                            1: TX FIFO empty.
                            0: Data in TX FIFO.*/
    uint8_t TX_FULL   : 1;/*TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO.*/
    uint8_t TX_REUSE  : 1;/*Used for a PTX device
                            Pulse the rfce high for at least 10��s to Reuse last
                            transmitted payload. TX payload reuse is active
                            until W_TX_PAYLOAD or FLUSH TX is executed.
                            TX_REUSE is set by the SPI command
                            REUSE_TX_PL, and is reset by the SPI commands
                            W_TX_PAYLOAD or FLUSH TX*/
    uint8_t Reserved2 : 1;/*Only '0' allowed*/
}FIFO_STATUS_t;

/*Enable dynamic payload length*/
typedef struct
{
    uint8_t DPL_P0   : 1;/* Enable dynamic payload length data pipe 0.(Requires EN_DPL and ENAA_P0)*/
    uint8_t DPL_P1   : 1;/* Enable dynamic payload length data pipe 1.(Requires EN_DPL and ENAA_P1)*/
    uint8_t DPL_P2   : 1;/* Enable dynamic payload length data pipe 2.(Requires EN_DPL and ENAA_P2)*/
    uint8_t DPL_P3   : 1;/* Enable dynamic payload length data pipe 3.(Requires EN_DPL and ENAA_P3)*/
    uint8_t DPL_P4   : 1;/* Enable dynamic payload length data pipe 4.(Requires EN_DPL and ENAA_P4)*/
    uint8_t DPL_P5   : 1;/* Enable dynamic payload length data pipe 5.(Requires EN_DPL and ENAA_P5)*/
    uint8_t Reserved : 2;/*Only '00' allowed*/
}DYNPD_t;

/*Feature Register*/
typedef struct
{
    uint8_t EN_DYN_ACK  : 1;/*Enables the W_TX_PAYLOAD_NOACK command*/
    uint8_t EN_ACK_PAYd : 1;/*Enables Payload with ACK*/
    uint8_t EN_DPL      : 1;/*Enables Dynamic Payload Length*/
    uint8_t Reserved    : 5;/*Only '00000' allowed*/
}FEATURE_t;

}

#endif
