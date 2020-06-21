#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "core.h"
#include "common.h"

static uint8_t buttons;
static int16_t BaseX = (screen.width() - WIDTH) / 2;
static int16_t BaseY = (screen.height() - HEIGHT) / 2;

ArduboyCore::ArduboyCore()
{}

void ArduboyCore::boot()
{
//  #if F_CPU == 8000000L
//  slowCPU();
//  #endif

//  SPI.begin();
    bootPins();
    bootLCD();

//  #ifdef SAFE_MODE
//  if (buttonsState() == (LEFT_BUTTON | UP_BUTTON))
//    safeMode();
//  #endif

//  saveMuchPower();
}

void ArduboyCore::bootLCD()
{

}

void ArduboyCore::bootPins()
{

}

uint8_t ArduboyCore::width() {
    return WIDTH;
}

uint8_t ArduboyCore::height() {
    return HEIGHT;
}

void ArduboyCore::paintScreen(const unsigned char *image)
{
    screen.setAddrWindow(BaseX, BaseY, BaseX + WIDTH - 1, HEIGHT + BaseY - 1);

    for(int16_t y = 0; y < HEIGHT; y++)
    {
        for(int16_t x = 0; x < WIDTH; x++ )
        {
            uint8_t row = (uint8_t)y / 8;
            if(image[(row*WIDTH) + (uint8_t)x] &  _BV((uint8_t)y % 8))
            {
                screen.pushColor(screen.White);
            }
            else
            {
                screen.pushColor(screen.Black);
            }
        }
    }
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void ArduboyCore::paintScreen(unsigned char image[])
{
    screen.setAddrWindow(BaseX, BaseY, BaseX + WIDTH - 1, HEIGHT + BaseY - 1);

    for(int16_t y = 0; y < HEIGHT; y++)
    {
        for(int16_t x = 0; x < WIDTH; x++ )
        {
            uint8_t row = (uint8_t)y / 8;
            if(image[(row*WIDTH) + (uint8_t)x] &  _BV((uint8_t)y % 8))
            {
                screen.pushColor(screen.White);
            }
            else
            {
                screen.pushColor(screen.Black);
            }
        }
    }
}

void ArduboyCore::blank()
{

}

void ArduboyCore::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
}

/* Buttons */

uint8_t ArduboyCore::getInput()
{
    return buttonsState();
}

uint8_t ArduboyCore::buttonsState()
{
    if(btDOWN)
    {
        buttons |= A_BUTTON;
    }
    else
    {
        buttons &= ~A_BUTTON;
    }

    if(btUP)
    {
        buttons |= B_BUTTON;
    }
    else
    {
        buttons &= ~B_BUTTON;
    }

    if(CTRL.JS_L.Y.Val < -200)
    {
        buttons |= UP_BUTTON;
    }
    else
    {
        buttons &= ~UP_BUTTON;
    }

    if(CTRL.JS_L.Y.Val > 200)
    {
        buttons |= DOWN_BUTTON;
    }
    else
    {
        buttons &= ~DOWN_BUTTON;
    }

    if(CTRL.JS_R.X.Val < - 200)
    {
        buttons |= LEFT_BUTTON;
    }
    else
    {
        buttons &= ~LEFT_BUTTON;
    }

    if(CTRL.JS_R.X.Val > 200)
    {
        buttons |= RIGHT_BUTTON;
    }
    else
    {
        buttons &= ~RIGHT_BUTTON;
    }

    return buttons;
}

