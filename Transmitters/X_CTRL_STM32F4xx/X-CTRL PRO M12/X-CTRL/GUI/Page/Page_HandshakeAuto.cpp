#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"

/*被选中的选项*/
static int16_t ItemSelect = 0;

/*可选选项最大个数*/
static uint8_t ItemSelect_MAX = 0;

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
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
    RCX::Handshake::Init(&nrfTRM, &nrfFHSS, XC_NAME);
    
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

        PageDelay(1);
    }

    /*显示搜索结果*/
    Serial.println(ItemSelect_MAX > 0 ? "Search Done!" : "Not Found!");
    PageDelay(200);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    page.PagePush(PAGE_CtrlPage);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    /*尝试连接从机*/
    Serial.println("Connecting...");

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
        PageDelay(1);
    }

    /*握手收尾设置，跳转至约定好的握手频率以及地址*/
    RCX::Handshake::Process(RCX::Handshake::State_Connected);

    /*如果未超时表示握手成功*/
    if(!IsTimeout)
    {
        Serial.println("Connect successfully");
        Audio_PlayMusic(MC_Type::MC_Connect);
    }
    else
    {
        Audio_PlayMusic(MC_Type::MC_UnstableConnect);
    }

    nrf.SetRF_Enable(false);
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(event == ButtonEvent_Type::EVENT_ButtonPress)
    {

    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_HandshakeAuto(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
