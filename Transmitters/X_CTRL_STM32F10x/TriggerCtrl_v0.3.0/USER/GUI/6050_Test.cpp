#include "FileGroup.h"

void MPU_Test() {
    static int16_t MPUX, MPUY, oldMPUX, oldMPUY;
    tft.drawRect(100, 3, 58, 58, ST77XX_YELLOW);
    MPUX = map(MPU_Data.X, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 100, 158);
    MPUY = map(MPU_Data.Y, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 3, 62);
    tft.fillCircle(MPUX, MPUY, 5, ST77XX_MAGENTA);
    if (oldMPUX != MPUX || oldMPUY != MPUY) {
        tft.fillCircle(oldMPUX, oldMPUY, 5, ST77XX_BLACK);
        tft.fillCircle(MPUX, MPUY, 5, ST77XX_MAGENTA);
        oldMPUX = MPUX;
        oldMPUY = MPUY;

    }
}
