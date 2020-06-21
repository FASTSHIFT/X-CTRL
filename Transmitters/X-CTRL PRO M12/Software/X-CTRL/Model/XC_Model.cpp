#include "XC_Model.h"
#include "Basic/FileGroup.h"

XC_Model_TypeDef XC_Model[XC_MODEL_NUM];

static void Model_SetAttachCTRL(uint16_t index)
{
    if(index < XC_MODEL_NUM)
    {
        CTRL.CH_Config = &XC_Model[index].CH_Config;
        CTRL.RF_Config = &XC_Model[index].RF_Config;
        CTRL.State = &XC_Model[index].State;
    }
}

void Model_Copy(uint16_t dest_idx, uint16_t src_idx)
{
    if(dest_idx < XC_MODEL_NUM && src_idx < XC_MODEL_NUM)
    {
        XC_Model[dest_idx] = XC_Model[src_idx];
    }
}

void Model_Init()
{
    if(CTRL.ModelIndex >= XC_MODEL_NUM)
        CTRL.ModelIndex = 0;
    
    if(XC_Model[CTRL.ModelIndex].Name[0] == '\0')
    {
        X_CTRL_SetDefault();
    }
    
    Model_Update(CTRL.ModelIndex);
}

void Model_Update(uint8_t index)
{
    if(index >= XC_MODEL_NUM)
        return;
    
    CTRL.ModelIndex = index;
    Model_SetAttachCTRL(CTRL.ModelIndex);
}

static int16_t Model_GetEmptyIndex()
{
    for(int i = 0; i < XC_MODEL_NUM; i++)
    {
        if(XC_Model[i].Name[0] == '\0')
        {
            return i;
        }
    }
    return -1;
}

int16_t Model_GetNameIndex(const char* name)
{
    if(name[0] == '\0' || name == NULL)
        return -1;
    
    for(int i = 0; i < XC_MODEL_NUM; i++)
    {
        if(strcmp(name, XC_Model[i].Name) == 0)
        {
            return i;
        }
    }
    
    return -1;
}

void Model_SetDefault()
{
    memset(XC_Model, 0, sizeof(XC_Model));
    CTRL.ModelIndex = 0;
    Model_SetAttachCTRL(0);
}

const char* Model_GetCurrentName()
{
    if(CTRL.ModelIndex < XC_MODEL_NUM)
    {
        return XC_Model[CTRL.ModelIndex].Name;
    }
    return NULL;
}

bool Model_Add(const char* name)
{
    int16_t index = Model_GetNameIndex(name);
    if(index >= 0)
    {
        Model_Update(index);
        return true;
    }
    
    index = Model_GetEmptyIndex();
    
    if(index < 0)
        return false;
    
    Model_Copy(index, 0);
    strncpy(XC_Model[index].Name, name, sizeof(XC_Model[index].Name));
    return true;
}

bool Model_Delete(const char* name)
{
    int16_t index = Model_GetNameIndex(name);
    
    if(index < 0)
        return false;
    
    XC_Model[index].Name[0] = '\0';
    
    return true;
}
