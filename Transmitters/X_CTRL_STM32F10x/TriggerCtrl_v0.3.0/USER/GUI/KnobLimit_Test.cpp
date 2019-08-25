#include "FileGroup.h"

void KnobLimit_Test() {
    static int16_t KnobLimitL, KnobLimitR, oldKnobLimitL, oldKnobLimitR;
    tft.drawRect(63, 68, 7, 58, ST77XX_YELLOW);
    tft.drawRect(91, 68, 7, 58, ST77XX_YELLOW);

    KnobLimitL = 124-map(CTRL.KnobLimit.L, 0, CtrlOutput_MaxValue, 0, 55);
    KnobLimitR = 124-map(CTRL.KnobLimit.R, 0, CtrlOutput_MaxValue, 0, 55);

    tft.fillRect(64, KnobLimitL, 5, 1, ST77XX_GREEN);
    tft.fillRect(92, KnobLimitR, 5, 1, ST77XX_GREEN);

    if (oldKnobLimitL != KnobLimitL || oldKnobLimitR != KnobLimitR) {

        tft.fillRect(64, oldKnobLimitL, 5, 1, ST77XX_BLACK);
        tft.fillRect(92, oldKnobLimitR, 5, 1, ST77XX_BLACK);
        tft.fillRect(64, KnobLimitL, 5, 1, ST77XX_GREEN);
        tft.fillRect(92, KnobLimitR, 5, 1, ST77XX_GREEN);

        oldKnobLimitL = KnobLimitL;
        oldKnobLimitR = KnobLimitR;


    }

}
