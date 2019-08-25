#include "common.h"

/*  Defines  */

enum {
    STATE_START = 0,
    STATE_GAME,
    STATE_CLEAR,
    STATE_OVER,
    STATE_PAUSE,
};

#define FLOORS_MANAGE   10
#define FLOORS_LEVEL    50

enum {
    FLRTYPE_NONE = 0,
    FLRTYPE_NORMAL,
    FLRTYPE_MOVE,
    FLRTYPE_VANISH,
    FLRTYPE_SMALL,
    FLRTYPE_GOAL,
};

#define FLOOR_Z_MAX     4096

#define CHIPS           64
#define CHIP_ZRANGE     1024

#define CENTER_X        64
#define CENTER_Y        32
//#define PI              3.141592653589793

enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTER,
    ALIGN_RIGHT,
};

#define nextFloorIdx(i) (((i) + 1) % FLOORS_MANAGE)
#define mid(a, b, c)    max(min((b), (c)), (a))
#define sign(n)         (((n) > 0) - ((n) < 0))

/*  Typedefs  */

typedef struct {
    uint8_t type;
    uint8_t pos;
    int16_t x, y, z;
    int16_t vx, vy;
    uint8_t size, sizeBack;
} FLOOR;

typedef struct
{
    int16_t x, y;
} CHIP;

/*  Local Functions  */

static void initLevel(bool isStart);
static void movePlayer(void);
static void springPlayer(FLOOR *pFloor, int diff);
static void moveFloors(int scrollX, int scrollY, int scrollZ);
static void addFloor(uint8_t type, uint8_t pos, int16_t x, int16_t y, int16_t z);
static void initChips(void);
static void moveChips(int scrollX, int scrollY, int scrollZ);

static void drawPlayer(void);
static void drawFloors(void);
static void drawChips(void);
static void drawStrings(void);
static int  drawFigure(int16_t x, int16_t y, int value, uint8_t align);
static void fillPatternedRect(int16_t x, int16_t y, uint8_t w, int8_t h, const uint8_t *ptn);

/*  Local Variables  */

PROGMEM static const uint8_t imgPlayers[][8] = { // 8x8 x16
    { 0x66, 0x24, 0x3C, 0xFF, 0x3C, 0x5A, 0x5A, 0x3C },
    { 0x30, 0x13, 0xFD, 0x3C, 0x5A, 0xB5, 0xF4, 0x38 },
    { 0x0C, 0x44, 0x3F, 0x5D, 0xBC, 0xF4, 0x6A, 0x30 },
    { 0x26, 0x12, 0x6C, 0x9C, 0xFF, 0xED, 0x54, 0x64 },
    { 0x08, 0x69, 0x9F, 0xFC, 0xFC, 0x9F, 0x69, 0x08 },
    { 0x64, 0x54, 0xED, 0xFF, 0x9C, 0x6C, 0x12, 0x26 },
    { 0x30, 0x6A, 0xF4, 0xBC, 0x5D, 0x3F, 0x44, 0x0C },
    { 0x38, 0xF4, 0xB5, 0x5A, 0x3C, 0xFD, 0x13, 0x30 },
    { 0x3C, 0x5A, 0x5A, 0x3C, 0xFF, 0x3C, 0x24, 0x66 },
    { 0x1C, 0x2F, 0xAD, 0x5A, 0x3C, 0xBF, 0xC8, 0x0C },
    { 0x0C, 0x56, 0x2F, 0x3D, 0xBA, 0xFC, 0x22, 0x30 },
    { 0x26, 0x2A, 0xB7, 0xFF, 0x39, 0x36, 0x48, 0x64 },
    { 0x10, 0x96, 0xF9, 0x3F, 0x3F, 0xF9, 0x96, 0x10 },
    { 0x64, 0x48, 0x36, 0x39, 0xFF, 0xB7, 0x2A, 0x26 },
    { 0x30, 0x22, 0xFC, 0xBA, 0x3D, 0x2F, 0x56, 0x0C },
    { 0x0C, 0xC8, 0xBF, 0x3C, 0x5A, 0xAD, 0x2F, 0x1C },
};

PROGMEM static const uint8_t posEyesMask[16] = {
    0x24, 0x34, 0x33, 0x43, 0x42, 0x41, 0x31, 0x30, 0x20, 0x10, 0x11, 0x01, 0x02, 0x03, 0x13, 0x14
};

