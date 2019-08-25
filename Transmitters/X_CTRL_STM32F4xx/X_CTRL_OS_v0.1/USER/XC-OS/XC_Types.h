#ifndef __XC_TYPES_H
#define __XC_TYPES_H

#include <stdint.h>

namespace XC_Types
{

/*单字节对齐*/
#pragma pack (1)

typedef enum
{
    TYPE_Info,
    TYPE_Media,
    TYPE_Code
} TYPE_Property_t;

/*文件头信息*/
typedef struct
{
    TYPE_Property_t Type;
    uint16_t Version;
    char Author[20];
    char Date[20];
    char Time[20];
} BasicInfo_t;

/*文件信息*/
typedef struct
{
    TYPE_Property_t Type;
    char Name[20];
    uint32_t Size;
} File_t;

/*媒体文件信息*/
typedef struct
{
    uint8_t PictureNum;
    File_t Pictrue[20];
    uint8_t VideoNum;
    File_t Video[20];
} MediaInfo_t;

/*脚本文件信息*/
typedef struct
{
    uint8_t ScriptNum;
    File_t Script[20];
} ScriptInfo_t;

/*安装包信息*/
typedef struct
{
    BasicInfo_t Basic;
    MediaInfo_t Media;
    ScriptInfo_t Script;
    uint64_t CheckSum;
} XC_Package_t;

/*取消单字节对齐*/
#pragma pack ()

}

#endif
