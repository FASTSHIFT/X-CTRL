#include "XFS.h"

#define XFS_DataHead (uint8_t)0xFD

XFS5152CE::XFS5152CE(HardwareSerial* Serialx, EncodingFormat_Type encodingFormat)
{
    XFS_Serial = Serialx;

    DataPack.DataHead = XFS_DataHead;
    DataPack.Length_HH = 0x00;
    DataPack.Length_LL = 0x00;

    DataPack.Commond = 0x00;
    DataPack.EncodingFormat = encodingFormat;
    
    ChipStatus = 0x00;
}

void XFS5152CE::StartSynthesis(char* str)
{
    uint16_t size = strlen(str) + 2;
    DataPack.Length_HH = highByte(size);
    DataPack.Length_LL = lowByte(size);
    DataPack.Commond = CMD_StartSynthesis;
    DataPack.Text = str;
    XFS_Serial->write((uint8_t*)&DataPack, 5);
    XFS_Serial->write(DataPack.Text, strlen(str));
}

void XFS5152CE::StartSynthesis(String str)
{
    StartSynthesis((char*)str.c_str());
}

void XFS5152CE::SendCommond(CMD_Type cmd)
{
    DataPack.Length_HH = 0x00;
    DataPack.Length_LL = 0x02;
    DataPack.Commond = cmd;
    XFS_Serial->write((uint8_t*)&DataPack, 4);
}

void XFS5152CE::StopSynthesis()
{
    SendCommond(CMD_StopSynthesis);
}

void XFS5152CE::PauseSynthesis()
{
    SendCommond(CMD_PauseSynthesis);
}

void XFS5152CE::RecoverySynthesis()
{
    SendCommond(CMD_RecoverySynthesis);
}

void XFS5152CE::TextCtrl(char c, int d)
{
    char str[10];
    if(d != -1)
        sprintf(str, "[%c%d]", c, d);
    else
        sprintf(str, "[%c]", c);
    StartSynthesis(str);
}

void XFS5152CE::SetEncodingFormat(EncodingFormat_Type encodingFormat)
{
    DataPack.EncodingFormat = encodingFormat;
}

void XFS5152CE::SetStyle(Style_Type style)
{
    TextCtrl('f', style);
}

void XFS5152CE::SetLanguage(Language_Type language)
{
    TextCtrl('g', language);
}

void XFS5152CE::SetArticulation(Articulation_Type articulation)
{
    TextCtrl('h', articulation);
}

void XFS5152CE::SetSpell(Spell_Type spell)
{
    TextCtrl('i', spell);
}

void XFS5152CE::SetReader(Reader_Type reader)
{
    TextCtrl('m', reader);
}

void XFS5152CE::SetNumberHandle(NumberHandle_Type numberHandle)
{
    TextCtrl('n', numberHandle);
}

void XFS5152CE::SetZeroPronunciation(ZeroPronunciation_Type zeroPronunciation)
{
    TextCtrl('o', zeroPronunciation);
}

void XFS5152CE::SetDelayTime(uint32_t ms)
{
    TextCtrl('p', ms);
}

void XFS5152CE::SetNamePronunciation(NamePronunciation_Type namePronunciation)
{
    TextCtrl('r', namePronunciation);
}

void XFS5152CE::SetSpeed(int speed)
{
    speed = constrain(speed, 0, 10);
    TextCtrl('s', speed);
}

void XFS5152CE::SetIntonation(int intonation)
{
    intonation = constrain(intonation, 0, 10);
    TextCtrl('t', intonation);
}

void XFS5152CE::SetVolume(int volume)
{
    volume = constrain(volume, 0, 10);
    TextCtrl('v', volume);
}

void XFS5152CE::SetPromptTone(PromptTone_Type promptTone)
{
    TextCtrl('x', promptTone);
}

void XFS5152CE::SetOnePronunciation(OnePronunciation_Type onePronunciation)
{
    TextCtrl('y', onePronunciation);
}

void XFS5152CE::SetRhythm(Rhythm_Type rhythm)
{
    TextCtrl('z', rhythm);
}

void XFS5152CE::SetConstraintSpell(ConstraintSpell_Type constraintSpell)
{
    TextCtrl('=', constraintSpell);
}

void XFS5152CE::SetRestoreDefault()
{
    TextCtrl('d', -1);
}
