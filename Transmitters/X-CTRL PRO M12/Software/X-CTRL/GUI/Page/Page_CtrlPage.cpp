#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"

static lv_obj_t * appWindow;

static lv_obj_t * tabview;
static lv_obj_t * tabGrp[3];
static lv_obj_t * ledGrp[3];

#ifndef TAB_JOYSTICK
/*摇杆*/
static lv_obj_t * JoysticksGrp[2];
static const uint8_t contJs_Size = 74;

/*三段开关*/
static lv_obj_t * imgSW[4];
static int16_t imgSW_MidPos;
static const uint8_t contSW_Height = 36;
static const uint8_t contSW_Width = 13;

static void Joystick_Creat(
    lv_obj_t** joystick,
    lv_obj_t * par,
    lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod
)
{
    lv_obj_t * js = lv_joystick_creat(par, NULL);
    
    lv_joystick_set_max_value(js, RCX_CHANNEL_DATA_MAX);
    lv_joystick_set_size(js, contJs_Size);

    /*摇杆容器*/
    static lv_style_t contFrame_style;
    contFrame_style = lv_style_transp;
    contFrame_style.body.radius = 5;
    lv_joystick_set_style(js, LV_JOYSTICK_STYLE_MAIN, &contFrame_style);
    lv_obj_align(js, par, align, x_mod, y_mod);

    /*摇杆移动底板*/
    static lv_style_t contjs_style;
    contjs_style = lv_style_plain;
    contjs_style.body.main_color = LV_COLOR_BLACK;
    contjs_style.body.grad_color = LV_COLOR_BLACK;
    contjs_style.body.opa = LV_OPA_30;
    lv_joystick_set_style(js, LV_JOYSTICK_STYLE_BASEPLATE, &contjs_style);

    /*底板十字架*/
    static lv_style_t style_line;
    style_line = lv_style_plain;
    style_line.line.color = LV_COLOR_GRAY;
    style_line.line.width = 1;
    lv_joystick_set_style(js, LV_JOYSTICK_STYLE_LINE, &style_line);

    /*贴图*/
    LV_IMG_DECLARE(IMG_JsFrame);
    LV_IMG_DECLARE(IMG_JsStick);
    
    /*摇杆*/
    lv_obj_t * imgJsStick = lv_img_create(lv_joystick_get_base_plate(js), NULL);
    lv_img_set_src(imgJsStick, &IMG_JsStick);
    lv_obj_align(imgJsStick, NULL, LV_ALIGN_CENTER, 0, 0);

    /*摇杆框*/
    lv_obj_t * imgJsFrame = lv_img_create(par, NULL);
    lv_img_set_src(imgJsFrame, &IMG_JsFrame);
    lv_obj_align(imgJsFrame, js, LV_ALIGN_CENTER, 0, 0);
    
    *joystick = js;
}

static void Joystick_Update()
{
    lv_joystick_set_value(JoysticksGrp[0], CTRL.JS_L.X.Val, CTRL.JS_L.Y.Val);
    lv_joystick_set_value(JoysticksGrp[1], CTRL.JS_R.X.Val, CTRL.JS_R.Y.Val);
}

static void Switch_Creat(
    uint8_t swIndex,
    const char* text,
    lv_obj_t * par,
    lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod
)
{
    lv_obj_t * cont = lv_cont_create(par, NULL);
    lv_obj_set_size(cont, contSW_Width, contSW_Height);
    lv_obj_align(cont, par, align, x_mod, y_mod);
    static lv_style_t style_cont;
    style_cont = lv_style_plain;
    style_cont.body.main_color = LV_COLOR_BLACK;
    style_cont.body.grad_color = LV_COLOR_BLACK;
    style_cont.body.opa = LV_OPA_10;
    style_cont.body.border.color = LV_COLOR_WHITE;
    style_cont.body.border.width = 1;
    style_cont.body.radius = 4;
    lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &style_cont);

    LV_IMG_DECLARE(IMG_Switch);
    lv_obj_t * img = lv_img_create(cont, NULL);
    lv_img_set_src(img, &IMG_Switch);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    imgSW_MidPos = lv_obj_get_y(img);
    imgSW[swIndex] = img;

    lv_obj_t * label = lv_label_create(par, NULL);
    lv_label_set_text(label, text);
    lv_obj_align(label, cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
}

static void Switch_Turn(uint8_t swIndex, uint8_t state)
{
    uint8_t y_pos[3] = {imgSW_MidPos - 10, imgSW_MidPos, imgSW_MidPos + 10};
    LV_OBJ_ADD_ANIM(imgSW[swIndex], y, y_pos[state % 3], 100);
}

