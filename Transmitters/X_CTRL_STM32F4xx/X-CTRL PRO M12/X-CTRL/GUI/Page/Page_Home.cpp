#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"

static lv_obj_t * appWindow;

static lv_obj_t * contDisp;
static lv_obj_t * contICON;
static lv_obj_t * contSel;

static lv_obj_t * labelTitle;
static lv_obj_t * contTitle;

LV_IMG_DECLARE(IMG_RemoteCtrl);
LV_IMG_DECLARE(IMG_Channel);
LV_IMG_DECLARE(IMG_Model);
LV_IMG_DECLARE(IMG_RadioCfg);
LV_IMG_DECLARE(IMG_Scan);
LV_IMG_DECLARE(IMG_Disnet);
LV_IMG_DECLARE(IMG_Bluetooth2);
LV_IMG_DECLARE(IMG_StopWatch);
LV_IMG_DECLARE(IMG_Gyroscope);
LV_IMG_DECLARE(IMG_Stopwatch2);
LV_IMG_DECLARE(IMG_Game1);
LV_IMG_DECLARE(IMG_Settings);

typedef struct{
    const void * src_img;
    const char * text;
    lv_obj_t * img;
    uint8_t pageID;
}ICON_TypeDef;

static ICON_TypeDef ICON_Grp[] = 
{
    {.src_img = &IMG_RemoteCtrl, .text = "Remote",  .pageID = PAGE_Handshake},
    {.src_img = &IMG_Channel,    .text = "Channel", .pageID = PAGE_ChannelCfg},
    {.src_img = &IMG_Model,      .text = "Model",},
    {.src_img = &IMG_RadioCfg,   .text = "Radio",   .pageID = PAGE_RadioCfg},
    {.src_img = &IMG_Scan,       .text = "Scanner", .pageID = PAGE_Scanner},
    {.src_img = &IMG_Disnet,     .text = "Radio Attack",},
    {.src_img = &IMG_Bluetooth2, .text = "Bluetooth",.pageID = PAGE_BluetoothCfg},
    {.src_img = &IMG_Gyroscope,  .text = "Gyroscope",.pageID = PAGE_GyroscopeCfg},
    {.src_img = &IMG_Stopwatch2, .text = "Stopwatch",},
    {.src_img = &IMG_Game1,      .text = "Game",},
    {.src_img = &IMG_Settings,   .text = "Settings", .pageID = PAGE_MiscCfg},
};

static const uint8_t ICON_IntervalPixel = 20;
static const uint8_t ICON_Size = 60;
static int8_t ICON_NowSelIndex = 0;

#define ICON_MAX_INDEX (__Sizeof(ICON_Grp) - 1)

