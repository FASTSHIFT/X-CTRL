#include "DisplayPrivate.h"
#include "BSP/BSP.h"

SCREEN_CLASS screen(LCD_CS_Pin, LCD_DC_Pin, LCD_RST_Pin);

static void LVGL_Init()
{
    lv_port_disp_init();
    lv_theme_set_current(lv_theme_alien_init(100, NULL));
    
    lv_theme_t * theme = lv_theme_get_current();
    lv_color_t color = theme->style.sw.bg->body.border.color;
    
#define BTN_STYLE_SET_COLOR(btn_state, btn_color)\
do{\
    lv_style_t * style_btn = theme->style.btn.btn_state;\
    style_btn->body.border.color = btn_color;\
}while(0)

    BTN_STYLE_SET_COLOR(rel, color);
    BTN_STYLE_SET_COLOR(pr, color);
}

void Display_Init()
{
    DEBUG_FUNC_LOG();
    Backlight_SetValue(0);

    screen.begin();
    screen.setRotation(3);
    screen.fillScreen(screen.Black);
    
    DisplayError_Init();
    
    LVGL_Init();
    
    AppWindow_Init();
    Page_Init();
    StatusBar_Init();
    
    Backlight_SetGradual(1000, 1000);
}

void Display_Update()
{
    lv_task_handler();
    page.Running();
}

//static void DisplayFPSTest()
//{
//    float Ftime, Ltime;
//    float FPS;
//    Ftime = millis();
//    int i;
//    for(i = 100; i > 0; i--)
//    {
//        screen.fillScreen(screen.Black);
//        screen.fillScreen(screen.White);
//    }
//    Ltime = millis() - Ftime;
//    FPS = 100.0f / (Ltime / 1000.0f) * 2.0f;
//    screen.fillScreen(screen.Blue);
//    screen.setTextSize(1);
//    screen.setTextColor(screen.White);
//    screen.setCursor(0, 0);
//    screen.print(Ltime);
//    screen.setCursor(45, 0);
//    screen.print("ms");
//    screen.setCursor(0, 9);
//    screen.print("FPS:");
//    screen.setCursor(25, 9);
//    screen.print(FPS);
//    while(1);
//}
