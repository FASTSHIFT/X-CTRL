#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "APP_Type.h"
#include "SdFat.h"
#include "LuaScript.h"
#include "TasksManage.h"
#include "Module.h"

#define APP_MAX 9
#define APP_NAME_MAX 20
#define APP_PATH_MAX (APP_NAME_MAX+30)

typedef struct{
    char path[APP_PATH_MAX];
    char text[APP_NAME_MAX];
}AppPathText_TypeDef;

AppPathText_TypeDef AppPathText_Grp[APP_MAX];
APP_TypeDef APP_Grp[APP_MAX];

static void APP_Call(const char *path);

static int LoadFolderInfo(const char *path)
{
    SdFile root, file;
    if (!root.open(path))
    {
        return 0;
    }

    int count = 0;
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden() && file.isDir())
        {
            char fileName[APP_NAME_MAX];
            file.getName(fileName, sizeof(fileName));
            
            char imgPath[APP_PATH_MAX];
            sprintf(imgPath, "S:%s/%s/%s.bin", path, fileName, fileName);
            
            strcpy(AppPathText_Grp[count].path, imgPath);
            strcpy(AppPathText_Grp[count].text, fileName);
            
            APP_Grp[count].img_dsc = AppPathText_Grp[count].path;
            APP_Grp[count].lable_text = AppPathText_Grp[count].text;
            APP_Grp[count].type = TYPE_FuncCall;
            APP_Grp[count].param = (int)APP_Call;
            
            count++;
        }
        file.close();
    }
    return count;
}  

static bool ExecuteLuaFile(const char* path)
{
    bool ret = false;
    SdFile file;
    if(file.open(path, O_RDONLY))
    {
        file.read(TextGetBuff(), TextGetSize());
        file.close();
        ret = true;
    }
    return ret;
}

static void APP_Call(const char *path)
{
    if(ExecuteLuaFile(path))
    {
        luaScript.registerStrOutput(NULL);
        LuaScriptStart(TextGetBuff());
    }
}

static void ImgbtnEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    /*单击事件*/
    if(event == LV_EVENT_CLICKED)
    {
        for(int i = 0; i < __Sizeof(APP_Grp); i++)
        {
            if(obj == APP_Grp[i].imgbtn && APP_Grp[i].type == TYPE_FuncCall)
            {
                String luaPath = String((const char*)APP_Grp[i].img_dsc);
                luaPath.replace("bin", "lua");
                typedef void(*void_func_t)(const char*);
                ((void_func_t)APP_Grp[i].param)(luaPath.c_str() + 2);
            }
        }
    }
}

static lv_obj_t * contApp;

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    contApp = lv_cont_create(appWindow, NULL);
    lv_cont_set_fit(contApp, LV_FIT_FLOOD);

    int count = LoadFolderInfo("/APP");
    taskENTER_CRITICAL();
    __LoopExecute(Creat_APP(APP_Grp[i], contApp, i, ImgbtnEvent_Handler), count);
    taskEXIT_CRITICAL();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_del_safe(&contApp);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_SubAPPs(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
