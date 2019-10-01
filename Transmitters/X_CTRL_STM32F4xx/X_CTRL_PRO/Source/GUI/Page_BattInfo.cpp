#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"
#include "Module.h"

static void Task_BattUpdate(lv_task_t * task);
static void Task_GaugeUpdate(lv_task_t * task);

static lv_obj_t * chart;
static lv_chart_series_t * serCurrent;
static lv_chart_series_t * serVoltage;

static lv_obj_t * gaugeCurrent;
static lv_obj_t * gaugeVoltage;
static lv_obj_t * labelCurrent;
static lv_obj_t * labelVoltage;

static lv_obj_t * labelStatus;

static lv_task_t * task_1;
static lv_task_t * task_2;

static void Creat_Label(lv_obj_t * parent, lv_obj_t** label)
{
    *label = lv_label_create(parent, NULL);
    lv_label_set_text(*label, "");
    lv_obj_align(*label, parent, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    lv_obj_set_auto_realign(*label, true);
}

static void Creat_Gauge()
{
    /*Describe the color for the needles*/
    static lv_color_t needle_colors[] = {LV_COLOR_BLUE};

    /*Create a gauge*/
    gaugeCurrent = lv_gauge_create(appWindow, NULL);
    lv_gauge_set_needle_count(gaugeCurrent, __Sizeof(needle_colors), needle_colors);
    lv_gauge_set_range(gaugeCurrent, 0, 30);
    lv_gauge_set_critical_value(gaugeCurrent, 24);
    lv_obj_set_size(gaugeCurrent, 150, 150);
    lv_obj_align(gaugeCurrent, barNavigation, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    /*Create a gauge*/
    gaugeVoltage = lv_gauge_create(appWindow, NULL);
    lv_gauge_set_needle_count(gaugeVoltage, __Sizeof(needle_colors), needle_colors);
    lv_gauge_set_critical_value(gaugeVoltage, 39);
    lv_gauge_set_range(gaugeVoltage, 26, 42);
    lv_obj_set_size(gaugeVoltage, 150, 150);
    lv_obj_align(gaugeVoltage, barNavigation, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);
}

void Creat_Chart(lv_obj_t** chart)
{
    /*Create a chart*/
    *chart = lv_chart_create(appWindow, NULL);
    lv_obj_set_size(*chart, 200, 150);
    lv_obj_align(*chart, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_chart_set_type(*chart, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_series_opa(*chart, LV_OPA_70);                            /*Opacity of the data series*/
    lv_chart_set_series_width(*chart, 2);                                  /*Line width and point radious*/

    lv_chart_set_range(*chart, 0, 100);
    lv_chart_set_point_count(*chart, 20);

    /*Add two data series*/
    serCurrent = lv_chart_add_series(*chart, LV_COLOR_RED);
    serVoltage = lv_chart_add_series(*chart, LV_COLOR_BLUE);
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
    cur = __Map(ABS(BattCurret), 0, 3000, 0, 100);
    vol = __Map(BattVoltage, 2600, 4200, 0, 100);
    
    lv_chart_set_next(chart, serCurrent, cur);
    lv_chart_set_next(chart, serVoltage, vol);

    lv_chart_refresh(chart); /*Required after direct set*/
    
    lv_label_set_text_format(labelCurrent, "%0.2fmA", BattCurret);
//    lv_obj_align(labelCurrent, gaugeCurrent, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    
    lv_label_set_text_format(labelVoltage, "%0.2fmV", BattVoltage);
//    lv_obj_align(labelVoltage, gaugeVoltage, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    
    float power = BattCurret * BattVoltage / 1000000.0f;
    if(BattCurret < 0.0f)
    {
        lv_label_set_text_format(labelStatus, "Discharge: %0.2fW", power);
    }
    else
    {
        lv_label_set_text_format(labelStatus, "Charge: %0.2fW", power);
    }
    lv_obj_set_parent(labelStatus, appWindow);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    Creat_Chart(&chart);
    Creat_Label(chart, &labelStatus);
    lv_obj_set_parent(labelStatus, appWindow);
    lv_obj_align(labelStatus, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    
    Creat_Gauge();
    Creat_Label(gaugeCurrent, &labelCurrent);
    Creat_Label(gaugeVoltage, &labelVoltage);

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
    lv_obj_del_safe(&chart);
    lv_obj_del_safe(&labelStatus);
    lv_obj_del_safe(&gaugeCurrent);
    lv_obj_del_safe(&gaugeVoltage);
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
