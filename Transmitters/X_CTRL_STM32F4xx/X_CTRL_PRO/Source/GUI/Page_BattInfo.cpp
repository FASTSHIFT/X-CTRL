#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"

static void Task_BattUpdate(lv_task_t * task);
static void Task_GaugeUpdate(lv_task_t * task);

static lv_obj_t * chart;
static lv_chart_series_t * serCurrent;
static lv_chart_series_t * serVoltage;

static lv_obj_t * gaugeCurrent;
static lv_obj_t * gaugeVoltage;
static lv_obj_t * slider;
static lv_obj_t * taBattInfo;

static lv_task_t * task_1;
static lv_task_t * task_2;

static void taBattInfo_creat()
{
    taBattInfo = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_set_size(taBattInfo, 200, 50);
    lv_obj_align(taBattInfo, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_ta_set_cursor_type(taBattInfo, LV_CURSOR_NONE);
    lv_ta_set_text(taBattInfo, "");
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        analogWrite(TFT_LED_Pin, lv_slider_get_value(obj));
    }
}

void lv_ex_slider_1(void)
{
    /*Create styles*/
    static lv_style_t style_bg;
    static lv_style_t style_indic;
    static lv_style_t style_knob;
    lv_style_copy(&style_bg, &lv_style_pretty);
    style_bg.body.main_color =  LV_COLOR_BLACK;
    style_bg.body.grad_color =  LV_COLOR_GRAY;
    style_bg.body.radius = LV_RADIUS_CIRCLE;
    style_bg.body.border.color = LV_COLOR_WHITE;

    lv_style_copy(&style_indic, &lv_style_pretty_color);
    style_indic.body.radius = LV_RADIUS_CIRCLE;
    style_indic.body.shadow.width = 8;
    style_indic.body.shadow.color = style_indic.body.main_color;
    style_indic.body.padding.left = 3;
    style_indic.body.padding.right = 3;
    style_indic.body.padding.top = 3;
    style_indic.body.padding.bottom = 3;

    lv_style_copy(&style_knob, &lv_style_pretty);
    style_knob.body.radius = LV_RADIUS_CIRCLE;
    style_knob.body.opa = LV_OPA_70;
    style_knob.body.padding.top = 10 ;
    style_knob.body.padding.bottom = 10 ;

    /*Create a slider*/
    slider = lv_slider_create(lv_scr_act(), NULL);
    int val = timer_get_compare(PIN_MAP[TFT_LED_Pin].TIMx, PIN_MAP[TFT_LED_Pin].TimerChannel);
    
    lv_slider_set_style(slider, LV_SLIDER_STYLE_BG, &style_bg);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_INDIC, &style_indic);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_KNOB, &style_knob);
    lv_slider_set_range(slider, 10, 1000);
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    
    lv_obj_set_size(slider, 200, 20);
    lv_obj_align(slider, taBattInfo, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_event_cb(slider, event_handler);
}

