#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"
#include "TasksManage.h"

lv_obj_t * barStatus;
lv_obj_t * barNavigation;
lv_obj_t * appWindow;

lv_obj_t * btnMenu;
lv_obj_t * btnHome;
lv_obj_t * btnBack;

static lv_obj_t * symBatt;
static lv_obj_t * labelBattUsage;
static lv_obj_t * labelCPUusage;
static lv_obj_t * contDropDown;
#define IS_DropDownShow (lv_obj_get_y(contDropDown)+lv_obj_get_height(contDropDown)>= 0)
static void DropDownList_AnimDown(bool down);

bool Is_BattCharging = false;

static void Task_UpdateStatusBar(lv_task_t * task)
{
    /*电池电量显示*/
    extern float BattVoltageOc, BattCurret; 
    Is_BattCharging = BattCurret > 0 ? true : false;

    const char * battSymbol[] =
    {
        LV_SYMBOL_BATTERY_EMPTY,
        LV_SYMBOL_BATTERY_1,
        LV_SYMBOL_BATTERY_2,
        LV_SYMBOL_BATTERY_3,
        LV_SYMBOL_BATTERY_FULL
    };
    int symIndex;
    if(Is_BattCharging)
    {
        static uint8_t usage = 0;
        usage++;
        usage %= map(BattVoltageOc, 2600, 4200, 0, __Sizeof(battSymbol)) + 1;
        symIndex = usage;
    }
    else
    {
        symIndex = map(BattVoltageOc, 2600, 4200, 0, __Sizeof(battSymbol));
    }
    __LimitValue(symIndex, 0, __Sizeof(battSymbol));
    lv_label_set_text(symBatt, battSymbol[symIndex]);
    lv_label_set_text_format(labelBattUsage, "%d%", map(BattVoltageOc, 2600, 4200, 0, 100));
    
    /*CPU占用显示*/
    lv_label_set_text_format(labelCPUusage, "%d%%", FreeRTOS_GetCPUUsage());
}

static void StatusBarEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED)
    {
        lv_obj_set_y(contDropDown, -lv_obj_get_height(contDropDown) + lv_obj_get_height(obj) * 3);
        lv_obj_set_top(contDropDown, true);
    }
}

static void Creat_StatusBar()
{
    barStatus = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_width(barStatus, lv_disp_get_hor_res(NULL));
    lv_obj_align(barStatus, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    
    static lv_style_t styleStatusBar;
    lv_style_copy(&styleStatusBar, &lv_style_plain_color);
    styleStatusBar.body.main_color = LV_COLOR_BLACK;
    styleStatusBar.body.grad_color = LV_COLOR_BLACK;
    styleStatusBar.body.opa = LV_OPA_COVER;
    lv_obj_set_style(barStatus, &styleStatusBar);

    symBatt = lv_label_create(barStatus, NULL);
    lv_label_set_text(symBatt, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_align(symBatt, NULL, LV_ALIGN_IN_RIGHT_MID, -LV_DPI / 10, 0);
    
    labelBattUsage = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelBattUsage,"--");
    lv_obj_align(labelBattUsage, symBatt, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_set_auto_realign(labelBattUsage, true);
    
    lv_obj_t * symLoop = lv_label_create(barStatus, NULL);
    lv_label_set_text(symLoop, LV_SYMBOL_LOOP);
    lv_obj_align(symLoop, symBatt, LV_ALIGN_OUT_LEFT_MID, -65, 0);
    
    labelCPUusage = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelCPUusage,"--%");
    lv_obj_align(labelCPUusage, symLoop, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    
    lv_cont_set_fit2(barStatus, LV_FIT_NONE, LV_FIT_TIGHT);   /*Let the height set automatically*/
    lv_obj_set_pos(barStatus, 0, 0);
    lv_obj_set_event_cb(barStatus, StatusBarEvent_Handler);

    lv_task_create(Task_UpdateStatusBar, 500, LV_TASK_PRIO_MID, 0);
}

static void NaviButtonEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED)
    {
        Motor_Vibrate(1, 50);
    }
    if(event == LV_EVENT_CLICKED)
    {
        if(obj == btnHome)
        {
            page.PageStackClear();
            page.PagePush(PAGE_Home);
        }
        if(IS_DropDownShow)
        {
            DropDownList_AnimDown(false);
            return;
        }
    }
    page.PageEventTransmit(event, obj);
}

