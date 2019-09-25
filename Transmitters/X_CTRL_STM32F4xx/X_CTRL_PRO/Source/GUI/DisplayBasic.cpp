#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "TasksManage.h"

SCREEN_CLASS screen(
    TFT_PORT_Pin, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

PageManager page(PAGE_MAX);

#include "demo.h"

void Task_Dispaly(void *pvParameters)
{
    screen.begin();
    //screen.setRotation(0);
    screen.fillScreen(screen.Black);
    tp_dev.init();
    
    PWM_Init(TFT_LED_Pin, 1000, 20000);
    pwmWrite(TFT_LED_Pin, 500);
    
    lv_user_init();
//  lv_user_fs_init();
    
    //PageCreat_LuaScript();
    //PageCreat_BattInfo();
    
//    demo_create();
    
    Init_Bar();
    
    PageRegister_Home(PAGE_Home);
    PageRegister_Settings(PAGE_Settings);
    PageRegister_BattInfo(PAGE_BattInfo);
    PageRegister_LuaScript(PAGE_LuaScript);
    
    page.PageChangeTo(PAGE_Home);
    
    for(;;)
    {
        lv_task_handler();
        page.Running();
        vTaskDelay(10);
    }
}

/*************************À¶ÆÁ¾¯¸æ*************************/
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
}


/***************************** Hooks *******************************/
extern "C"
{
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
    {
        char str[50];
        sprintf(str, "stack overflow\n: %s", pcTaskName);
        ShowCrashReports(str);
        SoftDealy(10000);
        NVIC_SystemReset();
    }
    
    void vApplicationMallocFailedHook()
    {
        ShowCrashReports("malloc failed");
        SoftDealy(10000);
        NVIC_SystemReset();
    }
    
    void HardFault_Handler()
    {
        ShowCrashReports("fatal error");
        SoftDealy(10000);
        NVIC_SystemReset();
    }
}
