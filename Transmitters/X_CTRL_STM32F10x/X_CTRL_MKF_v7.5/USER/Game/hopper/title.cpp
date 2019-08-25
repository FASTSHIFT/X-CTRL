#include "common.h"

/*  Defines  */

enum {
    STATE_MENU = 0,
    STATE_RECORD,
    STATE_CREDIT,
};

enum {
    MENU_START = 0,
    MENU_SOUND,
    MENU_RECORD,
    MENU_CREDIT,
};

enum {
    RECORD_NOT_READ = 0,
    RECORD_INITIAL,
    RECORD_STORED,
};

#define EEPROM_ADDR_BASE    800
#define EEPROM_SIGNATURE    0x024E424FUL // "OBN\x02"

/*  Typedefs  */

typedef struct
{
    const uint8_t *pImg;
    uint8_t width;
} TITLE_LETTER;

/*  Local Functions  */

static void initObjects();
static void moveObjects();
static bool handleButtons(void);
static void setSound(bool enabled);
static void playSound1(void);
static void playSound2(void);
static void readRecord(void);

static void drawTitleMenu(void);
static void drawTitleRecord(void);
static void drawTitleCredit(void);

/*  Local Variables  */

PROGMEM static const uint8_t imgMan[][128] = { // 32x32 x4
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFC, 0xFE, 0x1E, 0x3F, 0xFF, 0x8F, 0x1F, 0xFF, 0xFE, 0xFE,
        0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE0, 0xF3, 0xF7, 0xF7, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFC, 0x7C, 0x3E, 0x1E, 0x1E, 0x1E, 0x3E, 0x3C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x01, 0x00, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0xFF,
        0xFF, 0xFF, 0xFE, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1C, 0x00,
        0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xFC, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xA0, 0x60, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xE0, 0xF0, 0xF8, 0xF8, 0xF8, 0x70, 0x00,
        0x78, 0xFE, 0xFF, 0xE5, 0xE5, 0xE5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF9, 0xFF, 0xFF, 0xFE, 0xFE,
        0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xC7, 0xC3, 0x83, 0x81, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x01, 0x03, 0x03, 0x07, 0x37, 0x7B, 0x7D, 0x3D, 0x3E, 0x0E, 0x0F, 0x07, 0x03, 0x01, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x0F,
    },{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFC, 0xFC, 0xFC,
        0xFC, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x1C, 0x3E, 0x3F, 0x1F, 0x1F, 0x3F, 0x7E, 0xFC, 0xF8, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
        0xF8, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x8F, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0x7E, 0xFF, 0x97, 0x37, 0x6F, 0xEE, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x3E, 0x3F, 0x1F, 0x0F, 0x03, 0x01, 0x01, 0x01, 0x0F,
        0x3F, 0x7F, 0x7F, 0xFE, 0xFC, 0xF9, 0xFF, 0x7E, 0x7F, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
    },{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
        0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xBC, 0x7F, 0x7F, 0x8F, 0xF7,
        0xF7, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xE7, 0xA7, 0xA7, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1E,
        0x00, 0x00, 0x7C, 0xFE, 0xFF, 0x7F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x8F, 0xFF, 0xFF, 0xFF,
        0xE7, 0xDB, 0x3B, 0x7B, 0x77, 0x7F, 0x3F, 0x3F, 0x0F, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07,
        0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }
};

PROGMEM static const uint8_t imgLetterH[] = { // 16x16 "H"
    0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0xC0, 0xC0, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03,
    0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC1, 0x01, 0x01, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
};

PROGMEM static const uint8_t imgLetterO[] = { // 13x11 "o"
    0xF0, 0xFC, 0xFE, 0xFE, 0xFF, 0x83, 0x03, 0x0F, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8,
    0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x06, 0x06, 0x07, 0x03, 0x03, 0x01, 0x00,
};

PROGMEM static const uint8_t imgLetterP[] = { // 14x15 "p"
    0x0C, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0x02, 0x03, 0x07, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8,
    0x30, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x62, 0x06, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00,
};

PROGMEM static const uint8_t imgLetterE[] = { // 11x11 "e"
    0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0x33, 0x33, 0x3F, 0x3E, 0x3E, 0x38,
    0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x03, 0x03,
};

PROGMEM static const uint8_t imgLetterR[] = {
    0x06, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0x0C, 0x1F, 0x1F, 0x1F, 0x0E,
    0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x06, 0x00, 0x00, 0x00,
};

PROGMEM static const TITLE_LETTER letterInfos[] = {
    { imgLetterH, 16 },{ imgLetterO, 13 },{ imgLetterP, 14 },
    { imgLetterP, 14 },{ imgLetterE, 11 },{ imgLetterR, 11 },
};

