#include "FileGroup.h"

void HMI_end()
{
  HMI.write(0xFF);
  HMI.write(0xFF);
  HMI.write(0xFF);
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

void HMI_ShowRPS()
{
  static int Angle_Last;
  int Angle = Get_RPS_PointerAngle(Car_RPS);
  if(Angle != Angle_Last)
  {
    HMI.print("zRPM.val="),HMI.print(Angle),HMI_end();
    Angle_Last = Angle;
  }
}

void HMI_Display()
{
  HMI.print("tSPD.txt=\"");
  if (Car_Kph < 10.0) HMI.print('0');
  HMI.print((int)Car_Kph);
  HMI.print('"');
  HMI_end();
  
  HMI_ShowRPS();
}
