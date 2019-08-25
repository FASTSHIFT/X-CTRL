#include "FileGroup.h"

//*****************MAIN******************//
int main(void)
{
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
//  ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
