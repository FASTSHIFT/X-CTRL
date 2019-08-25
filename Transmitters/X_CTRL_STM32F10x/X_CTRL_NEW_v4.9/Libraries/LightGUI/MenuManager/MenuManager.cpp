#include "MenuManager.h"

MenuManager::MenuManager(int16_t itemNum_Max, int16_t itemNumDisplay_Max, PageManager *page)
{
    ItemList = new Item_TypeDef[itemNum_Max];
    ItemStrBuffer = new String[itemNumDisplay_Max];
    
    ItemNum_Max = itemNum_Max;
    ItemNumDisplay_Max = itemNumDisplay_Max;
    Page = page;
}

MenuManager::~MenuManager()
{
    delete[] ItemList;
    delete[] ItemStrBuffer;
}

void MenuManager::UpdateItemStrBuffer()
{
    for(int16_t i = 0; i < ItemNumDisplay_Max; i++)
    {
        int16_t pos = (ItemTop + i) % (ItemNum_Max + 1);
        ItemStrBuffer[i] = ItemList[pos].LabelText;

//        if(ItemList[pos].Type == TYPE_Bool)
//            ItemStrBuffer[i] += String((*((bool*)ItemList[pos].Data)) ? "ON" : "OFF");
    }
}

bool MenuManager::UpdateItem(int16_t itemNum, String text, Item_Type type, int data)
{
    if(itemNum < 0 || itemNum >= ItemNum_Max)
        return false;

    ItemList[itemNum].LabelText = String(itemNum + 1) + text;
    ItemList[itemNum].Type = type;
    ItemList[itemNum].Data = data;

    return true;
}

bool MenuManager::SelectMove(int16_t step)
{
    if(ItemSelectUpdating || ItemTopUpdating)
        return false;

    if(step > 0)
    {
        ItemSelect = (ItemSelect + step) % ItemNum_Max;
    }
    else if(step < 0)
    {
        ItemSelect = (ItemSelect + ItemNum_Max + step) % ItemNum_Max;
    }
    ItemSelectUpdating = true;
    if(ItemSelect - ItemTop > (ItemNumDisplay_Max - 1))
    {
        ItemTop = ItemSelect - (ItemNumDisplay_Max - 1);
        ItemTopUpdating = true;
    }
    if(ItemTop > ItemSelect)
    {
        ItemTop = ItemSelect;
        ItemTopUpdating = true;
    }

    return true;
}


void MenuManager::Running()
{

}

void MenuManager::ItemSelectEvent()
{
    if(CallbackFunc_ItemSelectEvent)
        CallbackFunc_ItemSelectEvent();

    if(ItemList[ItemSelect].Type == TYPE_Text)
    {
        ItemXUpdating = true;
    }
    else if(ItemList[ItemSelect].Type == TYPE_Bool)
    {
        *((bool*)ItemList[ItemSelect].Data) = ! *((bool*)ItemList[ItemSelect].Data);
        ItemXUpdating = true;
    }
    else if(ItemList[ItemSelect].Type == TYPE_PageJump)
    {
        if(Page)
            Page->PageChangeTo(ItemList[ItemSelect].Data);
    }
    else if(ItemList[ItemSelect].Type == TYPE_Callback)
    {
        if((CallbackFunction_t)ItemList[ItemSelect].Data)
            ((CallbackFunction_t)ItemList[ItemSelect].Data)();
    }
}