static void Switch_Event(int event, void* sw)
{
    uint8_t state = 0;
    for(int i = 0; i < SW_IDX_MAX; i++)
    {
        if(sw == &swGrp[i])
        {
            if(event == SwitchEvent::EVENT_SwitchUp)
            {
                state = 0;
            }
            else if(event == SwitchEvent::EVENT_SwitchOff)
            {
                state = 1;
            }
            else if(event == SwitchEvent::EVENT_SwitchDown)
            {
                state = 2;
            }
            Switch_Turn(i, state);
        }
    }
}

static void LED_Creat(lv_obj_t * par)
{
    /*Create a style for the LED*/
    static lv_style_t style_led;
    lv_style_copy(&style_led, &lv_style_plain);
    style_led.body.radius = LV_RADIUS_CIRCLE;
    style_led.body.main_color = LV_COLOR_WHITE;
    style_led.body.grad_color = LV_COLOR_WHITE;

    ledGrp[0] = lv_led_create(par, NULL);
    lv_led_set_style(ledGrp[0], LV_LED_STYLE_MAIN, &style_led);
    lv_obj_align(ledGrp[0], NULL, LV_ALIGN_IN_BOTTOM_MID, -10, -5);
    lv_led_off(ledGrp[0]);

    ledGrp[1] = lv_led_create(par, ledGrp[0]);
    lv_obj_align(ledGrp[1], NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -5);
    lv_led_off(ledGrp[1]);

    ledGrp[2] = lv_led_create(par, ledGrp[0]);
    lv_obj_align(ledGrp[2], NULL, LV_ALIGN_IN_BOTTOM_MID, 10, -5);
    lv_led_off(ledGrp[2]);
}
#endif

#ifndef TAB_SLAVE_INFO
static lv_obj_t * labelSlave;
static uint8_t SlaveInfo_Mode = 0;

static void SlaveInfo_SwitchMode()
{
    SlaveInfo_Mode++;
    if(SlaveInfo_Mode > 1)
        SlaveInfo_Mode = 0;
}

static void SlaveInfo_Creat(lv_obj_t * par)
{
    lv_obj_t * label = lv_label_create(par, NULL);
    lv_label_set_text(label, "Not Connected");
    lv_obj_align(label, par, LV_ALIGN_IN_LEFT_MID, 20, 0);
    lv_obj_set_auto_realign(label, true);
    labelSlave = label;
    
    SlaveInfo_Mode = 0;
}

static void SlaveInfo_NormalUpdate()
{
    if(SlaveInfo_Mode == 0)
    {
        float battVoltage = RCX::RxGetPackChannel(0) / 1000.0f;
        uint8_t battlevel = map(RCX::RxGetPackChannel(1), 0, RCX_CHANNEL_DATA_MAX, 0, 100);
        lv_label_set_text_fmt(
            labelSlave, 
            "Type: 0x%x  RSSI: %d%%\n"
            "ID: 0x%x  Freq: %dMHz\n"
            "Battery: %0.1fV %d%%",
            RCX::TxGetObjectType(), nrf.RF_RSSI,
            RCX::TxGetObjectID(), nrf.RF_Freq + 2400,
            battVoltage, battlevel
        );
    }
    else if(SlaveInfo_Mode == 1)
    {
        lv_label_set_text_fmt(
            labelSlave,
            "Passback >>\n"
            "CH0 : %04d CH1  : %04d\n"
            "CH2 : %04d CH3 : %04d\n"
            "CH4 : %04d CH5 : %04d\n"
            "CH6 : %04d CH7 : %04d\n",
            RCX::RxGetPackChannel(0), RCX::RxGetPackChannel(1),
            RCX::RxGetPackChannel(2), RCX::RxGetPackChannel(3),
            RCX::RxGetPackChannel(4), RCX::RxGetPackChannel(5),
            RCX::RxGetPackChannel(6), RCX::RxGetPackChannel(7)
        );
    }
}

static void SlaveInfo_Update()
{
    if(RCX::RxGetConnected())
    {
        SlaveInfo_NormalUpdate();
    }
    else if(RCX::RxGetSignalLost())
    {
        #define ERROR_CODE_DEF(errcd) (errorCode & RCX::EC_##errcd ? #errcd"\n" : "")
        uint8_t errorCode = RCX::RxGetPackErrorCode();
        lv_label_set_text_fmt(
            labelSlave, 
            "ERROR: 0x%x\n%s%s%s%s",
            errorCode,
            ERROR_CODE_DEF(CONNECT_TIMEOUT),
            ERROR_CODE_DEF(TYPE_ERROR),
            ERROR_CODE_DEF(ID_ERROR),
            ERROR_CODE_DEF(CRC_ERROR)
        );
    }
}

#endif

#ifndef TAB_CHANNEL
static lv_obj_t * sliderGrp[8];
#define Slider_Height 16
#define Slider_Width  75

