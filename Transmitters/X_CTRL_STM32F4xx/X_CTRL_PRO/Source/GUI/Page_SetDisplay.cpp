#include "FileGroup.h"
#include "DisplayPrivate.h"

/****************** Bright Ctrl ****************/
static lv_obj_t * sliderBright;
static lv_obj_t * labelBright;

#define getBright() timer_get_compare(PIN_MAP[TFT_LED_Pin].TIMx, PIN_MAP[TFT_LED_Pin].TimerChannel)

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        int value = lv_slider_get_value(obj);
        lv_label_set_text_format(labelBright, "%d%%", value / 10);
        analogWrite(TFT_LED_Pin, value);
    }
}

static void Creat_Slider(lv_obj_t** slider)
{
    /*Create a slider*/
    *slider = lv_slider_create(appWindow, NULL);
    lv_slider_set_range(*slider, 5, 1000);
    lv_slider_set_value(*slider, getBright(), LV_ANIM_ON);
    
    lv_obj_set_size(*slider, APP_WIN_WIDTH - 30, 20);
    lv_obj_align(*slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(*slider, event_handler);
}

static void Creat_Label(lv_obj_t** label)
{
    *label = lv_label_create(appWindow, NULL);
    lv_label_set_text_format(*label, "%d%%", getBright() / 10);
    lv_obj_align(*label, sliderBright, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

/********************* Theme Ctrl ***********************/
//static lv_theme_t * th_act;

//static const char * th_options =
//{

//#if LV_USE_THEME_NIGHT
//        "Night"
//#endif

//#if LV_USE_THEME_MATERIAL
//        "\nMaterial"
//#endif

//#if LV_USE_THEME_ALIEN
//        "\nAlien"
//#endif

//#if LV_USE_THEME_ZEN
//        "\nZen"
//#endif

//#if LV_USE_THEME_NEMO
//        "\nNemo"
//#endif

//#if LV_USE_THEME_MONO
//        "\nMono"
//#endif

//#if LV_USE_THEME_DEFAULT
//        "\nDefault"
//#endif
//        ""
//};

//static lv_theme_t * themes[8];

//static void init_all_themes(uint16_t hue)
//{
//    /* NOTE: This must be adjusted if more themes are added. */
//    int i = 0;
//#if LV_USE_THEME_NIGHT
//    themes[i++] = lv_theme_night_init(hue, NULL);
//#endif

//#if LV_USE_THEME_MATERIAL
//    themes[i++] = lv_theme_material_init(hue, NULL);
//#endif

//#if LV_USE_THEME_ALIEN
//    themes[i++] = lv_theme_alien_init(hue, NULL);
//#endif

//#if LV_USE_THEME_ZEN
//    themes[i++] = lv_theme_zen_init(hue, NULL);
//#endif

//#if LV_USE_THEME_NEMO
//    themes[i++] = lv_theme_nemo_init(hue, NULL);
//#endif

//#if LV_USE_THEME_MONO
//    themes[i++] = lv_theme_mono_init(hue, NULL);
//#endif

//#if LV_USE_THEME_DEFAULT
//    themes[i++] = lv_theme_default_init(hue, NULL);
//#endif
//}

//static void hue_select_event_cb(lv_obj_t * roller, lv_event_t event)
//{

//    if(event == LV_EVENT_VALUE_CHANGED) {
//        uint16_t hue = lv_roller_get_selected(roller) * 30;

//        init_all_themes(hue);

//        lv_theme_set_current(th_act);
//    }
//}


/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    Creat_Slider(&sliderBright);
    Creat_Label(&labelBright);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del_safe(&sliderBright);
    lv_obj_del_safe(&labelBright);
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
void PageRegister_SetDisplay(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
