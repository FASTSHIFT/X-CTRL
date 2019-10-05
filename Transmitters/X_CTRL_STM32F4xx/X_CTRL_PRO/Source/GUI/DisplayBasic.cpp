#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "TasksManage.h"

TaskHandle_t TaskHandle_Display = 0;
TaskHandle_t TaskHandle_PageRun = 0;

SCREEN_CLASS screen(
    TFT_PORT_Pin, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

PageManager page(PAGE_MAX);

static void Init_Pages()
{
    PageRegister_Home(PAGE_Home);
    PageRegister_Settings(PAGE_Settings);
    PageRegister_BattInfo(PAGE_BattInfo);
    PageRegister_LuaScript(PAGE_LuaScript);
    PageRegister_SetDisplay(PAGE_SetDisplay);
    PageRegister_FileExplorer(PAGE_FileExplorer);
    PageRegister_SubAPPs(PAGE_SubAPPs);
    
    page.PagePush(PAGE_Home);
}

void Task_PageRun(void *pvParameters)
{
    for(;;)
    {
        page.Running();
        vTaskDelay(20);
    }
}

void Task_Dispaly(void *pvParameters)
{
    screen.begin();
    //screen.setRotation(0);
    screen.fillScreen(screen.Black);
    tp_dev.init();
    
    PWM_Init(TFT_LED_Pin, 1000, 20000);
    pwmWrite(TFT_LED_Pin, 500);
    
    lv_user_init();
    lv_user_fs_init();
    lv_theme_set_current(lv_theme_material_init(200, LV_FONT_DEFAULT));
    
    Init_Bar();
    Init_Pages();
    
    for(;;)
    {
        lv_task_handler();
        vTaskDelay(10);
    }
}