static void ChannelSlider_Creat(
    lv_obj_t** sliderCh, uint8_t chNum,
    lv_obj_t * par,
    lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod
)
{
    lv_obj_t * slider = lv_slider_create(par, NULL);
    lv_obj_set_size(slider, Slider_Width, Slider_Height);
    lv_obj_align(slider, par, align, x_mod, y_mod);
    lv_slider_set_range(slider, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);
    lv_slider_set_value(slider, 0, LV_ANIM_OFF);
    lv_slider_set_knob_in(slider, true);
    lv_slider_set_sym(slider, true);
    *sliderCh = slider;

    lv_obj_t * label = lv_label_create(par, NULL);
    lv_label_set_text_fmt(label, "CH%d", chNum);
    lv_obj_align(label, slider, LV_ALIGN_OUT_LEFT_MID, -3, 2);
}

static void ChannelSlider_Update()
{
    for(int i = 0; i < __Sizeof(sliderGrp); i++)
    {
        int16_t chVal = RCX::ChannelRead(i);
        lv_slider_set_value(sliderGrp[i], chVal, LV_ANIM_OFF);
    }
}
#endif

static void TabView_MoveTab(int8_t dir)
{
    uint8_t newTabIndex = lv_tabview_get_tab_act(tabview) + dir;
    __LimitValue(newTabIndex, 1, __Sizeof(tabGrp));

    lv_tabview_set_tab_act(tabview, newTabIndex, LV_ANIM_ON);

    for(int i = 0; i < __Sizeof(ledGrp); i++)
    {
        ((newTabIndex - 1) == i) ? lv_led_on(ledGrp[i]) : lv_led_off(ledGrp[i]);
    }
}

static void TabView_Creat()
{
    tabview = lv_tabview_create(appWindow, NULL);
    lv_tabview_set_btns_hidden(tabview, true);
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &lv_style_transp);
    lv_obj_align(tabview, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_tabview_add_tab(tabview, "");
    lv_tabview_set_tab_act(tabview, 0, LV_ANIM_OFF);
    __LoopExecute(tabGrp[i] = lv_tabview_add_tab(tabview, ""), __Sizeof(tabGrp));

    LED_Creat(tabview);
    TabView_MoveTab(0);
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);

    nrf.SetRF_Enable(true);
    RCX::ChannelReset();
    Com_SetEnable(true);

    TabView_Creat();
    Joystick_Creat(&JoysticksGrp[0], tabGrp[0], LV_ALIGN_IN_LEFT_MID, 10, 0);
    Joystick_Creat(&JoysticksGrp[1], tabGrp[0], LV_ALIGN_IN_RIGHT_MID, -10, 0);
    Switch_Creat(SW_IDX_G, "G", tabGrp[0], LV_ALIGN_CENTER, -(2 + 13 + 8), 0);
    Switch_Creat(SW_IDX_E, "E", tabGrp[0], LV_ALIGN_CENTER, -8, 0);
    Switch_Creat(SW_IDX_F, "F", tabGrp[0], LV_ALIGN_CENTER, 8, 0);
    Switch_Creat(SW_IDX_H, "H", tabGrp[0], LV_ALIGN_CENTER, 2 + 13 + 8, 0);
    
    SlaveInfo_Creat(tabGrp[1]);

    const int16_t ch_y_mod[4] =
    {
        -Slider_Height * 2,
        -Slider_Height * 0.7f,
        Slider_Height * 0.7f,
        Slider_Height * 2
    };
    for(int i = 0; i < __Sizeof(sliderGrp); i++)
    {
        ChannelSlider_Creat(&sliderGrp[i], i, tabGrp[2], LV_ALIGN_CENTER, (i % 2 ? 70 : -45), ch_y_mod[i / 2]);
    }
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    uint8_t actTab = lv_tabview_get_tab_act(tabview);
    switch(actTab)
    {
    case 1:
        Joystick_Update();
        break;
    case 2:
        __IntervalExecute(SlaveInfo_Update(), 200);
        break;
    case 3:
        ChannelSlider_Update();
        break;
    }
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_tabview_set_tab_act(tabview, 0, LV_ANIM_ON);
    Page_Delay(LV_TABVIEW_DEF_ANIM_TIME);
    lv_obj_clean(appWindow);

    Com_SetEnable(false);
    nrf.SetRF_Enable(false);
}

/**
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(void* btn, int event)
{
    if(event > SwitchEvent::EVENT_SwitchNone)
    {
        Switch_Event(event, btn);
        return;
    }

    if(btn == &btBACK)
    {
        if(event == ButtonEvent::EVENT_ButtonLongPressed)
        {
            Page_ReturnHome();
        }
    }

    if(event == ButtonEvent::EVENT_ButtonClick)
    {
        if(btn == &btUPL)
        {
            TabView_MoveTab(-1);
        }
        if(btn == &btDOWNL)
        {
            TabView_MoveTab(+1);
        }
    }
    if(event == ButtonEvent::EVENT_ButtonLongPressed)
    {
        if(btn == &btUPL)
        {
            SlaveInfo_SwitchMode();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_CtrlPage(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
