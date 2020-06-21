#ifndef __XC_MODEL_H
#define __XC_MODEL_H

#include "Basic/XC_Type.h"

#define XC_MODEL_NUM      5
#define XC_MODEL_NAME_MAX 16

typedef struct{
    char                 Name[XC_MODEL_NAME_MAX];
    XC_CH_Config_TypeDef CH_Config;
    XC_RF_Config_TypeDef RF_Config;
    XC_State_TypeDef     State;
}XC_Model_TypeDef;

extern XC_Model_TypeDef XC_Model[XC_MODEL_NUM];

void Model_Init();
void Model_Update(uint8_t index);
void Model_SetDefault();
int16_t Model_GetNameIndex(const char* name);
const char* Model_GetCurrentName();
bool Model_Add(const char* name);
bool Model_Delete(const char* name);

#endif
