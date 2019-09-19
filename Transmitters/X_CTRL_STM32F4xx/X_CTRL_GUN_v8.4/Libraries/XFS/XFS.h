#ifndef __XFS_H
#define __XFS_H

#include "Arduino.h"

class XFS5152CE {
public:
	HardwareSerial* XFS_Serial;

    typedef struct {
        uint8_t DataHead;
        uint8_t Length_HH;
        uint8_t Length_LL;
        uint8_t Commond;
        uint8_t EncodingFormat;
        char* Text;
    } XFS_Protocol_TypeDef;
    XFS_Protocol_TypeDef DataPack;
   /*
    *| 帧头(1Byte)| 数据区长度(2Byte)|           数据区(<4KByte)          |
    *|            |  高字节 | 低字节 | 命令字 | 文本编码格式 | 待合成文本 |
    *|    0xFD    |  0xHH   |  0xLL  |  0x01  |   0x00~0x03  |  ... ...   |
    */

    typedef enum {
        CMD_StartSynthesis = 0x01,//语音合成命令 
        CMD_StopSynthesis = 0x02,//停止合成命令，没有参数 
        CMD_PauseSynthesis = 0x03,//暂停合成命令，没有参数 
        CMD_RecoverySynthesis = 0x04,//恢复合成命令，没有参数 
        CMD_CheckChipStatus = 0x21,//芯片状态查询命令 
        CMD_PowerSavingMode = 0x88,//芯片进入省电模式 
        CMD_NormalMode = 0xFF//芯片从省电模式返回正常工作模式 
    } CMD_Type;//命令字
    void StartSynthesis(char* str);//开始合成
    void StartSynthesis(String str);//开始合成
    void SendCommond(CMD_Type cmd);
    void StopSynthesis();//停止合成
    void PauseSynthesis();//暂停合成
    void RecoverySynthesis();//恢复合成

    typedef enum {
        GB2312 = 0x00,
        GBK = 0x01,
        BIG5 = 0x02,
        UNICODE = 0x03
    } EncodingFormat_Type;//文本的编码格式
    void SetEncodingFormat(EncodingFormat_Type encodingFormat);

    typedef enum {
        ChipStatus_InitSuccessful = 0x4A,//初始化成功回传 
        ChipStatus_CorrectCommand = 0x41,//收到正确的命令帧回传 
        ChipStatus_ErrorCommand = 0x45,//收到不能识别命令帧回传 
        ChipStatus_Busy = 0x4E,//芯片忙碌状态回传
        ChipStatus_Idle = 0x4F//芯片空闲状态回传 
    } ChipStatus_Type;//芯片回传
    uint8_t ChipStatus;
    
    typedef enum{
        Style_Single,//？为 0，一字一顿的风格 
        Style_Continue//？为 1，正常合成 
    }Style_Type;//合成风格设置 [f?] 
    void SetStyle(Style_Type style);
    
    typedef enum{
        Language_Auto,//? 为 0，自动判断语种 
        Language_Chinese,//? 为 1，阿拉伯数字、度量单位、特殊符号等合成为中文 
        Language_English//? 为 2，阿拉伯数字、度量单位、特殊符号等合成为英文 
    }Language_Type;//合成语种设置 [g?] 
    void SetLanguage(Language_Type language);
    
    typedef enum{
        Articulation_Auto,//? 为 0，自动判断单词发音方式 
        Articulation_Letter,//? 为 1，字母发音方式 
        Articulation_Word//? 为 2，单词发音方式 
    }Articulation_Type;//设置单词的发音方式 [h?] 
    void SetArticulation(Articulation_Type articulation);
    
    typedef enum{
        Spell_Disable,//? 为 0，不识别汉语拼音 
        Spell_Enable//? 为 1，将“拼音＋1 位数字（声调）”识别为汉语拼音，例如： hao3 
    }Spell_Type;//设置对汉语拼音的识别 [i?]
    void SetSpell(Spell_Type spell);

