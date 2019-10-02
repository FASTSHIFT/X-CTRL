#include "DisplayPrivate.h"
#include "TasksManage.h"

#define BlueScreeDelayTime 10000

static void SoftDealy(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while(i--);
}

#include "Fonts\FreeMono24pt7b.h"
static void ShowCrashReports(const char* report)
{
    screen.fillScreen(screen.Blue);
    screen.setTextColor(screen.White);
    screen.setFont(&FreeMono24pt7b);
    screen.setTextSize(2);
    screen.setCursor(0, 80);
    screen.println(":(");

    screen.setFont();
    screen.setTextSize(2);

    screen.setCursor(0, ScreenMid_H - TEXT_HEIGHT_1 * 2);
    screen.println(report);
    screen.print("Syetem will reboot...");

    screen.setCursor(0, screen.height() - TEXT_HEIGHT_1 * 2 * 6 - 2);
    screen.println("Error code:");
    screen.printf("MMFAR=0x%x\r\n", SCB->MMFAR);
    screen.printf("BFAR=0x%x\r\n", SCB->BFAR);
    screen.printf("CFSR=0x%x\r\n", SCB->CFSR);
    screen.printf("HFSR=0x%x\r\n", SCB->HFSR);
    screen.printf("DFSR=0x%x\r\n", SCB->DFSR);
    
    SoftDealy(BlueScreeDelayTime);
    NVIC_SystemReset();
}


/***************************** Hooks *******************************/
extern "C"
{
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
    {
        char str[configMAX_TASK_NAME_LEN + 1];
        sprintf(str, "stack overflow\n < %s >", pcTaskName);
        ShowCrashReports(str);
    }
    
    void vApplicationMallocFailedHook()
    {
        ShowCrashReports("malloc failed");
    }
    
    void UsageFault_Handler()
    {
        ShowCrashReports("usage fault");
    }
    
    void BusFault_Handler()
    {
        ShowCrashReports("bus fault");
    }
    
    void MemManage_Handler()
    {
        ShowCrashReports("memory manage fault");
    }
    
    void HardFault_Handler()
    {
        ShowCrashReports("hard fault");
    }
}