static void ICON_Grp_Creat(lv_obj_t * par)
{
    contDisp = lv_cont_create(par, NULL);
    lv_cont_set_style(contDisp, LV_CONT_STYLE_MAIN, &lv_style_transp);
    lv_obj_set_size(contDisp, AppWindow_GetWidth(), APP_WIN_HEIGHT - lv_obj_get_height(contTitle));
    lv_obj_align(contDisp, par, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    contICON = lv_cont_create(contDisp, NULL);
    lv_cont_set_style(contICON, LV_CONT_STYLE_MAIN, &lv_style_transp);
    lv_obj_set_size(contICON, (ICON_Size + ICON_IntervalPixel) * __Sizeof(ICON_Grp), lv_obj_get_height(contDisp));
    lv_obj_set_x(contICON, lv_obj_get_width(contDisp));
    
    contSel = lv_cont_create(contDisp, NULL);
    lv_obj_set_size(contSel, ICON_Size + 10, ICON_Size + 10);
    lv_obj_align(contSel, contDisp, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_opa_scale_enable(contSel, true);
    lv_obj_set_opa_scale(contSel, LV_OPA_TRANSP);
    static lv_style_t style_cont;
    style_cont = *lv_cont_get_style(contSel, LV_CONT_STYLE_MAIN);
    style_cont.body.opa = LV_OPA_0;
    style_cont.body.border.opa = LV_OPA_COVER;
    style_cont.body.border.width = 2;
    style_cont.body.border.color = LV_COLOR_ARMY_GREEN;
    style_cont.body.radius = 5;
    lv_cont_set_style(contSel, LV_CONT_STYLE_MAIN, &style_cont);
    
    for(int i = 0; i < __Sizeof(ICON_Grp); i++)
    {
        lv_obj_t * img = lv_img_create(contICON, NULL);
        lv_img_set_src(img, ICON_Grp[i].src_img);
        lv_obj_align(img, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
        
        int x_offset = (ICON_Size - lv_obj_get_width(img)) / 2 + 9;
        
        lv_obj_set_x(img, (ICON_Size + ICON_IntervalPixel) * i + x_offset);
        
        ICON_Grp[i].img = img;
    }
    
    LV_OBJ_ADD_ANIM(contSel, opa_scale, LV_OPA_COVER, LV_ANIM_TIME_DEFAULT);
}

static void Title_Creat(lv_obj_t * par)
{
    contTitle = lv_cont_create(par, NULL);
    static lv_style_t style_cont;
    style_cont = lv_style_plain;
    style_cont.body.main_color = LV_COLOR_BLACK;
    style_cont.body.grad_color = LV_COLOR_BLACK;
    style_cont.body.opa = LV_OPA_40;
    lv_cont_set_style(contTitle, LV_CONT_STYLE_MAIN, &style_cont);
    lv_obj_set_size(contTitle, AppWindow_GetWidth(), StatusBar_GetHeight());
    lv_obj_align(contTitle, par, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    int targey_y = lv_obj_get_y(contTitle);
    lv_obj_set_y(contTitle, targey_y + lv_obj_get_height(contTitle));
    
    labelTitle = lv_label_create(contTitle, NULL);
    LV_LABEL_SET_FONT(labelTitle, HandGotn_14, LV_COLOR_WHITE);
    lv_label_set_static_text(labelTitle, ICON_Grp[0].text);
    lv_obj_align(labelTitle, NULL, LV_ALIGN_CENTER, 0, 2);
    lv_obj_set_auto_realign(labelTitle, true);
    
    LV_OBJ_ADD_ANIM(contTitle, y, targey_y, LV_ANIM_TIME_DEFAULT);
}

static void ICON_Grp_MoveFouce(uint8_t iconIndex)
{
    if(iconIndex > ICON_MAX_INDEX)
        return;
    
    lv_label_set_static_text(labelTitle, ICON_Grp[iconIndex].text);
    
    int16_t target_x = -(ICON_Size + ICON_IntervalPixel) * (iconIndex - 1);

    LV_OBJ_ADD_ANIM(contICON, x, target_x, LV_ANIM_TIME_DEFAULT);
}

static void ICON_Grp_Move(int8_t dir)
{
    __ValuePlus(ICON_NowSelIndex, dir, 0, ICON_MAX_INDEX);
    ICON_Grp_MoveFouce(ICON_NowSelIndex);
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
    Title_Creat(appWindow);
    ICON_Grp_Creat(appWindow);
    ICON_Grp_MoveFouce(ICON_NowSelIndex);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    LV_OBJ_ADD_ANIM(contICON, x, lv_obj_get_width(contDisp) + ICON_Size, LV_ANIM_TIME_DEFAULT);
    LV_OBJ_ADD_ANIM(contTitle, y, lv_obj_get_y(contTitle) + lv_obj_get_height(contTitle), LV_ANIM_TIME_DEFAULT);
    LV_OBJ_ADD_ANIM(contSel, opa_scale, LV_OPA_TRANSP, LV_ANIM_TIME_DEFAULT);
    PageDelay(LV_ANIM_TIME_DEFAULT);
    lv_obj_clean(appWindow);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* btn)
{
    if(btn == &btOK)
    {
        if(event == ButtonEvent_Type::EVENT_ButtonLongPressed)
        {
            page.PagePop();
        }
        else if(event == ButtonEvent_Type::EVENT_ButtonClick)
        {
            uint8_t pageID = ICON_Grp[ICON_NowSelIndex].pageID;
            if(pageID != PAGE_NONE)
            {
                page.PagePush(pageID);
            }
        }
    }
    
    if(event == ButtonEvent_Type::EVENT_ButtonPress || event == ButtonEvent_Type::EVENT_ButtonLongPressRepeat)
    {
        if(btn == &btUP || btn == &btUPL)
        {
            ICON_Grp_Move(-1);
        }
        if(btn == &btDOWN || btn == &btDOWNL)
        {
            ICON_Grp_Move(+1);
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Home(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