PROGMEM static const uint8_t imgFloor[][4] = { // 4x8 x4
    { 0x55, 0xAA, 0x55, 0xAA },
    { 0x33, 0x66, 0xCC, 0x99 },
    { 0x00, 0xAA, 0x00, 0xAA },
    { 0x55, 0xFF, 0x55, 0xFF }
};

PROGMEM static const uint8_t imgFigures[][16] = { // 8x12 x10
    { 0x78, 0xFC, 0xFE, 0x86, 0xFE, 0xFC, 0x78, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00 },
    { 0x8C, 0x8C, 0xFE, 0xFE, 0xFE, 0x86, 0x80, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x0C, 0x86, 0xE6, 0xFE, 0xBE, 0xDC, 0xC0, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x00 },
    { 0x04, 0x26, 0x36, 0x3E, 0xFE, 0xF4, 0xE0, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00 },
    { 0xE0, 0xF0, 0xD8, 0xFC, 0xFE, 0xFE, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00 },
    { 0x38, 0x3E, 0x36, 0x76, 0xF6, 0xE6, 0xE6, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00 },
    { 0x78, 0xFE, 0xFF, 0x9F, 0xF8, 0xF8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00 },
    { 0x0E, 0x0E, 0x06, 0xC6, 0xF6, 0x7E, 0x06, 0x00, 0x00, 0x00, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00 },
    { 0xEE, 0xFF, 0x3F, 0x7D, 0xF9, 0xFF, 0xE6, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00 },
    { 0x3C, 0x7E, 0x7E, 0xE6, 0xFE, 0xFC, 0x7C, 0x00, 0x00, 0x02, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00 },
};

PROGMEM static const uint8_t imgStart[] = { // 50x12 "Ready?"
    0x03, 0xFF, 0xFF, 0xFF, 0x33, 0xF3, 0xFF, 0xFF, 0x8E, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x48, 0x78,
    0x70, 0x00, 0x90, 0xD8, 0x48, 0xF8, 0xF8, 0xF0, 0x00, 0x00, 0xF0, 0xF8, 0xF8, 0x1A, 0xFE, 0xFE,
    0xFE, 0x00, 0x00, 0x18, 0x38, 0xF8, 0xF8, 0xD8, 0xC0, 0x78, 0x18, 0x00, 0x0E, 0x8F, 0xB3, 0xBF,
    0x1F, 0x0E, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03,
    0x03, 0x03, 0x01, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x02, 0x00, 0x06, 0x0E, 0x0C, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x03, 0x03, 0x00, 0x00
};

PROGMEM static const uint8_t imgClear[] = { // 68x10 "Excellent!"
    0x03, 0xFF, 0xFF, 0xFF, 0x33, 0x7B, 0x7B, 0x87, 0xC0, 0x00, 0x18, 0xB8, 0x78, 0xF8, 0xE0, 0xB8,
    0x18, 0x00, 0xE0, 0xF0, 0xF8, 0x18, 0x38, 0xB0, 0x00, 0xE0, 0xF0, 0xF8, 0x48, 0x78, 0x70, 0x00,
    0x04, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x04, 0xFE, 0xFE, 0xFE, 0x00, 0xE0, 0xF0, 0xF8, 0x48, 0x78,
    0x70, 0x00, 0x10, 0xF0, 0xF8, 0xF8, 0x10, 0xF8, 0xF8, 0x00, 0x18, 0xFC, 0xFC, 0xFE, 0x18, 0x18,
    0x00, 0x8F, 0xBF, 0x9F, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x03, 0x03,
    0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x03, 0x03,
    0x03, 0x03, 0x01, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
    0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x00, 0x01,
    0x03, 0x03, 0x03, 0x01, 0x00, 0x03, 0x03, 0x03
};