static void lv_ex_gauge_1()
{
    /*Create a style*/
    static lv_style_t styleGauge;
    lv_style_copy(&styleGauge, &lv_style_pretty_color);
    styleGauge.body.main_color = lv_color_hex3(0x666);     /*Line color at the beginning*/
    styleGauge.body.grad_color =  lv_color_hex3(0x666);    /*Line color at the end*/
    styleGauge.body.padding.left = 10;                      /*Scale line length*/
    styleGauge.body.padding.inner = 2;                    /*Scale label padding*/
    styleGauge.body.border.color = lv_color_hex3(0x333);   /*Needle middle circle color*/
    //style.body.radius = 2;
    styleGauge.line.width = 3;
    styleGauge.text.color = lv_color_hex3(0x333);
    styleGauge.line.color = LV_COLOR_RED;                  /*Line color after the critical value*/

    /*Describe the color for the needles*/
    static lv_color_t needle_colors[] = {LV_COLOR_BLUE};

    /*Create a gauge*/
    gaugeCurrent = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_style(gaugeCurrent, LV_GAUGE_STYLE_MAIN, &styleGauge);
    lv_gauge_set_needle_count(gaugeCurrent, __Sizeof(needle_colors), needle_colors);
    lv_gauge_set_range(gaugeCurrent, 0, 30);
    lv_gauge_set_critical_value(gaugeCurrent, 24);
    lv_obj_set_size(gaugeCurrent, 150, 150);
    lv_obj_align(gaugeCurrent, barNavigation, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    /*Create a gauge*/
    gaugeVoltage = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_style(gaugeVoltage, LV_GAUGE_STYLE_MAIN, &styleGauge);
    lv_gauge_set_needle_count(gaugeVoltage, __Sizeof(needle_colors), needle_colors);
    lv_gauge_set_critical_value(gaugeVoltage, 39);
    lv_gauge_set_range(gaugeVoltage, 26, 42);
    lv_obj_set_size(gaugeVoltage, 150, 150);
    lv_obj_align(gaugeVoltage, barNavigation, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);
}

void lv_ex_chart_creat()
{
    /*Create a chart*/
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_chart_set_type(chart, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_series_opa(chart, LV_OPA_70);                            /*Opacity of the data series*/
    lv_chart_set_series_width(chart, 2);                                  /*Line width and point radious*/

    lv_chart_set_range(chart, 0, 100);
    lv_chart_set_point_count(chart, 20);

    //lv_chart_set_margin(chart, 10);
    //lv_chart_set_x_tick_texts(chart, "TIME\n", 1, LV_CHART_AXIS_DRAW_LAST_TICK);
    //lv_chart_set_y_tick_texts(chart, "1\n2\n3", 3, LV_CHART_AXIS_DRAW_LAST_TICK);

    /*Add two data series*/
    serCurrent = lv_chart_add_series(chart, LV_COLOR_RED);
    serVoltage = lv_chart_add_series(chart, LV_COLOR_BLUE);
}

static int cur, vol;

static void Task_GaugeUpdate(lv_task_t * task)
{
    static float curf, volf;
    __ValueCloseTo(curf, cur, 1);
    __ValueCloseTo(volf, vol, 1);

    lv_gauge_set_value(gaugeCurrent, 0, __Map(curf, 0, 100, 0, 30));
    lv_gauge_set_value(gaugeVoltage, 0, __Map(volf, 0, 100, 26, 42));
}

static void Task_BattUpdate(lv_task_t * task)
{
    extern float BattCurret, BattVoltage, BattVoltageOc;
    cur = __Map(ABS(BattCurret), 0, 3000, 0, 100);
    vol = __Map(BattVoltage, 2600, 4200, 0, 100);

    char info[50];
    sprintf(info, "Current:%0.2fmA\nVoltage:%0.2fmV ", BattCurret, BattVoltage);
    lv_ta_set_text(taBattInfo, info);
    /*
    screen.setCursor(0, 30);
    screen.setTextColor(screen.Black, screen.White);
    screen.printf("Current:%0.2fmA Voltage:%0.2fmV ", BattCurret, BattVoltage);
    */

    lv_chart_set_next(chart, serCurrent, cur);
    lv_chart_set_next(chart, serVoltage, vol);

    lv_chart_refresh(chart); /*Required after direct set*/
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    taBattInfo_creat();
    lv_ex_slider_1();
    lv_ex_chart_creat();
    lv_ex_gauge_1();

    task_1 = lv_task_create(Task_BattUpdate, 500, LV_TASK_PRIO_MID, 0);
    task_2 = lv_task_create(Task_GaugeUpdate, 50, LV_TASK_PRIO_MID, 0);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del(chart);
    lv_obj_del(gaugeCurrent);
    lv_obj_del(gaugeVoltage);
    lv_obj_del(slider);
    lv_obj_del(taBattInfo);
    lv_task_del(task_1);
    lv_task_del(task_2);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PageChangeTo(page.LastPage);
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_BattInfo(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
