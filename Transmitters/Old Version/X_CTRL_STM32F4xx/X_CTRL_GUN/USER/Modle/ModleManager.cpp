#include "FileGroup.h"
#include "ModlePrivate.h"
#include "EEPROM_File.h"
#include "RCX/crc.h"

typedef struct
{
    char Name[32];
    uint8_t Type;
    uint8_t ID;
    int16_t ChannelMap[32];
    uint8_t CRC8;
} ModelData_TypeDef;

ModelData_TypeDef Model;
static EEPROM_File ModelFile;

#define MDDEL_PATH "/"_X_CTRL_NAME"/model"

void Modle_Save()
{
    uint8_t* pData = (uint8_t*)&Model;
    for(int i = 0; i < sizeof(Model); i++)
    {
        ModelFile.update(i, pData[i]);
    }
}

bool Model_Creat(const char* name, uint8_t type, uint8_t id)
{
    ModelFile.begin(MDDEL_PATH, name, sizeof(Model));
    strncpy(Model.Name, name, sizeof(Model.Name));
    Model.Type = type;
    Model.ID = id;
    Modle_Save();
    ModelFile.end();
    return true;
}

void Model_UpdateChannel()
{
    
}

bool Model_Load(const char* name)
{
    bool retval = false;
    
    /*路径是否合法*/
    SdFile root;
    if (!root.open(MDDEL_PATH))
        return false;

    /*文件目录*/
    SdFile file;

    /*遍历目录所有项*/
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden())
        {
            char fileName[32];
            file.getName(fileName, sizeof(fileName));
            if(strcmp(fileName, name) == 0)
            {
                file.read(&Model, sizeof(Model));
                retval = true;
            }
        }
        file.close();
    }

    root.close();
    return retval;
}