PROGMEM static const uint8_t imgOver[] = { // 76x10 "Game Over"
    0x78, 0xFE, 0xFE, 0x87, 0x03, 0x63, 0xE7, 0xEF, 0xEE, 0x60, 0x00, 0x90, 0xD8, 0x48, 0xF8, 0xF8,
    0xF0, 0x00, 0x00, 0x10, 0xF8, 0xF8, 0xF8, 0x10, 0xF8, 0xF8, 0xF8, 0x10, 0xF8, 0xF8, 0xF8, 0x00,
    0xE0, 0xF0, 0xF8, 0x48, 0x78, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xFE, 0xFE, 0xC3, 0x83,
    0x07, 0x0F, 0xFE, 0xFE, 0x78, 0x00, 0x18, 0x38, 0xF8, 0xF8, 0xD8, 0xC0, 0x38, 0x18, 0x00, 0xE0,
    0xF0, 0xF8, 0x48, 0x78, 0x70, 0x00, 0x10, 0xF8, 0xF8, 0xF8, 0x18, 0x18, 0x00, 0x01, 0x01, 0x03,
    0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x03,
    0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03,
    0x01, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00
};

PROGMEM static const byte soundStart[] = {
    0x90, 72, 0, 100, 0x80, 0, 25,
    0x90, 74, 0, 100, 0x80, 0, 25,
    0x90, 76, 0, 100, 0x80, 0, 25,
    0x90, 77, 0, 100, 0x80, 0, 25,
    0x90, 79, 0, 200, 0x80, 0xF0
};

PROGMEM static const byte soundJump[] = {
    0x90,  5, 0, 15, 0x90, 17, 0, 15,
    0x90, 29, 0, 15, 0x90, 41, 0, 15,
    0x90, 53, 0, 15, 0x90, 65, 0, 15,
    0x90, 77, 0, 15, 0x90, 89, 0, 15, 0x80, 0xF0
};

PROGMEM static const byte soundClear[] = {
    0x90, 81, 0, 40, 0x80, 0, 40,
    0x90, 86, 0, 40, 0x80, 0, 40,
    0x90, 90, 0, 40, 0x80, 0, 40, 0xE0
};

PROGMEM static const byte soundOver[] = {
    0x90, 55, 0, 120, 0x80, 0, 10,
    0x90, 54, 0, 140, 0x80, 0, 20,
    0x90, 53, 0, 160, 0x80, 0, 30,
    0x90, 52, 0, 180, 0x80, 0, 40,
    0x90, 51, 0, 200, 0x80, 0, 50,
    0x90, 50, 0, 220, 0x80, 0, 60,
    0x90, 49, 0, 240, 0x80, 0, 70,
    0x90, 48, 0, 260, 0x80, 0xF0
};

static uint8_t  state;
static bool     toDraw;
static uint32_t gameFrames;
static int      counter;
static int8_t   ignoreCnt;
static uint     score;
static uint     level;
static bool     isHiscore;
static int16_t  playerX, playerY;
static int8_t   playerJump, playerTorque;
static uint8_t  playerRotate;
static FLOOR    floorAry[FLOORS_MANAGE];
static uint8_t  floorIdxFirst, floorIdxLast;
static uint8_t  flrTypes[4], flrTypesNum;
static FLOOR    *pFloorBlur;
static CHIP     chipAry[CHIPS];
static uint16_t chipBaseZ;
static bool     isBlink;

/*---------------------------------------------------------------------------*/
/*                              Main Functions                               */
/*---------------------------------------------------------------------------*/

void initGame(void)
{
    ignoreCnt = 0;
    playerX = 0;
    playerY = 0;
    playerJump = 0;
    playerTorque = 0;
    playerRotate = 196;
    pFloorBlur = NULL;
    isBlink = false;
    initLevel(true);
    initChips();
}

bool updateGame(void)
{
    /*  In cas of pausing  */
    if (state == STATE_PAUSE) {
        if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
            state = STATE_GAME;
            ignoreCnt = 30; // 1/2 sec
            toDraw = true;
            dprintln("Resume");
        }
        return false;
    }

    /*  Move objects  */
    int scrollX = -playerX / 4;
    int scrollY = -playerY / 4;
    playerX += scrollX;
    playerY += scrollY;
    playerRotate += playerTorque;
    if (playerJump > -64) {
        if (--playerJump == 0) {
            pFloorBlur = NULL;
            if (state == STATE_CLEAR) {
                initLevel(false);
            }
        }
    }
    if (state != STATE_OVER) {
        moveFloors(scrollX, scrollY, playerJump);
    }
    moveChips(scrollX, scrollY, playerJump);

    /*  Player interaction  */
    if (state == STATE_GAME || state == STATE_OVER) {
        movePlayer();
        if (state == STATE_GAME) {
            gameFrames++;
            if (floorAry[floorIdxFirst].z < 0) {
                state = STATE_OVER;
                isHiscore = (setLastScore(score, gameFrames) == 0);
                counter = 480; // 8 secs
                arduboy.playScore2(soundOver, 1);
                dprint("Game Over: score=");
                dprintln(score);
            } else if (ignoreCnt == 0 && arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
                state = STATE_PAUSE;
                dprintln("Pause");
            }
        } else {
            counter--;
            if (ignoreCnt == 0 && arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
                initLevel(true);
            }
        }
    }
    if (ignoreCnt > 0) ignoreCnt--;
    toDraw = true;

    return (state == STATE_OVER && counter == 0);
}

