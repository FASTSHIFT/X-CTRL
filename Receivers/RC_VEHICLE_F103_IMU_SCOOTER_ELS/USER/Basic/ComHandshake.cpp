#include "FileGroup.h"
#include "ComPrivate.h"
#include "string.h"

Protocol_Handshake_t HandshakePack_Master;//主机握手数据包
Protocol_Handshake_t HandshakePack_Slave;//从机握手数据包
Protocol_Handshake_t HandshakePack_Attached;//绑定握手数据包

/**
  * @brief  NRF通信处理(T = 10ms)
  * @param  无
  * @retval 无
  */
static void NRF_ComProcess_Handler()
{
    nrf.RecvTran(NRF_RxBuff, NRF_TxBuff);
}

/**
  * @brief  与主机通信,交换数据
  * @param  *master:主机握手包指针
  * @retval 无
  */
static void ComToMaster(Protocol_Handshake_t *master)
{
#ifndef TIM_Handshake
    __IntervalExecute(NRF_ComProcess_Handler(), 10);
#endif
    *(Protocol_Handshake_t*)&NRF_TxBuff = HandshakePack_Slave;
    *master = *(Protocol_Handshake_t*)&NRF_RxBuff;
}

/**
  * @brief  主机握手状态机
  * @param  无
  * @retval true: 成功, false: 失败
  */
bool HandshakeToMaster(uint8_t setState)
{
    uint8_t ret = false;
    static uint8_t NowState = HandshakeState_Prepare;
    if(setState != 0xff) { NowState = setState; }
    uint32_t time;

    switch(NowState)
    {
    case HandshakeState_Prepare://握手准备状态
        nrf.SetAddress(BroadcastAddr);//设置广播地址
        nrf.SetFreqency(BroadcastFreq);//设置广播频段
        nrf.SetSpeed(BroadcastSpeed);//设置广播通信速度

        randomSeed((unsigned long)analogRead(BattDet_Pin));
        HandshakePack_Slave.HeadCode = BroadcastHead_SlaveWait;//设置帧头识别码为从机等待
        HandshakePack_Slave.ID = Slave_ID;//设置从机ID

        strcpy(HandshakePack_Slave.Description, Slave_Description);//设置从机文字描述
        HandshakePack_Slave.CtrlType = Slave_Type;//设置从机控制类型
        HandshakePack_Slave.SupportPassback = Slave_PassbackSupport;//设置是否支持回传
        HandshakePack_Slave.Speed = 0;//设置从机通信速度
        HandshakePack_Slave.Commond = CMD_ReqConnect;//设置从机命令:请求连接

#ifdef TIM_Handshake
        TimerSet(TIM_Handshake, 10 * 1000, NRF_ComProcess_Handler); //设置10ms定时中断运行NRF_ComProcess_Handler
        TIM_Cmd(TIM_Handshake, ENABLE);//开启定时器
#endif
        NowState = HandshakeState_Search;//状态机跳转至搜索状态
        break;

    case HandshakeState_Search://广播搜索状态
        ComToMaster(&HandshakePack_Master);//在广播地址交换数据，将搜到的主机信息存至HandshakePack_Master
        if( HandshakePack_Master.HeadCode == BroadcastHead_MasterAttach//帧头识别码为主机请求连接
                && HandshakePack_Master.Commond == CMD_AttachConnect//主机命令为请求绑定
                && HandshakePack_Master.ID == Slave_ID)//主机ID与从机ID一致
        {
            HandshakePack_Attached = HandshakePack_Master;//保存主机发过来的握手信息(地址，跳频表等)

            HandshakePack_Slave.HeadCode = BroadcastHead_SlaveAttach;//帧头识别码设置为从机响应绑定
            HandshakePack_Slave.Commond = CMD_AgreeConnect;//从机同意连接
            NowState = HandshakeState_ReqConnect;//状态机跳转至申请连接状态
        }
        break;

    case HandshakeState_ReqConnect://申请连接状态
        time = millis();
        while(millis() - time < 1000)//连续发送2秒数据包给主机表示可以连接
        {
            ComToMaster(&HandshakePack_Master);
        }

        NowState = HandshakeState_Connected;//状态机跳转至连接之后状态
#ifdef TIM_Handshake
        TIM_Cmd(TIM_Handshake, DISABLE);//关定时器中断
#endif
        break;

    case HandshakeState_Connected: //连接之后状态
        
        memcpy(
            NRF_Cfg.Address, 
            HandshakePack_Attached.Address, 
            sizeof(NRF_Cfg.Address)
        );//保存地址信息
        NRF_Cfg.Freq = HandshakePack_Attached.FerqList[0];//保存频率
        NRF_Cfg.Speed = HandshakePack_Attached.Speed;//保存通信速度
        EEPROM_Handle(EEPROM_Chs::SaveData);//保存所有数据
    
        nrf.SetAddress(HandshakePack_Attached.Address);//应用新地址
        nrf.SetFreqency(HandshakePack_Attached.FerqList[0]);//应用新频道
    
        if(Slave_PassbackSupport)
            PassBack_State = HandshakePack_Attached.SupportPassback;
    
        if(HandshakePack_Attached.Speed == 0)//应用新通信速率
            nrf.SetSpeed(nrf.SPEED_250Kbps);
        else if(HandshakePack_Attached.Speed == 1)
            nrf.SetSpeed(nrf.SPEED_1Mbps);
        else if(HandshakePack_Attached.Speed == 2)
            nrf.SetSpeed(nrf.SPEED_2Mbps);
        ret = true;
        ConnectState.TimePoint_Recv = millis();
        break;
    }
    return ret;
}