PROGMEM static const char menuText[] = "START GAME\0SOUND \0RECORD\0CREDIT";
PROGMEM static const char creditText[] = "- " APP_TITLE " -\0\0" APP_RELEASED \
        "\0PROGREMMED BY OBONO\0\0THIS PROGRAM IS\0" "RELEASED UNDER\0" "THE MIT LICENSE.";

static uint8_t  state;
static bool     toDraw;
static bool     sound;
static int8_t   menuPos;
static int8_t   manX, manVy, manType;
static int8_t   letterY[6];
static int8_t   letterVy[6];
static uint16_t lastScore = 0;
static uint8_t  recordState = RECORD_NOT_READ;
static uint16_t hiScore[10];
static uint16_t playCount;
static uint32_t playFrames;

/*---------------------------------------------------------------------------*/
/*                              Main Functions                               */
/*---------------------------------------------------------------------------*/

void initTitle(void)
{
    state = STATE_MENU;
    menuPos = MENU_START;
    toDraw = true;
    initObjects();
    setSound(arduboy.audio.enabled());
    if (recordState == RECORD_NOT_READ) {
        readRecord();
    }
}

bool updateTitle(void)
{
    bool ret = false;
    if (state == STATE_MENU) {
        moveObjects();
        ret = handleButtons();
        toDraw = true;
    } else {
        if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
            state = STATE_MENU;
            playSound2();
            toDraw = true;
        }
    }

#ifdef DEBUG
    if (dbgRecvChar == 'r') {
        arduboy.eepSeek(EEPROM_ADDR_BASE);
        for (int i = 0; i < 8; i++) {
            arduboy.eepWrite32(0);
        }
        recordState = RECORD_INITIAL;
        dprintln("Clean EEPROM");
    }
#endif

    randomSeed(rand() ^ micros()); // Shuffle random
    return ret;
}

void drawTitle(void)
{
    if (toDraw) {
        arduboy.clear();
        switch (state) {
        case STATE_MENU:
            drawTitleMenu();
            break;
        case STATE_RECORD:
            drawTitleRecord();
            break;
        case STATE_CREDIT:
            drawTitleCredit();
            break;
        }
        toDraw = false;
    }
}

uint8_t setLastScore(int score, uint32_t frames)
{
    lastScore = score;

    /*  Updarte best 10  */
    int r = 10;
    uint16_t h;
    while (r > 0 && (h = hiScore[r - 1]) < score) {
        if (--r < 9) hiScore[r + 1] = h;
    }
    if (r < 10) hiScore[r] = score;
    playCount++;
    playFrames += frames;

    /*  Store record to EEPROM  */
    if (recordState == RECORD_INITIAL) {
        arduboy.eepSeek(EEPROM_ADDR_BASE);
        arduboy.eepWrite32(EEPROM_SIGNATURE);
    }
    arduboy.eepSeek(EEPROM_ADDR_BASE + 4 + r * 2);
    for (int i = r; i < 10; i++) {
        arduboy.eepWrite16(hiScore[i]);
    }
    arduboy.eepWrite16(playCount);
    arduboy.eepWrite32(playFrames);

    uint16_t checkSum = (EEPROM_SIGNATURE & 0xFFFF) + (EEPROM_SIGNATURE >> 16) * 2;
    for (int i = 0; i < 10; i++) {
        checkSum += hiScore[i] * (i + 3);
    }
    checkSum += playCount * 13;
    checkSum += (playFrames & 0xFFFF) * 14 + (playFrames >> 16) * 15;
    arduboy.eepWrite16(checkSum);

    recordState = RECORD_STORED;
    return r;
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initObjects()
{
    manVy = 32;
    for (int i = 0; i < 6; i++) {
        letterY[i] = -72;
        letterVy[i] = 0;
    }
}

static void moveObjects()
{
    if (manVy < 32) {
        manVy++;
    } else  {
        manX = random(25);
        manVy = -random(32, 97);
        manType = random(4);
    }

    for (int i = 0; i < 6; i++) {
        int8_t y = letterY[i], vy = letterVy[i];
        y += ++vy;
        if (y >= 64) {
            y = 64;
            vy = -sqrt((float)random(64, 196));
        }
        letterY[i] = y;
        letterVy[i] = vy;
    }
}

static bool handleButtons()
{
    bool ret = false;
    if (arduboy.buttonDown(UP_BUTTON)) {
        if (menuPos-- == MENU_START) menuPos = MENU_CREDIT;
        playSound1();
    }
    if (arduboy.buttonDown(DOWN_BUTTON)) {
        if (menuPos++ == MENU_CREDIT) menuPos = MENU_START;
        playSound1();
    }
    if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
        switch (menuPos) {
        case MENU_START:
            ret = true;
            arduboy.audio.saveOnOff();
            break;
        case MENU_SOUND:
            setSound(!sound);
            playSound2();
            break;
        case MENU_RECORD:
            state = STATE_RECORD;
            playSound2();
            break;
        case MENU_CREDIT:
            state = STATE_CREDIT;
            playSound2();
            break;
        }
    }
    return ret;
}

