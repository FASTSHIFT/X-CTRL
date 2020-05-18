#include "Basic/FileGroup.h"
#include "ComPrivate.h"

/*被选中的选项*/
static int16_t ItemSelect = 0;

/*可选选项最大个数*/
static uint8_t ItemSelect_MAX = 0;

static void ConnectSetup()
{
    Com_SetEnable(false);//遥控关闭
    ItemSelect_MAX = 0;
    ItemSelect = 0;
    
    /*配置基本信息*/
    RCX::Handshake::Pack_t* master = RCX::Handshake::GetMaster();
    master->EnableFunction.Passback = CTRL.State.Passback;
    master->EnableFunction.FHSS = CTRL.State.FHSS;
    master->Speed = nrf.GetSpeed();

    /*握手初始化*/
    RCX::Handshake::Init(&nrfTRM, &nrfFHSS);
    
    /*主机准备握手*/
    RCX::Handshake::Process(RCX::Handshake::State_Prepare);
    
    Serial.println("Searching...");
    /*超时设置*/
    uint32_t time = millis();
    uint8_t ItemSelect_MAX_Last = 0;
    while(millis() - time < 5000)
    {
        /*获取从机列表数量*/
        ItemSelect_MAX = RCX::Handshake::Process(RCX::Handshake::State_Search);

        if(ItemSelect_MAX > ItemSelect_MAX_Last)
        {
            Serial.printf("Find %d Slave...\r\n", ItemSelect_MAX);
            ItemSelect_MAX_Last = ItemSelect_MAX;
            break;
        }
    }

    /*显示搜索结果*/
    Serial.println(ItemSelect_MAX > 0 ? "Search Done!" : "Not Found!");
}

static void ConnectProcess()
{
    /*尝试连接从机*/
    Serial.printf("Connecting %s...\r\n", RCX::Handshake::GetSlave(ItemSelect)->Description);

    /*超时设置*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*等待从机响应握手信号*/
    while(
        RCX::Handshake::Process(
            RCX::Handshake::State_ReqConnect, 
            ItemSelect, 
            RCX::Handshake::CMD_AttachConnect
        ) 
        != RCX::Handshake::CMD_AgreeConnect
    )
    {
        /*超时*/
        if(millis() - timeout > 1000)
        {
            Serial.println("Timeout");
            IsTimeout = true;
            break;
        }
    }

    /*握手收尾设置，跳转至约定好的握手频率以及地址*/
    RCX::Handshake::Process(RCX::Handshake::State_Connected);

    /*如果未超时表示握手成功*/
    if(!IsTimeout)
    {
        Serial.println("Connect successfully");
    }
    else
    {
        Serial.println("Connect timeout");
    }

    nrf.SetRF_Enable(false);
}

void Com_TestSetup()
{
    Serial.begin(115200);
    CTRL.State.Handshake = true;
    CTRL.State.FHSS = true;
    CTRL.State.Passback = false;
    
    if(Com_Init())
    {
        Serial.println("RF init successful");
    }
    ConnectSetup();
    ConnectProcess();
    
    nrf.SetRF_Enable(true);
    RCX::ChannelReset();
    Com_SetEnable(true);
}

static void Joystick_Update()
{
    CTRL.JS_L.X.Val++;
    CTRL.JS_R.Y.Val++;
    
    if(CTRL.JS_L.X.Val > RCX_CHANNEL_DATA_MAX)
        CTRL.JS_L.X.Val = -RCX_CHANNEL_DATA_MAX;
    
    if(CTRL.JS_R.Y.Val > RCX_CHANNEL_DATA_MAX)
        CTRL.JS_R.Y.Val = -RCX_CHANNEL_DATA_MAX;
}

void Com_TestLoop()
{
    __IntervalExecute(Com_Update(), 10);
    __IntervalExecute(Serial.printf("RSSI:%d\r\n", nrf.GetRSSI()), 1000);
    __IntervalExecute(Joystick_Update(), 1);
}
