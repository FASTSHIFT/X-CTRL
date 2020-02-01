#include "RCX.h"

/*强制单字节对齐*/
#pragma pack (1)

typedef struct
{
    uint16_t HeadCode;
    uint8_t Type;
    uint8_t ID;
    float Channel[7];
} Protocol_Passback_xChannel_t;

typedef struct
{
    uint8_t X;
    uint8_t Y;
    uint8_t TextSize;
    uint16_t TextColor;
    uint16_t BackColor;
    char Text[25];
} Protocol_Passback_CommonDisplay_t;

#pragma pack ()

/*自定义回传数据包回调*/
static const uint8_t CallbackGrp_NUM = 10;
static RCX::CustomDataPack_Callback_t UserCustomPassBack_Group[CallbackGrp_NUM] = {0};

/**
  * @brief  设置自定义回传数据包回调
  * @param  无
  * @retval 无
  */
bool RCX::PassBackAddCustom(CustomDataPack_Callback_t callback)
{
    static uint8_t regCnt = 0;

    /*寻找是否有重复函数*/
    for(uint8_t i = 0; i < CallbackGrp_NUM; i++)
    {
        if(callback == UserCustomPassBack_Group[i])
            return true;
    }

    /*在新的空位填入函数地址*/
    if(regCnt < CallbackGrp_NUM)
    {
        UserCustomPassBack_Group[regCnt] = callback;
        regCnt++;
        return true;
    }
    else
        return false;
}

void RCX::PassbackProcess(uint8_t* rxbuff)
{
    /*数组第二位为0说明不是普通文本回传类型*/
    bool isTextPassback = (rxbuff[1] == 0) ? false : true;
    if(isTextPassback)
    {
    }
    else
    {
        /*调用用户自定义回传数据解析数据*/
        for(int i = 0; i < CallbackGrp_NUM; i++)
        {
            if(UserCustomPassBack_Group[i])
                UserCustomPassBack_Group[i](rxbuff);
            else
                break;
        }
    }
}