void drawGame(void)
{
    if (toDraw) {
        arduboy.clear();
        if (state != STATE_PAUSE) {
            drawChips();
            drawFloors();
        }
        drawPlayer();
        drawStrings();
        isBlink = !isBlink;
        toDraw = false;
    }
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initLevel(bool isStart) {
    if (isStart) {
        state = STATE_START;
        gameFrames = 0;
        score = 0;
        level = 0;
        arduboy.playScore2(soundStart, 0);
    } else {
        level++;
        arduboy.tunes.stopScore();
    }
    dprint("Init Level ");
    dprintln(level);

    flrTypesNum = 0;
    if (level < 6) flrTypes[flrTypesNum++] = FLRTYPE_NORMAL;
    if (level ==1) flrTypes[flrTypesNum++] = FLRTYPE_NORMAL;
    if (level & 1) flrTypes[flrTypesNum++] = FLRTYPE_MOVE;
    if (level & 2) flrTypes[flrTypesNum++] = FLRTYPE_VANISH;
    if (level & 4 || level == 8) flrTypes[flrTypesNum++] = FLRTYPE_SMALL;

    for (int i = 0; i < FLOORS_MANAGE; i++) {
        floorAry[i].type = FLRTYPE_NONE;
    }
    floorIdxFirst = 0;
    floorIdxLast = 0;
    addFloor(FLRTYPE_NORMAL, 0, playerX, playerY, FLOOR_Z_MAX);
}

static void movePlayer(void)
{
    int vx = 0, vy = 0, vr;
    if (arduboy.buttonPressed(LEFT_BUTTON)) vx--;
    if (arduboy.buttonPressed(RIGHT_BUTTON)) vx++;
    if (arduboy.buttonPressed(UP_BUTTON)) vy--;
    if (arduboy.buttonPressed(DOWN_BUTTON)) vy++;
    vr = (vx != 0 && vy != 0) ? 384 : 512;
    playerX += vx * vr;
    playerY += vy * vr;

    if (vx != 0 || vy != 0) {
        uint8_t targetRotate = (uint8_t) (atan2((float)vy, (float)vx) * 128.0f / PI);
        int8_t diffRotate = targetRotate - playerRotate;
        if (abs(playerTorque) >= 2) {
            if (abs(diffRotate) < 3) playerTorque -= sign(playerTorque);
        } else {
            playerTorque = mid(-2, diffRotate, 2);
        }
    }
}

static void springPlayer(FLOOR *pFloor, int diff)
{
    score = max(score, level * FLOORS_LEVEL + pFloor->pos);
    playerTorque = (random(2) * 2 - 1) * mid(1, diff / 512 - 2, 8);
    pFloorBlur = pFloor;
    if (pFloor->type == FLRTYPE_GOAL) {
        playerJump = 96;
        state = STATE_CLEAR;
        arduboy.playScore2(soundClear, 2);
    } else {
        playerJump = 64;
        if (pFloor->type == FLRTYPE_VANISH) {
            pFloor->size--;
        }
        if (state == STATE_START || state == STATE_CLEAR) {
            state = STATE_GAME;
        }
        arduboy.playScore2(soundJump, 3);
    }
}

static void moveFloors(int scrollX, int scrollY, int scrollZ)
{
    FLOOR *pFloor;
    for (int i = 0; i < FLOORS_MANAGE; i++) {
        pFloor = &floorAry[i];
        if (pFloor->type != FLRTYPE_NONE) {
            pFloor->x += scrollX + pFloor->vx;
            pFloor->y += scrollY + pFloor->vy;
            pFloor->z += scrollZ;
            int z = pFloor->z;
            if (pFloor->type == FLRTYPE_VANISH) {
                if (z < 0) {
                    pFloor->size = pFloor->sizeBack;
                } else if (pFloor->size != pFloor->sizeBack && pFloor->size > 0) {
                    pFloor->size--;
                }
            }
            if (scrollZ <= 0 && z >= 0 && z + scrollZ - 1 < 0) {
                int diff = max(abs(pFloor->x - playerX), abs(pFloor->y - playerY));
                if (diff < pFloor->size * 128) {
                    springPlayer(pFloor, diff);
                    dprint("Spring floor ");
                    dprintln(i);
                }
            }
        }
    }

    pFloor = &floorAry[floorIdxFirst];
    if (pFloor->z > FLOOR_Z_MAX) {
        pFloor->type = FLRTYPE_NONE;
        floorIdxFirst = nextFloorIdx(floorIdxFirst);
        dprint("floorIdxFirst=");
        dprintln(floorIdxFirst);
    }

    pFloor = &floorAry[floorIdxLast];
    int z = pFloor->z;
    if (state == STATE_GAME && z >= 0 && pFloor->type != FLRTYPE_GOAL) {
        uint8_t pos = pFloor->pos + 1;
        uint8_t type = (pos == FLOORS_LEVEL) ? FLRTYPE_GOAL : flrTypes[random(flrTypesNum)];
        floorIdxLast = nextFloorIdx(floorIdxLast);
        addFloor(type, pos, rand() * 2, rand() * 2, z - (512 + level * 32));
        dprint("floorIdxLast=");
        dprintln(floorIdxLast);
    }

#ifdef DEBUG
    if (dbgRecvChar == 'z') {
        pFloor->type = FLRTYPE_GOAL;
        springPlayer(pFloor, 0);
    }
#endif
}

static void addFloor(uint8_t type, uint8_t pos, int16_t x, int16_t y, int16_t z)
{
    FLOOR *pFloor = &floorAry[floorIdxLast];
    pFloor->type = type;
    pFloor->pos = pos;
    pFloor->x = x;
    pFloor->y = y;
    pFloor->z = z;

    double vd = random(256) * PI / 128.0f;
    int vr = (type == FLRTYPE_MOVE) ? 256 + level * 16 : 0;
    pFloor->vx = cos(vd) * vr;
    pFloor->vy = sin(vd) * vr;

    uint8_t size = (type == FLRTYPE_SMALL) ? 36 - level : 64 - level * 3;
    pFloor->size = max(size, 16);
    pFloor->sizeBack = pFloor->size;

    dprint("New Floor: type=");
    dprint(type);
    dprint(" size=");
    dprintln(size);
}

static void initChips(void)
{
    for (int i = 0; i < CHIPS; i++) {
        CHIP *pChip = &chipAry[i];
        pChip->x = rand() * 2;
        pChip->y = rand() * 2;
    }
    chipBaseZ = 0;
}

static void moveChips(int scrollX, int scrollY, int scrollZ)
{
    for (int i = 0; i < CHIPS; i++) {
        CHIP *pChip = &chipAry[i];
        pChip->x += scrollX;
        pChip->y += scrollY;
    }
    chipBaseZ += scrollZ;
    chipBaseZ %= CHIP_ZRANGE;
}

/*---------------------------------------------------------------------------*/
/*                              Draw Functions                               */
/*---------------------------------------------------------------------------*/

static void drawPlayer(void)
{
    int8_t  x = playerX / 256 + CENTER_X - 4;
    int8_t  y = playerY / 256 + CENTER_Y - 4;
    uint8_t rotate = (playerRotate + 8) / 16 % 16;

    if (isBlink) {
        for (int p = 0; p < 9; p++) {
            arduboy.drawBitmap(x + (p % 3) - 1, y + (p / 3) - 1, imgPlayers[rotate], 8, 8, BLACK);
        }
    } else {
        uint8_t p = pgm_read_byte(posEyesMask + rotate);
        arduboy.fillRect2(x + (p % 16), y + (p / 16), 4, 4, BLACK);
    }
    arduboy.drawBitmap(x, y, imgPlayers[rotate], 8, 8, WHITE);
}

static void drawFloors(void)
{
    for (int i = 0; i < FLOORS_MANAGE; i++) {
        FLOOR   *pFloor = &floorAry[(floorIdxFirst + i) % FLOORS_MANAGE];
        uint8_t type = pFloor->type;
        if (pFloor->type == FLRTYPE_NONE) break;
        if (pFloor->z < 0 || pFloor->size == 0) continue;
        int16_t q = 256 + pFloor->z;
        uint8_t s = pFloor->size * 256 / q;
        int16_t x = pFloor->x / q + CENTER_X - s / 2;
        int16_t y = pFloor->y / q + CENTER_Y - s / 2;
        if (pFloor == pFloorBlur) {
            int blur = (playerJump + 16) / 32;
            x += blur * random(-1, 2);
            y += blur * random(-1, 2);
        }
        if (type == FLRTYPE_GOAL) {
            arduboy.fillRect2(x, y, s, s, (isBlink) ? WHITE : BLACK);
        } else {
            fillPatternedRect(x, y, s, s, imgFloor[type - 1]);
        }
        arduboy.drawRect2(x, y, s, s, WHITE);
    }
}

static void drawChips(void)
{
    for (int i = 0; i < CHIPS; i++) {
        CHIP *pChip = &chipAry[i];
        int16_t q1 = 256 + (chipBaseZ + i * (CHIP_ZRANGE / CHIPS)) % CHIP_ZRANGE;
        int16_t x1 = pChip->x / q1 + CENTER_X;
        int16_t y1 = pChip->y / q1 + CENTER_Y;
        int16_t q2 = q1 + abs(playerJump);
        int16_t x2 = pChip->x / q2 + CENTER_X;
        int16_t y2 = pChip->y / q2 + CENTER_Y;
        arduboy.drawLine(x1, y1, x2, y2, WHITE);
    }
}

static void drawStrings(void)
{
    if (state == STATE_START) {
        arduboy.drawBitmap(39, 12, imgStart, 50, 12, WHITE);
    } else {
        arduboy.printEx(0, 0, F("SCORE"));
        drawFigure(0, 6, score, ALIGN_LEFT);
        arduboy.printEx(98, 0, F("LEVEL"));
        drawFigure(120, 6, level + 1, ALIGN_RIGHT);
        if (state == STATE_CLEAR) {
            arduboy.drawBitmap(30, 12, imgClear, 68, 12, WHITE);
            arduboy.printEx(22, 46, F("TRY NEXT LEVEL"));
        } else if (state == STATE_OVER) {
            arduboy.drawBitmap(random(25, 28), random(11, 14), imgOver, 76, 12, WHITE);
            if (isHiscore && counter % 8 < 4) {
                arduboy.printEx(31, 46, F("NEW RECORD!"));
            }
        } else if (state == STATE_PAUSE) {
            arduboy.printEx(49, 46, F("PAUSE"));
        }

    }
}

static int  drawFigure(int16_t x, int16_t y, int value, uint8_t align)
{
    int k = (value > 9) ? drawFigure(x - align * 4, y, value / 10, align) : 0;
    arduboy.drawBitmap(x + k, y, imgFigures[value % 10], 8, 16, WHITE);
    return k + 8 - align * 4;
}

static void fillPatternedRect(int16_t x, int16_t y, uint8_t w, int8_t h, const uint8_t *ptn)
{
    /*  Check parameters  */
    if (x < 0) {
        if (w <= -x) return;
        w += x;
        x = 0;
    }
    if (y < 0) {
        if (h <= -y) return;
        h += y;
        y = 0;
    }
    if (w <= 0 || x >= WIDTH || h <= 0 || y >= HEIGHT) return;
    if (x + w > WIDTH) w = WIDTH - x;
    if (y + h > HEIGHT) h = HEIGHT - y;

    /*  Draw a patterned rectangle  */
    uint8_t yOdd = y & 7;
    uchar d = 0xFF << yOdd;
    y -= yOdd;
    h += yOdd;
    for (uchar *p = arduboy.getBuffer() + x + (y / 8) * WIDTH; h > 0; h -= 8, p += WIDTH - w) {
        if (h < 8) d &= 0xFF >> (8 - h);
        for (uint8_t i = w; i > 0; i--, p++) {
            *p = *p & ~d | pgm_read_byte(ptn + (int) p % 4) & d;
        }
        d = 0xFF;
    }
}
