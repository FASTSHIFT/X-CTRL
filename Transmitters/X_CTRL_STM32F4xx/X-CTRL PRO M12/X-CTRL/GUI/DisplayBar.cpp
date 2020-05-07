#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "BSP/BSP.h"
#include "Communication/Comprivate.h"

#define StatusBar_Height 20

/*状态栏*/
static lv_obj_t * barStatus;

/*信号指示*/
static lv_obj_t * imgRSSI;
LV_IMG_DECLARE(IMG_Sign0);
LV_IMG_DECLARE(IMG_Sign1);
LV_IMG_DECLARE(IMG_Sign2);
LV_IMG_DECLARE(IMG_Sign3);
static const void * IMG_SignGrp[] = {&IMG_Sign0, &IMG_Sign1, &IMG_Sign2, &IMG_Sign3};

static lv_obj_t * imgUpDownload;
LV_IMG_DECLARE(IMG_Upload);
LV_IMG_DECLARE(IMG_Download);
LV_IMG_DECLARE(IMG_NoUpDownload);

/*蓝牙*/
static lv_obj_t * imgBluetooth;
LV_IMG_DECLARE(IMG_Bluetooth);

/*电池*/
static lv_obj_t * imgBatt;
static lv_obj_t * contBatt;
static lv_obj_t * labelBatt;
LV_IMG_DECLARE(IMG_Battery);
static const uint8_t contBatt_Width = 11;