static void Creat_Buttons(lv_obj_t** btn, const char *text, lv_align_t align)
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
    lv_obj_set_event_cb(*btn, NaviButtonEvent_Handler);
    lv_obj_set_width(*btn, lv_disp_get_hor_res(NULL) / 3);
    lv_obj_align(*btn, barNavigation, align, 0, 0);
 
    lv_btn_set_style(*btn, LV_BTN_STYLE_REL, &btnStyle_Release);
    lv_btn_set_style(*btn, LV_BTN_STYLE_PR, &btnStyle_Press);

    lv_obj_t * sym = lv_label_create(*btn, NULL);
    lv_label_set_text(sym, text);

    lv_btn_set_ink_in_time(*btn, 200);
    lv_btn_set_ink_out_time(*btn, 200);
}

static void Creat_NavigationBar()
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

    Creat_Buttons(&btnMenu, LV_SYMBOL_STOP, LV_ALIGN_IN_LEFT_MID);
    Creat_Buttons(&btnHome, LV_SYMBOL_HOME, LV_ALIGN_CENTER);
    Creat_Buttons(&btnBack, LV_SYMBOL_LEFT, LV_ALIGN_IN_RIGHT_MID);
}

static void Creat_AppWindow()
{
#define PAGE_HIEGHT (lv_disp_get_ver_res(NULL) - lv_obj_get_height(barStatus) - lv_obj_get_height(barNavigation))
#define PAGE_WIDTH  (lv_disp_get_hor_res(NULL))
    appWindow = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(appWindow, PAGE_WIDTH, PAGE_HIEGHT);
    lv_obj_align(appWindow, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_cont_set_fit(appWindow, LV_FIT_NONE);
    lv_cont_set_style(appWindow, LV_CONT_STYLE_MAIN, &lv_style_transp);
}

static void DropDownList_AnimDown(bool down)
{
    static lv_anim_t a;
    a.var = contDropDown;
    a.start = lv_obj_get_y(contDropDown);
    
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_linear;
//    a.ready_cb = DropDownList_AnimEnd;
    a.time = 300;
    if(down)
    {
        a.end = 0;
    }
    else
    {
        a.end = -lv_obj_get_height(contDropDown) - 5;
    }
    
    lv_anim_create(&a);
}

static void DropDownListEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED || event == LV_EVENT_DRAG_END)
    {
        lv_coord_t ver = lv_disp_get_ver_res(NULL);
        lv_coord_t y = lv_obj_get_y(contDropDown) + lv_obj_get_height(contDropDown);
        if(y > ver / 2)
        {
            DropDownList_AnimDown(true);
        }
        else
        {
            DropDownList_AnimDown(false);
        }
    }
}

static void Creat_DropDownList()
{
    contDropDown = lv_cont_create(lv_scr_act(), NULL);
    
    static lv_style_t style = *lv_obj_get_style(contDropDown);
    style.body.main_color = LV_COLOR_BLACK;
    style.body.grad_color = LV_COLOR_BLACK;
    style.body.opa = LV_OPA_70;
    lv_cont_set_style(contDropDown, LV_CONT_STYLE_MAIN, &style);
    
    lv_obj_set_size(contDropDown, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL) - lv_obj_get_height(barNavigation));
    lv_obj_set_drag_dir(contDropDown, LV_DRAG_DIR_VER);
    lv_obj_set_drag(contDropDown, true);
//    lv_obj_set_drag_throw(contDropDown, true);
    lv_obj_set_event_cb(contDropDown, DropDownListEvent_Handler);
    lv_obj_align(contDropDown, barStatus, LV_ALIGN_OUT_TOP_MID, 0, -5);
    
    /*label*/
    lv_obj_t * label = lv_label_create(contDropDown, NULL);
    lv_label_set_text(label, LV_SYMBOL_DOWN);
    lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    
//    lv_obj_t * label2 = lv_label_create(contDropDown, NULL);
//    lv_label_set_text(label2, "X-CTRL PRO");
//    lv_obj_align(label2, label, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

void Init_Bar()
{
    Creat_StatusBar();
    Creat_NavigationBar();
    Creat_AppWindow();
    Creat_DropDownList();
}
