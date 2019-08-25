#ifndef __BUTTON_PRIVATE_H
#define __BUTTON_PRIVATE_H

#include "ButtonEvent.h"
namespace Key{
typedef enum{    
    Up,
    Down,
    Left,
    Right,
    Power,
    Shift,
    Bt1,
    Bt2,
    Bt3,
    Bt4,
    Bt5,
    Bt6,
    MAX
}Key_Type;
}
extern ButtonEvent btGrp[Key::MAX];

#define btUP    btGrp[Key::Up]
#define btDOWN  btGrp[Key::Down]
#define btLEFT  btGrp[Key::Left]
#define btRIGHT btGrp[Key::Right]
#define btOK btRIGHT
#define btBACK btLEFT

#endif
