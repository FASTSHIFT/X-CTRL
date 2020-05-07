#include "../RCX.h"

using namespace RCX;
using namespace Handshake;

static NRF_TRM*  nrfTRM  = NULL;
static NRF_FHSS* nrfFHSS = NULL;
static uint8_t NRF_TxBuff[32];
static uint8_t NRF_RxBuff[32];
static const uint8_t NRF_ComProcess_TimeMs = 10;

static Pack_t Master;//主机握手数据包

#define SLAVE_MAX 5 //从机列表长度
static Pack_t Slave_Grp[SLAVE_MAX] = {0};//从机握手列表
static uint8_t SlaveSelectIndex = 0;

void Handshake::Init(
    NRF_TRM*  trm,
    NRF_FHSS* fhss, 
    const char* name
)
{
    nrfTRM = trm;
    nrfFHSS = fhss;
    strncpy(Master.Description, name, sizeof(Master.Description));
}

const Pack_t* Handshake::GetSlave(uint8_t index)
{
    return &(Slave_Grp[index % SLAVE_MAX]);
}

uint8_t Handshake::GetSlaveSelectIndex()
{
    return SlaveSelectIndex;
}

uint8_t Handshake::GetSlaveID()
{
    return Master.ID;
}

Pack_t* Handshake::GetMaster()
{
    return &Master;
}

/**
  * @brief  检查列表内是否有重复从机
  * @param  id: 被检查的ID号
  * @retval true: 有重复ID, false: 无重复ID
  */
static bool CheckNewSlave(Pack_t* slave)
{
    for(int i = 0; i < SLAVE_MAX; i++)//遍历列表
    {
        if(slave->ID == Slave_Grp[i].ID)//查找是否有相同ID号
        {
            return false;
        }
    }
    return true;
}

/**
  * @brief  生成随机地址和随机跳频表
  * @param  hs: 列表地址
  * @param  randSeed: 随机种子
  * @retval 无
  */
static void GenerateAddrFreqList(Pack_t* hs)
{
    randomSeed(micros());//选择随机数种子
    for(uint8_t i = 0; i < sizeof(hs->Address); i++)
    {
        hs->Address[i] = random(0, 256);//生成随机地址
    }
    for(uint8_t i = 0; i < sizeof(hs->FerqList); i++)
    {
        hs->FerqList[i] = random(0, 126);//生成随机跳频表 2400~2525MHz
    }
}

/**
  * @brief  NRF通信处理(T = 10ms)
  * @param  无
  * @retval 无
  */
static void NRF_ComProcess_Handler()
{
    nrfTRM->TranRecv(NRF_TxBuff, NRF_RxBuff);
}

/**
  * @brief  与从机通信,交换数据
  * @param  *slave:从机握手包指针
  * @retval 无
  */
static void ComToSlave(Pack_t* slave)
{
    static uint32_t lastTime;
    if(millis() - lastTime >= NRF_ComProcess_TimeMs)
    {
        lastTime = millis();
        NRF_ComProcess_Handler();
    }
    *(Pack_t*)&NRF_TxBuff = Master;
    *slave = *(Pack_t*)&NRF_RxBuff;
}

/**
  * @brief  握手通信状态机
  * @param  State: 状态机状态
  * @param  SlaveNum: 从机位于列表内的编号
  * @param  Cmd: 命令
  * @retval 详见ret
  */
uint8_t Handshake::Process(uint8_t state, uint8_t slaveSelect, CMD_Type cmd)
{
    if(nrfTRM == NULL || nrfFHSS == NULL)
        return 0;
    
    static volatile uint8_t SlaveCnt = 0;//从机数量
    uint8_t ret = false;//函数返回值
    Pack_t Slave_temp = {0};

    switch(state)
    {
    case State_Prepare://握手准备状态
        SlaveCnt = 0;//从机数量清零
        memset(&NRF_TxBuff, 0, sizeof(NRF_TxBuff));
        memset(&NRF_RxBuff, 0, sizeof(NRF_RxBuff));
        memset(&Slave_Grp,  0, sizeof(Slave_Grp));

        /*配置NRF为广播*/
        nrfTRM->Basic->SetRF_Enable(false);
        nrfTRM->Basic->SetAddress(RCX_BROADCAST_ADDR);//设置广播地址
        nrfTRM->Basic->SetFreqency(RCX_BROADCAST_FREQ);//设置广播频段
        nrfTRM->Basic->SetSpeed(RCX_BROADCAST_SPEED);//设置广播通信速度
        nrfTRM->Basic->SetAutoRetryTimeout(NRF_ComProcess_TimeMs);
        nrfTRM->Basic->TX_Mode();
        nrfTRM->Basic->SetRF_Enable(true);

        Master.BroadcastHead = BroadcastHead_MasterWait;//设置帧头识别码为主机等待连接
        GenerateAddrFreqList(&Master);//生成随机地址和随机跳频表
        ret = true;//返回 成功
        break;

    case State_Search://广播搜索状态
        ComToSlave(&Slave_temp);//在广播地址交换数据，将搜到的主机信息存至Slave_temp
        if((Slave_temp.BroadcastHead == BroadcastHead_SlaveWait)//帧头识别码为从机等待连接
                && (CheckNewSlave(&Slave_temp) == true)//是否为新ID
                && (SlaveCnt < SLAVE_MAX))//未超出从机列表大小
        {
            Slave_Grp[SlaveCnt] = Slave_temp;//保存至从机列表
            SlaveCnt ++;//从机个数++
        }
        ret = SlaveCnt;//返回从机个数
        break;

    case State_ReqConnect://申请连接状态
        SlaveSelectIndex = slaveSelect % SLAVE_MAX;
        ComToSlave(&Slave_temp);//在广播地址交换数据
        Master.BroadcastHead = BroadcastHead_MasterAttach;//设置帧头识别码为主机绑定状态
        Master.ID = Slave_Grp[SlaveSelectIndex].ID;//设置主机ID与从机一致
        Master.Type = Slave_Grp[SlaveSelectIndex].Type;//设置主机类型与从机一致
        Master.CMD = cmd;//向从机发送命令
        
        if(Slave_temp.BroadcastHead == BroadcastHead_SlaveAttach)//如果从机的帧头识别码为从机绑定状态
        {
            ret = Slave_temp.CMD;//返回从机命令
        }
        else
        {
            ret = false;//返回 失败
        }
        break;

    case State_Connected://连接之后状态
        /*设置数据包类型*/
        SetTxObjectType(Master.Type);
        /*设置数据包ID*/
        SetTxObjectID(Master.ID);

        nrfTRM->Basic->SetRF_Enable(false);
        nrfTRM->Basic->SetAddress(Master.Address);//应用新地址
        nrfTRM->Basic->SetFreqency(Master.FerqList[0]);//应用新频道
        nrfTRM->Basic->SetSpeed(Master.Speed);//应用新通信速
        //nrfTRM->Basic->SetAutoRetry(0, 15);
        nrfTRM->Basic->SetAutoRetryTimeout(NRF_ComProcess_TimeMs - 3);
        nrfTRM->Basic->TX_Mode();
        nrfTRM->Basic->SetRF_Enable(true);
    
        nrfFHSS->SetFreqHoppingList(Master.FerqList, sizeof(Master.FerqList));

        ret = true;//返回 成功
        break;
    }
    return ret;
}
