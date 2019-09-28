#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "TasksManage.h"

SCREEN_CLASS screen(
    TFT_PORT_Pin, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

PageManager page(PAGE_MAX);

void Task_Dispaly(void *pvParameters)
{
    screen.begin();
    //screen.setRotation(0);
    screen.fillScreen(screen.Black);
    tp_dev.init();
    
    PWM_Init(TFT_LED_Pin, 1000, 20000);
    pwmWrite(TFT_LED_Pin, 500);
    
    lv_user_init();
//    lv_user_fs_init();
    
    //PageCreat_LuaScript();
    //PageCreat_BattInfo();
    
//    demo_create();
    
    Init_Bar();
    
    PageRegister_Home(PAGE_Home);
    PageRegister_Settings(PAGE_Settings);
    PageRegister_BattInfo(PAGE_BattInfo);
    PageRegister_LuaScript(PAGE_LuaScript);
    PageRegister_SetDisplay(PAGE_SetDisplay);
    
    page.PageChangeTo(PAGE_Home);
    
    for(;;)
    {
        lv_task_handler();
        page.Running();
        vTaskDelay(10);
    }
}
