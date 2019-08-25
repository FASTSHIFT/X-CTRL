#include "FileGroup.h"

const uint8_t Data_Head[2] = {0xAB, 0xCD};
const uint8_t Data_Tail[2] = {0xDC, 0xBA};
const uint8_t RX_DataLength = 18;
const uint8_t TX_DataLength = 18;
uint8_t TX_buff[14];
uint8_t RX_buff[14];
uint8_t RX_DataPackage_Num;

void TX_DataLoad()
{
    int16_t lp, rp, bp;
    int16_t lx, ly, rx, ry;
    if (CTRL_Object == CAR || CTRL_Object == CRAWLER)
    {
        lp = L_PWM + 300;
        TX_buff[0] = lp;
        TX_buff[1] = lp >> 8;

        rp = R_PWM + 300;
        TX_buff[2] = rp;
        TX_buff[3] = rp >> 8;

        TX_buff[4] = ServoPos.X;
        TX_buff[5] = ServoPos.X >> 8;

        TX_buff[6] = ServoPos.Y;
        TX_buff[7] = ServoPos.Y >> 8;

        TX_buff[8] = TX_buff[8] + 1;
    }
    else if (CTRL_Object == TANK)
    {
        lp = L_PWM + 300;
        TX_buff[0] = lp;
        TX_buff[1] = lp >> 8;

        rp = R_PWM + 300;
        TX_buff[2] = rp;
        TX_buff[3] = rp >> 8;

        bp = B_PWM + 300;
        TX_buff[4] = bp;
        TX_buff[5] = bp >> 8;
    }
    else if (CTRL_Object == CPTR || CTRL_Object == SPIDER)
    {
        lx = JS_L.X + 300;
        TX_buff[0] = lx;
        TX_buff[1] = lx >> 8;

        ly = JS_L.Y + 300;
        TX_buff[2] = ly;
        TX_buff[3] = ly >> 8;

        rx = JS_R.X + 300;
        TX_buff[4] = rx;
        TX_buff[5] = rx >> 8;

        ry = JS_R.Y + 300;
        TX_buff[6] = ry;
        TX_buff[7] = ry >> 8;

        TX_buff[8] = L_CRE;
        TX_buff[9] = L_CRE >> 8;

        TX_buff[10] = R_CRE;
        TX_buff[11] = R_CRE >> 8;
    }

    TX_buff[12] = (Key[k_up] << 7) | (Key[k_down] << 6) | (Key[k_ok] << 5) | (Key[k_back] << 4) | (Key[k_l] << 3) | (Key[k_r] << 2) | (Key[k_jsl] << 1) | Key[k_jsr];
    TX_buff[13] = CTRL_Object;
}

void SendData()
{
    if (CTRL_Module == Module_NRF)
    {
        //nrf.Tran(TX_buff);
        nrf.TranRecv(TX_buff, RX_buff);
    }
    else if (CTRL_Module == Module_UART)
    {
        Serial.write(Data_Head, sizeof(Data_Head));
        Serial.write(TX_buff,   sizeof(TX_buff));
        Serial.write(Data_Tail, sizeof(Data_Tail));
    }
}

void RX_DataLoad()
{
    Recv_TimePoint = millis() + 10000;
    uint8_t rx_data[RX_DataLength * 2], dbs = 0; //数据包缓冲的缓冲，数据位
    for (uint8_t i = 0; i < RX_DataLength * 2; i++)
    {
        rx_data[i] = Serial.read();//只抽取只有数据包长度的字节数
    }
    Serial.flush();

    //  for (uint8_t i = 0; i < DATA_LETH * 2; i++) //显示原始数据
    //  {
    //    Serial.print(rx_data[i], HEX);
    //    Serial.print(' ');
    //  }
    //  Serial.println(' ');

    while (dbs < RX_DataLength + 2)
    {
        if (rx_data[ dbs                       ] == Data_Head[0] &&
                rx_data[(dbs + 1)                  ] == Data_Head[1] &&
                rx_data[(dbs + (RX_DataLength - 2))] == Data_Tail[0] &&
                rx_data[(dbs + (RX_DataLength - 1))] == Data_Tail[1])
        {
            for (uint8_t i = 0; i < (RX_DataLength - 4); i++)//抽取包头包尾中间夹着的数据，填入缓冲区
            {
                RX_buff[i] = rx_data[dbs + 2 + i];
            }
            RX_Transform();//进入转换
            break;
        }
        dbs++;
    }
}

void RX_Transform()
{
    Slave_Temp = (RX_buff[1] << 8 | RX_buff[0]) / 10.0;
    Slave_BattVol = (RX_buff[3] << 8 | RX_buff[2]) / 10.0;
    //  if (CTRL_Object == CRAWLER)
    //  {
    //    CR_kpa = (RX_buff[5] << 8 | RX_buff[4]) / 100.0;
    //    CR_alt = (RX_buff[7] << 8 | RX_buff[6]) / 10.0;
    //
    //    CR_roll = RX_buff[9];
    //    if (!RX_buff[8])CR_roll = -CR_roll;
    //
    //    CR_pitch = RX_buff[11];
    //    if ((!RX_buff[10]))CR_pitch = -CR_pitch;
    //
    //    CR_FPV_Vol = RX_buff[12] / 10.0;
    //  }
    if (CTRL_Object == CAR)
    {
        Car_Kph = (RX_buff[5] << 8 | RX_buff[4]) / 100.0;
        Car_RPS = (RX_buff[7] << 8 | RX_buff[6]) / 100.0;
        RX_DataPackage_Num = RX_buff[13];
    }
}

void NRF_CheckConnection()
{
    static uint8_t RX_DataPackage_Num_Last;
    if (RX_DataPackage_Num != RX_DataPackage_Num_Last)Recv_TimePoint = millis() + 2000;
    RX_DataPackage_Num_Last = RX_DataPackage_Num;
}