static void setSound(bool enabled)
{
    if (enabled) {
        arduboy.audio.on();
    } else {
        arduboy.audio.off();
    }
    sound = enabled;
}

static void playSound1(void)
{
    arduboy.tunes.tone(440, 10);
}

static void playSound2(void)
{
    arduboy.tunes.tone(587, 20);
}

static void readRecord(void)
{
    bool isRegular = false;
    arduboy.eepSeek(EEPROM_ADDR_BASE);
    if (arduboy.eepRead32() == EEPROM_SIGNATURE) {
        uint16_t checkSum = (EEPROM_SIGNATURE & 0xFFFF) + (EEPROM_SIGNATURE >> 16) * 2;
        for (int i = 0; i < 13; i++) {
            checkSum += arduboy.eepRead16() * (i + 3);
        }
        isRegular = (arduboy.eepRead16() == checkSum);
    }

    if (isRegular) {
        /*  Read record from EEPROM  */
        arduboy.eepSeek(EEPROM_ADDR_BASE + 4);
        for (int i = 0; i < 10; i++) {
            hiScore[i] = arduboy.eepRead16();
        }
        playCount = arduboy.eepRead16();
        playFrames = arduboy.eepRead32();
        recordState = RECORD_STORED;
    } else {
        /*  Initialize record  */
        for (int i = 0; i < 10; i++) {
            hiScore[i] = 0;
        }
        playCount = 0;
        playFrames = 0;
        recordState = RECORD_INITIAL;
    }
}

/*---------------------------------------------------------------------------*/
/*                              Draw Functions                               */
/*---------------------------------------------------------------------------*/

static void drawTitleMenu(void)
{
    /*  Last score  */
    if (lastScore > 0) {
        arduboy.printEx(0, 0, F("SCORE"));
        arduboy.setCursor(0, 6);
        arduboy.print(lastScore);
    }

    /*  Man  */
    int16_t y = 32 + manVy * manVy / 32;
    if (y < 64) {
        arduboy.drawBitmap(manX, y, imgMan[manType], 32, 32, WHITE);
    }

    /*  "Hopper" letters  */
    for (int i = 0, x = 22; i < 6; i++) {
//        uint8_t *pImg = (uint8_t*)pgm_read_word(&(letterInfos[i].pImg));
//        uint8_t w = pgm_read_byte(&(letterInfos[i].width));
		uint8_t *pImg = (uint8_t*)letterInfos[i].pImg;
		uint8_t w = letterInfos[i].width;
        arduboy.drawBitmap(x, letterY[i] / 4 + (i > 0) * 5, pImg, w, 16, WHITE);
        x += w + 1;
    }

    /*  Menu items  */
    char buf[12];
    const char *p = menuText;
    for (int i = 0; i < 4; i++) {
        strcpy_P(buf, p);
        p += arduboy.printEx(68 - (i == menuPos) * 4, i * 6 + 40, buf) + 1;
        if (i == MENU_SOUND) {
            arduboy.print((sound) ? F("ON") : F("OFF"));
        }
    }
    arduboy.fillRect2(56, menuPos * 6 + 40, 5, 5, WHITE);
}

static void drawTitleRecord(void)
{
    arduboy.printEx(22, 4, F("BEST 10 SCORES"));
    arduboy.drawFastHLine2(0, 12, 128, WHITE);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 5; j++) {
            int r = i * 5 + j;
            arduboy.printEx(i * 60 + 4 - (r == 9) * 6, j * 6 + 14, F("["));
            arduboy.print(r + 1);
            arduboy.print(F("] "));
            arduboy.print(hiScore[r]);
        }
    }
    arduboy.drawFastHLine2(0, 44, 128, WHITE);
    arduboy.printEx(16, 48, F("PLAY COUNT "));
    arduboy.print(playCount);
    arduboy.printEx(16, 54, F("PLAY TIME  "));
    arduboy.print(playFrames / 3600); // minutes
    char buf[6];
    sprintf(buf, "'%02d\"", playFrames / 60 % 60); // seconds
    arduboy.print(buf);
}

static void drawTitleCredit(void)
{
    char buf[20];
    const char *p = creditText;
    for (int i = 0; i < 8; i++) {
        strcpy_P(buf, p);
        uint8_t len = strnlen(buf, sizeof(buf));
        p += arduboy.printEx(64 - len * 3, i * 6 + 8, buf) + 1;
    }
}
