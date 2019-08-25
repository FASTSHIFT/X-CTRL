#include "FileGroup.h"

void OLED_Display()
{
    //  oled.setCursor(1,0),oled.print(F("P:")),oled.print(CR_kpa),oled.print(F("kPa  "));
    //  oled.setCursor(1,1),oled.print(F("ALT:")),oled.print(CR_alt,1),oled.print(F("m  "));
    //  oled.setCursor(1,2),oled.print(F("ROLL: ")),oled.print(CR_roll,0),oled_Space(3);
    //  oled.setCursor(1,3),oled.print(F("PITC: ")),oled.print(CR_pitch,0),oled_Space(3);
    //  oled.setCursor(1,4),oled.print(F("FPV: ")),oled.print(CR_FPV_Vol,1),oled.print(F("V "));
    //  oled.setCursor(1,0),oled.print(F("SPD:")),oled.print(Car_Kph,1),oled.print(F("km/h "));
    //  oled.setCursor(1,1),oled.print(F("RPS:")),oled.print(Car_RPS,1),oled.print(F("r/s "));
    //  oled.putCache();
}

//void oled_Space(uint8_t num)
//{
//  while (num--)
//  {
//    oled.print(' ');
//  }
//}

void HMI_end()
{
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
}

void HMI_Display()
{
    Serial.print("tSPD.txt=\"");
    if (Car_Kph < 10.0) Serial.print('0');
    Serial.print((int)Car_Kph);
    Serial.print('"');
    HMI_end();

    HMI_ShowRPS();
}

void HMI_ShowRPS()
{
    static int Angle_Last;
    int Angle = Get_RPS_PointerAngle(Car_RPS);
    if(Angle != Angle_Last)
    {
        Serial.print("zRPM.val="), Serial.print(Angle), HMI_end();
        Angle_Last = Angle;
    }
}

int Get_RPS_PointerAngle(float rps)
{
    if (rps < 40.0)
    {
        return (270.0 + rps / 40.0 * 90.0);
    }
    else
    {
        return (rps / 40.0 * 90.0 - 90.0);
    }
}

