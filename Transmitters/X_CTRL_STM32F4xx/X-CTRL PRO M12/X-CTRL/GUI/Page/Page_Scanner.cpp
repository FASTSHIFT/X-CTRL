#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "Communication/ComPrivate.h"

#define NRF_CHANNELS 126
/*smooths signal strength with numerical range 0 - 0x7FFF*/
static uint16_t SignalStrength[NRF_CHANNELS];
static uint8_t NRF_BKUP_EN_AA = 0;
static uint8_t NRF_BKUP_RF_SETUP = 0;
static uint8_t NRF_BKUP_RF_CH = 0;
static uint8_t NRF_BKUP_CONFIG = 0;
//static bool HoldGraph = false;
static float RecvSensitivity = 1.0f;

static void NRF_Prepare(bool setup)
{
    if(setup)
    {
//        HoldGraph = false;
        nrf.SetRF_Enable(false);

        /*backup nrf reg*/
        NRF_BKUP_EN_AA = nrf.SPI_Read(nrf.EN_AA);
        NRF_BKUP_RF_SETUP = nrf.SPI_Read(nrf.RF_SETUP);
        NRF_BKUP_RF_CH = nrf.SPI_Read(nrf.RF_CH);
        NRF_BKUP_CONFIG = nrf.SPI_Read(nrf.CONFIG);

        /*write nrf reg*/
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, 0x00); // switch off Shockburst mode
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, 0x0F);// write default value to setup register

        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, 0x70);        // reset the flag(s)
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, 0x0F);        // switch to RX mode
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.STATUS, nrf.RX_DR | nrf.TX_DS | nrf.MAX_RT);    //reset the flag(s)
        nrf.RX_Mode();

        delayMicroseconds(130);
        nrf.SetRF_Enable(true);
    }
    else
    {
        nrf.SetRF_Enable(false);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, NRF_BKUP_EN_AA);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, NRF_BKUP_RF_SETUP);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_CH, NRF_BKUP_RF_CH);
        nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, NRF_BKUP_CONFIG);
    }
}

/*控件*/
static lv_obj_t * appWindow;
static lv_obj_t * chartSpectrum;
static lv_chart_series_t * serSpectrum;

static void ChartSpectrum_Creat(lv_obj_t * par)
{
    lv_obj_t * chart = lv_chart_create(par, NULL);
    
    static lv_style_t style_chart;
    style_chart = *lv_chart_get_style(chart, LV_CHART_STYLE_MAIN);
    style_chart.text.color = LV_COLOR_BLACK;
    lv_chart_set_style(chart, LV_CHART_STYLE_MAIN, &style_chart);
    
    lv_obj_set_size(chart, APP_WIN_WIDTH - 40, 1);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
    lv_obj_set_opa_scale_enable(chart, true);
    lv_obj_set_opa_scale(chart, LV_OPA_80);
    lv_chart_set_type(chart, LV_CHART_TYPE_COLUMN);
    lv_chart_set_series_opa(chart, LV_OPA_COVER);
    lv_chart_set_series_width(chart, 10);
    lv_chart_set_series_darking(chart, LV_OPA_50);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);

    lv_chart_set_range(chart, 0, 50);
    lv_chart_set_point_count(chart, NRF_CHANNELS);
    lv_chart_set_div_line_count(chart, 2, 4);
    lv_chart_set_margin(chart, 100);

    //lv_chart_set_y_tick_length(chart, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);
    //lv_chart_set_y_tick_texts(chart, "S\n" "3\n" "2\n" "1\n" "0", 2, LV_CHART_AXIS_SKIP_LAST_TICK);
    lv_chart_set_x_tick_length(chart, LV_CHART_TICK_LENGTH_AUTO, LV_CHART_TICK_LENGTH_AUTO);
    lv_chart_set_x_tick_texts(chart, "2.4G\n" ".425\n" ".45\n" ".475\n" ".5\n" ".525", 2, LV_CHART_AXIS_DRAW_LAST_TICK);

    serSpectrum = lv_chart_add_series(chart, lv_style_plain_color.body.main_color);

    chartSpectrum = chart;

    LV_OBJ_ADD_ANIM(chart, height, APP_WIN_HEIGHT - 40, LV_ANIM_TIME_DEFAULT);
    Page_Delay(LV_ANIM_TIME_DEFAULT);
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
    NRF_Prepare(true);
    ChartSpectrum_Creat(appWindow);
}

static void Loop()
{
    randomSeed(micros());
    for (int MHz = 0; MHz < NRF_CHANNELS; MHz++ )   // tune to frequency (2400 + MHz) so this loop covers 2.400 - 2.525 GHz (maximum range module can handle) when channels is set to 128.
    {
        nrf.SetFreqency(MHz);
        nrf.SetRF_Enable(true); // start receiving
        delayMicroseconds(random(130, 230)); // allow receiver time to tune and start receiving 130 uS seems to be the minimum time.  Random additional delay helps prevent strobing effects with frequency-hopping transmitters.
        nrf.SetRF_Enable(false); // stop receiving - one bit is now set if received power was > -64 dBm at that instant
        if (nrf.SPI_Read(nrf.CD))   // signal detected so increase signalStrength unless already maxed out
        {
            SignalStrength[MHz] += (0x7FFF - SignalStrength[MHz]) >> 5; // increase rapidly when previous value was low, with increase reducing exponentially as value approaches maximum
        }
        else     // no signal detected so reduce signalStrength unless already at minimum
        {
            SignalStrength[MHz] -= SignalStrength[MHz] >> 5; // decrease rapidly when previous value was high, with decrease reducing exponentially as value approaches zero
        }

        float strength = ((SignalStrength[MHz] + 0x0040) >> 7) * RecvSensitivity;
        serSpectrum->points[MHz] = strength;
    }
    lv_chart_refresh(chartSpectrum);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    LV_OBJ_ADD_ANIM(chartSpectrum, height, 0, LV_ANIM_TIME_DEFAULT);
    Page_Delay(LV_ANIM_TIME_DEFAULT);
    lv_obj_clean(appWindow);
    NRF_Prepare(false);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* btn)
{
    if(event == ButtonEvent_Type::EVENT_ButtonPress)
    {
        if(btn == &btBACK)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Scanner(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
