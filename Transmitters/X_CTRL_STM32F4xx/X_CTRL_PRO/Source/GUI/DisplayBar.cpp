#include "FileGroup.h"
#include "DisplayPrivate.h"

lv_obj_t * barStatus;
lv_obj_t * barNavigation;

lv_obj_t * btnMenu;
lv_obj_t * btnHome;
lv_obj_t * btnBack;

static lv_obj_t *symBatt;

static void Task_UpdateStatusBar(lv_task_t * task)
{
    extern float BattVoltageOc;

    const char * batt[] =
    {
        LV_SYMBOL_BATTERY_EMPTY,
        LV_SYMBOL_BATTERY_1,
        LV_SYMBOL_BATTERY_2,
        LV_SYMBOL_BATTERY_3,
        LV_SYMBOL_BATTERY_FULL
    };
    int battUsage = map(BattVoltageOc, 2600, 4200, 0, __Sizeof(batt));
    lv_label_set_text(symBatt, batt[battUsage]);
}

static void CreatStatusBar()
{
    static lv_style_t styleStatusBar;
    lv_style_copy(&styleStatusBar, &lv_style_plain_color);
    styleStatusBar.body.main_color = LV_COLOR_BLACK;
    styleStatusBar.body.grad_color = LV_COLOR_BLACK;
    styleStatusBar.body.opa = LV_OPA_COVER;

    barStatus = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_width(barStatus, lv_disp_get_hor_res(NULL));
    lv_obj_align(barStatus, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style(barStatus, &styleStatusBar);

    symBatt = lv_label_create(barStatus, NULL);
    lv_label_set_text(symBatt, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_align(symBatt, NULL, LV_ALIGN_IN_RIGHT_MID, -LV_DPI / 10, 0);

    lv_cont_set_fit2(barStatus, LV_FIT_NONE, LV_FIT_TIGHT);   /*Let the height set automatically*/
    lv_obj_set_pos(barStatus, 0, 0);

    lv_task_create(Task_UpdateStatusBar, 500, LV_TASK_PRIO_MID, 0);
}

static void ButtonEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED)
    {
        //MotorVibrate(1, 50);
    }
    if(event == LV_EVENT_CLICKED)
    {
        if(obj == btnHome)
        {
            page.PageChangeTo(PAGE_Home);
        }
    }
    page.PageEventTransmit(event, obj);
}

static void CreatButtons(lv_obj_t** btn, const char *text, lv_align_t align)
{
    static lv_style_t btnStyle_Release, btnStyle_Press;
    lv_style_copy(&btnStyle_Release, &lv_style_plain_color);
    btnStyle_Release.body.main_color = LV_COLOR_BLACK;
    btnStyle_Release.body.grad_color = LV_COLOR_BLACK;
    btnStyle_Release.body.opa = LV_OPA_TRANSP;
    
    lv_style_copy(&btnStyle_Press, &lv_style_plain_color);
    btnStyle_Press.body.main_color = LV_COLOR_WHITE;
    btnStyle_Press.body.grad_color = LV_COLOR_WHITE;
    btnStyle_Press.body.opa = LV_OPA_50;
    
    *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(*btn, ButtonEvent_Handler);
    lv_obj_set_width(*btn, lv_disp_get_hor_res(NULL) / 3);
    lv_obj_align(*btn, barNavigation, align, 0, 0);
 
    lv_btn_set_style(*btn, LV_BTN_STYLE_REL, &btnStyle_Release);
    lv_btn_set_style(*btn, LV_BTN_STYLE_PR, &btnStyle_Press);
    //lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, &btnStyle);
    //lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, &btnStyle);

    lv_obj_t * sym = lv_label_create(*btn, NULL);
    lv_label_set_text(sym, text);

    lv_btn_set_ink_in_time(*btn, 200);//圆成长的时间
    //lv_btn_set_ink_wait_time(btn, 500);//最短时间保持完全覆盖（压下）状态
    lv_btn_set_ink_out_time(*btn, 200);//时间消失回到释放状态
}

static void CreatNavigationBar()
{
    static lv_style_t styleNavigationBar;
    lv_style_copy(&styleNavigationBar, &lv_style_plain_color);
    styleNavigationBar.body.main_color = LV_COLOR_BLACK;
    styleNavigationBar.body.grad_color = LV_COLOR_BLACK;
    styleNavigationBar.body.opa = LV_OPA_COVER;

    barNavigation = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_width(barNavigation, lv_disp_get_hor_res(NULL));
    lv_obj_align(barNavigation, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style(barNavigation, &styleNavigationBar);

    CreatButtons(&btnMenu, LV_SYMBOL_STOP, LV_ALIGN_IN_LEFT_MID);
    CreatButtons(&btnHome, LV_SYMBOL_HOME, LV_ALIGN_CENTER);
    CreatButtons(&btnBack, LV_SYMBOL_LEFT, LV_ALIGN_IN_RIGHT_MID);
}

void Init_Bar()
{
    CreatStatusBar();
    CreatNavigationBar();
}
