#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"

static lv_obj_t * appWindow;

static lv_obj_t * labelCurve;

static lv_obj_t * chartCurve;
static lv_chart_series_t * serCurve;
static lv_chart_series_t * serPosX;
static lv_chart_series_t * serPosY;
static lv_task_t * taskCurveUpdate;

static XC_Joystick_TypeDef* JoystickCfg = NULL;

void Joystick_CurveCfg(XC_Joystick_TypeDef* js)
{
    JoystickCfg = js;
}

static void ChartCurve_Update(lv_task_t * task)
{
    if(JoystickCfg == NULL)
        return;
    
    uint16_t point_cnt = lv_chart_get_point_cnt(chartCurve);
    
    lv_chart_init_points(chartCurve, serPosX, 0);
    int16_t pos_x = map(JoystickCfg->X.AdcVal, JoystickCfg->X.Min, JoystickCfg->X.Max, 0, point_cnt - 1);
    serPosX->points[pos_x] = JoystickCfg->X.Val;
    
    lv_chart_init_points(chartCurve, serPosY, 0);
    int16_t pos_y = map(JoystickCfg->Y.AdcVal, JoystickCfg->Y.Min, JoystickCfg->Y.Max, 0, point_cnt - 1);
    serPosY->points[pos_y] = JoystickCfg->Y.Val;
    
    lv_chart_refresh(chartCurve);
    
    static uint8_t cnt = 0;
    if(cnt > 10)
    {
        lv_label_set_text_fmt(
            labelCurve,
            "#0000FF X:%04d# #FF0000 Y:%04d#\n"
            "start: %0.1f end: %0.1f",
            JoystickCfg->X.Val, JoystickCfg->Y.Val,
            JoystickCfg->X.Curve.Start, JoystickCfg->X.Curve.End
        );
        cnt = 0;
    }
    cnt++;
}

static void LabelCurve_Creat()
{
    labelCurve = lv_label_create(appWindow, NULL);
    lv_label_set_recolor(labelCurve, true);
    lv_label_set_text(labelCurve, "---");
    lv_obj_align(labelCurve, chartCurve, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_auto_realign(labelCurve, true);
}

static void ChartCurve_Creat()
{
    lv_obj_t * chart = lv_chart_create(appWindow, NULL);
    lv_coord_t size = APP_WIN_HEIGHT - 30;
    lv_obj_set_size(chart, size, size);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_series_opa(chart, LV_OPA_COVER);
    lv_chart_set_series_width(chart, 1);
    lv_chart_set_div_line_count(chart, 3, 3);

    lv_chart_set_point_count(chart, size);
    lv_chart_set_range(chart, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);

    /*Add two data series*/
    serCurve = lv_chart_add_series(chart, LV_COLOR_YELLOW);
    serPosX = lv_chart_add_series(chart, LV_COLOR_BLUE);
    serPosY = lv_chart_add_series(chart, LV_COLOR_RED);
    
    chartCurve = chart;

    taskCurveUpdate = lv_task_create(ChartCurve_Update, 20, LV_TASK_PRIO_MID, 0);
    
    if(JoystickCfg == NULL)
        return;
    
    LabelCurve_Creat();
    
    Joystick_GetCurve(JoystickCfg, serCurve->points, lv_chart_get_point_cnt(chartCurve));
    ChartCurve_Update(taskCurveUpdate);
}



/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
    
    ChartCurve_Creat();
}

static void Loop()
{
    if(JoystickCfg == NULL)
    {
        page.PagePop();
    }
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_task_del(taskCurveUpdate);
    lv_obj_clean(appWindow);
    JoystickCfg = NULL;
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* btn)
{
    if(btn == &btBACK)
    {
        if(event == ButtonEvent::EVENT_ButtonPress)
        {
            page.PagePop();
        }
    }
    
    if(btn == &btOK)
    {
        if(event == ButtonEvent::EVENT_ButtonLongPressed)
        {
            Joystick_SetCurve(JoystickCfg, 5.0f, 5.0f);
            Joystick_GetCurve(JoystickCfg, serCurve->points, lv_chart_get_point_cnt(chartCurve));
        }
    }

    if(event == ButtonEvent::EVENT_ButtonPress || event == ButtonEvent::EVENT_ButtonLongPressRepeat)
    {
        float startK = JoystickCfg->X.Curve.Start;
        float endK = JoystickCfg->X.Curve.End;
        if(btn == &btUPL)
        {
            startK += 0.1f;
        }
        if(btn == &btDOWNL)
        {
            startK -= 0.1f;
        }
        if(btn == &btUP)
        {
            endK += 0.1f;
        }
        if(btn == &btDOWN)
        {
            endK -= 0.1f;
        }
        Joystick_SetCurve(JoystickCfg, startK, endK);
        Joystick_GetCurve(JoystickCfg, serCurve->points, lv_chart_get_point_cnt(chartCurve));
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_JoystickCurveCfg(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
