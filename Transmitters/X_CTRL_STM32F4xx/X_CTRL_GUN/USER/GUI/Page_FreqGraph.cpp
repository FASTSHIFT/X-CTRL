#include "FileGroup.h"
#include "ComPrivate.h"
#include "DisplayPrivate.h"

// the nRF24L01+ can tune to 126 channels with 1 MHz spacing from 2.400 GHz to 2.525 GHz.
#define NRF_CHANNELS 126
#define TEXT_BASE_Y (StatusBar_Height + TEXT_HEIGHT_1)
#define DISP_BASE_Y (TEXT_HEIGHT_1+10)
#define DISP_MAX_HEIGHT (screen.height() - StatusBar_Height - DISP_BASE_Y - 5)
static uint16_t SignalStrength[NRF_CHANNELS]; // smooths signal strength with numerical range 0 - 0x7FFF
static uint8_t NRF_BKUP_EN_AA = 0;
static uint8_t NRF_BKUP_RF_SETUP = 0;
static uint8_t NRF_BKUP_RF_CH = 0;
static uint8_t NRF_BKUP_CONFIG = 0;
static bool HoldGraph = false;
static float RecvSensitivity = 0.4f;

void Draw_RFScaleplate(int16_t x, int16_t y, int16_t length, uint16_t color)
{
    //screen.drawFastHLine(0, screen.height() - BASE_Y + 1, CHANNELS, screen.White);
    screen.drawFastHLine(x, y, length, color);

    for (int f = 0; f < length; f++)
    {
        if (!(f % 10))
        {
            screen.drawFastVLine(x + f, y, 3, color); // graduation tick every 10 MHz
        }
        if (f >= 10 && ((f - 10) % 50 == 0))
        {
            screen.drawFastVLine(x + f, y, 5, color); // scale markers at 2.41, 2.46, and 2.51 GHz
            screen.setTextColor(color);
            screen.setCursor((x + f) - TEXT_WIDTH_1 * 1.5f, y + 6);
            screen.print(2.4f + 0.001f * f, 2);
        }
    }
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    State_RF = false;
    HoldGraph = false;
    ClearPage();

    screen.setTextColor(screen.White, screen.Black);
    screen.setTextSize(1);
    screen.setCursor(TextMid(0.5f, 7), TEXT_BASE_Y);
    screen.print("2.4 GHz");
    screen.setCursor(TextMid(0.5f, 12), TEXT_BASE_Y + TEXT_HEIGHT_1);
    screen.print("Band scanner");
    screen.setCursor(TextMid(0.5f, 20), TEXT_BASE_Y + TEXT_HEIGHT_1 * 3);
    screen.print("By ceptimus. Nov '16");
    PageDelay(1000);
    ClearPage();

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

    Draw_RFScaleplate(0, screen.height() - DISP_BASE_Y + 1, NRF_CHANNELS, screen.White);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    static uint8_t refresh;
    randomSeed(RNG_GetRandNum());
    for (int MHz = 0; MHz < NRF_CHANNELS; MHz++ )   // tune to frequency (2400 + MHz) so this loop covers 2.400 - 2.527 GHz (maximum range module can handle) when channels is set to 128.
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
        // Serial.print((signalStrength[MHz] + 0x0100) >> 9, HEX); // debugging without lcd display
        // Serial.print(" "); // debugging without lcd display


        __ValueMonitor(
            (int(RecvSensitivity * 10)),
            (
                screen.setTextColor(screen.White, screen.Black),
                screen.setCursor(TextMid(0.8f, 3), StatusBar_Height + TEXT_HEIGHT_1),
                screen.print(RecvSensitivity, 1)
            )
        );

        if(HoldGraph)
        {
            screen.setTextColor(screen.Red);
            screen.setCursor(TextMid(0.5f, 4), ScreenMid_H);
            screen.print("HOLD");
            return;
        }

        if (!--refresh)   // don't refresh whole display every scan (too slow)
        {
            refresh = 19; // speed up by only refreshing every n-th frequency loop - reset number should be relatively prime to CHANNELS
            float strength = ((SignalStrength[MHz] + 0x0040) >> 7) * RecvSensitivity;
            __LimitValue(strength, 0.0f, (float)DISP_MAX_HEIGHT);

            screen.drawFastVLine(MHz, screen.height() - DISP_BASE_Y - DISP_MAX_HEIGHT, DISP_MAX_HEIGHT - strength, screen.Black);
            screen.drawFastVLine(MHz, screen.height() - DISP_BASE_Y - strength, strength, screen.Yellow);
        }
    }
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    nrf.SetRF_Enable(false);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.EN_AA, NRF_BKUP_EN_AA);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_SETUP, NRF_BKUP_RF_SETUP);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.RF_CH, NRF_BKUP_RF_CH);
    nrf.SPI_RW_Reg(nrf.WRITE_REG + nrf.CONFIG, NRF_BKUP_CONFIG);
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonPress)
    {
        if(btOK)
            HoldGraph = !HoldGraph;

        if(btUP)
            RecvSensitivity += 0.1f;

        if(btDOWN)
            RecvSensitivity -= 0.1f;

        __LimitValue(RecvSensitivity, 0.1f, 1.0f);
    }
    if(event == EVENT_ButtonLongPressed)
    {
        if(btBACK)
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
void PageRegister_FreqGraph(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