    typedef enum {
        Reader_XiaoYan = 3,//? 为 3，设置发音人为小燕(女声, 推荐发音人) 
        Reader_XuJiu = 51,//? 为 51，设置发音人为许久(男声, 推荐发音人) 
        Reader_XuDuo = 52,//? 为 52，设置发音人为许多(男声) 
        Reader_XiaoPing = 53,//? 为 53，设置发音人为小萍(女声) 
        Reader_DonaldDuck = 54,//? 为 54，设置发音人为唐老鸭(效果器) 
        Reader_XuXiaoBao = 55//? 为 55，设置发音人为许小宝(女童声) 
    } Reader_Type;//选择发音人 [m?] 
    void SetReader(Reader_Type reader);
    
    typedef enum{
        NumberHandle_Auto,//? 为 0，自动判断  
        NumberHandle_Number,//? 为 1，数字作号码处理 
        NumberHandle_Value//? 为 2，数字作数值处理 
    }NumberHandle_Type;//设置数字处理策略 [n?]
    void SetNumberHandle(NumberHandle_Type numberHandle);
    
    typedef enum{
        ZeroPronunciation_Zero,//? 为 0，读成“zero
        ZeroPronunciation_O//? 为 1，读成“欧”音 
    }ZeroPronunciation_Type;//数字“0”在读 作英文、号码时 的读法 [o?]
    void SetZeroPronunciation(ZeroPronunciation_Type zeroPronunciation);
    
    void SetDelayTime(uint32_t ms);//合成过程中停 顿一段时间 [p?] ? 为无符号整数，表示停顿的时间长度，单位为毫秒(ms) 
    
    typedef enum{
        NamePronunciation_Auto,//? 为 0，自动判断姓氏读音 
        NamePronunciation_Constraint//? 为 1，强制使用姓氏读音规则 
    }NamePronunciation_Type;//设置姓名读音 策略 [r?] 
    void SetNamePronunciation(NamePronunciation_Type namePronunciation);
    
    void SetSpeed(int speed);//设置语速 [s?] ? 为语速值，取值：0～10 
    void SetIntonation(int intonation);//设置语调 [t?] ? 为语调值，取值：0～10
    void SetVolume(int volume);//设置音量 [v?] ? 为音量值，取值：0～10
    
    typedef enum{
        PromptTone_Disable,//? 为 0，不使用提示音 
        PromptTone_Enable//? 为 1，使用提示音 
    }PromptTone_Type;//设置提示音处理策略 [x?]
    void SetPromptTone(PromptTone_Type promptTone);
    
    typedef enum{
        OnePronunciation_Yao,//? 为 0，合成号码“1”时读成“幺
        OnePronunciation_Yi//? 为 1，合成号码“1”时读成“一” 
    }OnePronunciation_Type;//设置号码中“1”的读法 [y?]
    void SetOnePronunciation(OnePronunciation_Type onePronunciation);
    
    typedef enum{
        Rhythm_Diasble,//? 为 0，“ *”和“#”读出符号 
        Rhythm_Enable//? 为 1，处理成韵律，“*”用于断词，“#”用于停顿 
    }Rhythm_Type;//是否使用韵律 标记“*”和“#” [z?]
    void SetRhythm(Rhythm_Type rhythm);

    typedef enum{
        ConstraintSpell_YinPing,//阴平
        ConstraintSpell_YangPing,//阳平
        ConstraintSpell_ShangSheng,//上声
        ConstraintSpell_QuSheng,//去声
        ConstraintSpell_QingSheng,//轻声
    }ConstraintSpell_Type;
    void SetConstraintSpell(ConstraintSpell_Type constraintSpell);
    /*为单个汉字强 制指定拼音 [=?] 
     *? 为标记前一个汉字的拼音＋声调（1～5 分别表示阴平，阳平， 上声，去声和轻声）5 个声调。
     *例如：“着[=zhuo2]手”，“着”字 读作“zhuó” 
     */
    
    void SetRestoreDefault();//恢复默认的合成参数 [d] 所有设置（除发音人设置、语种设置外）恢复为默认值 

    XFS5152CE(HardwareSerial* Serialx, EncodingFormat_Type encodingFormat = GB2312);
    void TextCtrl(char c,int d);
	
private :
    
};

#endif