/***************信号状态****************/
static void SignalStatus_Creat()
{
    /*信号强度图标*/
    imgRSSI = lv_img_create(barStatus, NULL);
    lv_img_set_src(imgRSSI, IMG_SignGrp[0]);
    lv_obj_align(imgRSSI, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    
    /*上传下载图标*/
    imgUpDownload = lv_img_create(barStatus, NULL);
    lv_img_set_src(imgUpDownload, &IMG_NoUpDownload);
    lv_obj_align(imgUpDownload, barStatus, LV_ALIGN_IN_LEFT_MID, 17, 0);
}

static void SignalStatus_Update()
{
    uint8_t rssi = nrf.GetRSSI();
    uint8_t imgIndex = 0;

    if(rssi > 80)
    {
        imgIndex = 3;
    }
    else if(rssi > 50)
    {
        imgIndex = 2;
    }
    else if(rssi > 0)
    {
        imgIndex = 1;
    }
    else
    {
        imgIndex = 0;
    }
    
    lv_img_set_src(imgRSSI, IMG_SignGrp[imgIndex]);
    
    if(!nrf.RF_Enabled)
    {
        lv_img_set_src(imgUpDownload, &IMG_NoUpDownload);
    }
    else if(nrf.GetRF_State() == nrf.State_TX)
    {
        lv_img_set_src(imgUpDownload, &IMG_Upload);
    }
    else if(nrf.GetRF_State() == nrf.State_RX)
    {
        lv_img_set_src(imgUpDownload, &IMG_Download);
    }
}

/***************蓝牙状态****************/
static void BluetoothStatus_Creat()
{
    /*蓝牙图标*/
    imgBluetooth = lv_img_create(barStatus, NULL);
    lv_img_set_src(imgBluetooth, &IMG_Bluetooth);
    lv_obj_align(imgBluetooth, barStatus, LV_ALIGN_IN_LEFT_MID, 35, 0);
}

static void BluetoothStatus_Update()
{
    lv_obj_set_hidden(imgBluetooth, !CTRL.Bluetooth.Enable);
}

/***************电池状态****************/

static void Battery_Creat()
{
    /*电池图标*/
    imgBatt = lv_img_create(barStatus, NULL);
    lv_img_set_src(imgBatt, &IMG_Battery);
    lv_obj_align(imgBatt, barStatus, LV_ALIGN_IN_RIGHT_MID, -30, 0);
    
    /*电池电量*/
    contBatt = lv_cont_create(imgBatt, NULL);
    lv_cont_set_style(contBatt, LV_CONT_STYLE_MAIN, &lv_style_plain);
    lv_obj_set_size(contBatt, contBatt_Width, 8);
    lv_obj_align(contBatt, NULL, LV_ALIGN_IN_RIGHT_MID, -3, 0);
    lv_obj_set_auto_realign(contBatt, true);
    
    /*电量百分比*/
    labelBatt = lv_label_create(barStatus, NULL);
    LV_LABEL_SET_FONT(labelBatt, HandGotn_14, LV_COLOR_WHITE);
    lv_label_set_text(labelBatt, "--");
    lv_obj_align(labelBatt, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 2);
    lv_obj_set_auto_realign(labelBatt, true);
}

static void PowerLedAnimCallback(lv_obj_t * obj, int16_t val)
{
    Power_SetLedValue(val);
}

static void Battery_Update()
{
    /*电池电量显示*/
    float battCurrent, battVoltageOc;
    Power_GetInfo(&battCurrent, NULL, &battVoltageOc);
    int batUsage = Power_GetBattUsage();
    bool Is_BattCharging = (battCurrent > 0 && batUsage < 100) ? true : false;
    
    static bool Is_BattChargingAnimActive = false;
    static lv_anim_t battAnim, ledAnim;
    
    if(Is_BattCharging)
    {
        if(!Is_BattChargingAnimActive)
        {
            lv_anim_set_repeat(&battAnim, 100);
            lv_obj_add_anim(
                contBatt, &battAnim,
                (lv_anim_exec_xcb_t)lv_obj_set_width,
                0, contBatt_Width,
                1000
            );
            
            lv_anim_set_repeat(&ledAnim, 100);
            lv_anim_set_playback(&ledAnim, 100);
            lv_obj_add_anim(
                contBatt, &ledAnim,
                (lv_anim_exec_xcb_t)PowerLedAnimCallback,
                1000, 0,
                3000
            );
            
            Is_BattChargingAnimActive = true;
        }
    }
    else
    {
        if(Is_BattChargingAnimActive)
        {
            lv_anim_del(contBatt, (lv_anim_exec_xcb_t)lv_obj_set_width);
            lv_anim_del(contBatt, (lv_anim_exec_xcb_t)PowerLedAnimCallback);
            Power_SetLedState(true);
            Is_BattChargingAnimActive = false;
        }
        uint8_t width = map(batUsage, 0, 100, 0, contBatt_Width);
        lv_obj_set_width(contBatt, width);
    }

    lv_style_t * style_label = (lv_style_t *)labelBatt->style_p;
    style_label->text.color = batUsage < 10 ? LV_COLOR_RED : LV_COLOR_WHITE;
    lv_label_set_text_fmt(labelBatt, "%d", batUsage);
}

/***************状态栏****************/
lv_coord_t StatusBar_GetHeight()
{
    return StatusBar_Height;
}

lv_obj_t * StatusBar_GetObj()
{
    return barStatus;
}

void StatusBar_SetOpen(bool open)
{
    LV_OBJ_ADD_ANIM(barStatus, y, open ? 0 : -StatusBar_Height, LV_ANIM_TIME_DEFAULT);
}

static void StatusBar_Update(lv_task_t * task)
{
    I2C_SetLock(true);
    Power_Update();
    I2C_SetLock(false);
    
    SignalStatus_Update();
    BluetoothStatus_Update();
    Battery_Update();
}

void StatusBar_Init()
{
    barStatus = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(barStatus, lv_disp_get_hor_res(NULL), StatusBar_Height);
    lv_obj_set_pos(barStatus, 0, -StatusBar_Height);

    static lv_style_t styleStatusBar;
    lv_style_copy(&styleStatusBar, &lv_style_plain_color);
    styleStatusBar.body.main_color = LV_COLOR_BLACK;
    styleStatusBar.body.grad_color = LV_COLOR_BLACK;
    styleStatusBar.body.opa = LV_OPA_40;
    lv_obj_set_style(barStatus, &styleStatusBar);
    
    SignalStatus_Creat();
    BluetoothStatus_Creat();
    Battery_Creat();
    
    lv_task_create(StatusBar_Update, 1050, LV_TASK_PRIO_LOW, 0);
    StatusBar_Update(NULL);
    StatusBar_SetOpen(true);
}
