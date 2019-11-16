#ifndef __MENUMANAGER_H
#define __MENUMANAGER_H

#include "WString.h"
#include "PageManager.h"

class MenuManager {
    typedef void(*CallbackFunction_t)(void);
    typedef struct {
        String LabelText;
        uint8_t Type;
        int Data;
    } Item_TypeDef;
public:
    typedef enum{
        TYPE_Text,
        TYPE_Bool,
        TYPE_PageJump,
        TYPE_Callback
    }Item_Type;
    MenuManager(int16_t itemNum_Max, int16_t itemNumDisplay_Max, PageManager *page = 0);
    ~MenuManager();
    CallbackFunction_t CallbackFunc_ItemSelectEvent;
    int16_t ItemNum_Max;
    int16_t ItemNumDisplay_Max;
    int16_t ItemSelect;
    int16_t ItemTop;
    
    bool ItemAutoSquence;
    bool ItemSelectUpdating;
    bool ItemTopUpdating;
    bool ItemXUpdating;
    bool SelectMove(int16_t step);
    String *ItemStrBuffer;
    Item_TypeDef *ItemList;

    void Running();
    void UpdateItemStrBuffer();
    bool UpdateItem(int16_t itemNum, String text, Item_Type type = TYPE_Text, int data = 0);
    void ItemSelectEvent();
    int16_t GetItemDispalyPos();
private:
    PageManager *Page;
};

#endif
