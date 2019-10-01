#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "SdFat.h"

static lv_obj_t * tabviewFm;
static lv_obj_t * tabInfo;
static lv_obj_t * tabFile;

static void Creat_PreloadLoading(lv_obj_t * parent, lv_obj_t** preload)
{
    /*Create a Preloader object*/
    *preload = lv_preload_create(parent, NULL);
    lv_obj_set_size(*preload, 50, 50);
    lv_obj_align(*preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_preload_set_type(*preload, LV_PRELOAD_TYPE_FILLSPIN_ARC);
}

static void Creat_Tabview(lv_obj_t** tabview)
{
    *tabview = lv_tabview_create(appWindow, NULL);
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(*tabview, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    tabInfo = lv_tabview_add_tab(*tabview, "Info");
    tabFile = lv_tabview_add_tab(*tabview, "File");
}

static void Creat_TabInfo(lv_obj_t * tab)
{
    #define TO_MB(size) (size*0.000512f)
    #define TO_GB(size) (TO_MB(size)/1024.0f)
    uint32_t cardSize = SD.card()->cardSize();
    uint32_t volFree = SD.vol()->freeClusterCount() * SD.vol()->blocksPerCluster();
    
    float totalSize = TO_GB(cardSize);
    float freeSize = TO_GB(volFree);
    float usageSize = totalSize - freeSize;
    
    /*LineMeter*/
    lv_obj_t * lmeter = lv_lmeter_create(tab, NULL);
    lv_obj_t * lmeterLabel = lv_label_create(lmeter, NULL);
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_lmeter_set_range(lmeter, 0, 100);  
    lv_lmeter_set_scale(lmeter, 240, 31);
    
    int16_t dispUseage = usageSize / totalSize * 100.0f;
    lv_lmeter_set_value(lmeter, dispUseage);
    lv_label_set_text_format(lmeterLabel, "%d%%", dispUseage);
    
    static lv_style_t labelStyle = *lv_obj_get_style(lmeterLabel);
    labelStyle.text.font = &lv_font_roboto_28;
    lv_label_set_style(lmeterLabel, LV_LABEL_STYLE_MAIN, &labelStyle);
    
    lv_obj_align(lmeterLabel, lmeter, LV_ALIGN_CENTER, 0, 0);
    
    /*Size*/
    lv_obj_t * labelSD = lv_label_create(lmeter, NULL);
    lv_label_set_text_format(labelSD, "%0.2fGB / %0.2fGB", usageSize, totalSize);
    lv_obj_align(labelSD, lmeter, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    Creat_Tabview(&tabviewFm);
    
    lv_obj_t * preLoading;
    Creat_PreloadLoading(tabviewFm, &preLoading);
    Creat_TabInfo(tabInfo);
    lv_obj_del_safe(&preLoading);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del_safe(&tabviewFm);
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
void PageRegister_FileExplorer(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
