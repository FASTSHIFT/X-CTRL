#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "SdFat.h"
#include "LuaScript.h"
#include "Module.h"

static SdFile root;

static lv_obj_t * tabviewFm;
static lv_obj_t * tabDrive;
static lv_obj_t * tabFileList;
static lv_obj_t * listFiles;

static void Creat_Tabview(lv_obj_t** tabview)
{
    *tabview = lv_tabview_create(appWindow, NULL);
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(*tabview, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    tabDrive = lv_tabview_add_tab(*tabview, LV_SYMBOL_DRIVE);
    tabFileList = lv_tabview_add_tab(*tabview, LV_SYMBOL_LIST);
}

static void Creat_TabDrive(lv_obj_t * tab)
{
#define TO_MB(size) (size*0.000512f)
#define TO_GB(size) (TO_MB(size)/1024.0f)
    uint32_t cardSize = SD.card()->cardSize();
    uint32_t volFree = SD.vol()->freeClusterCount() * SD.vol()->blocksPerCluster();

    float totalSize = TO_GB(cardSize);
    float freeSize = TO_GB(volFree);
    float usageSize = totalSize - freeSize;

    /*LineMeter*/
    lv_obj_t * lmeter = lv_lmeter_create(tab, NULL);
    lv_obj_t * lmeterLabel = lv_label_create(lmeter, NULL);
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_lmeter_set_range(lmeter, 0, 100);
    lv_lmeter_set_scale(lmeter, 240, 31);

    int16_t dispUseage = usageSize / totalSize * 100.0f;
    lv_lmeter_set_value(lmeter, dispUseage);
    lv_label_set_text_format(lmeterLabel, "%d%%", dispUseage);

    static lv_style_t labelStyle = *lv_obj_get_style(lmeterLabel);
    labelStyle.text.font = &lv_font_roboto_28;
    lv_label_set_style(lmeterLabel, LV_LABEL_STYLE_MAIN, &labelStyle);

    lv_obj_align(lmeterLabel, lmeter, LV_ALIGN_CENTER, 0, 0);

    /*Size*/
    lv_obj_t * labelSD = lv_label_create(lmeter, NULL);
    lv_label_set_text_format(labelSD, "%0.2fGB / %0.2fGB", usageSize, totalSize);
    lv_obj_align(labelSD, lmeter, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

static lv_obj_t * mboxError;
#define mboxErrorAutoClose() 
static void mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        lv_mbox_start_auto_close(obj, 20);
    }
    if(event == LV_EVENT_CANCEL)
    {
        lv_mbox_start_auto_close(obj, 20);
    }
    if(event == LV_EVENT_DELETE)
    {
        mboxError = NULL;
    }
}

static bool MboxThorw(const char *text)
{
    if(mboxError)
        return false;
    
    MessageBox_Activate(
        true,
        tabFileList,
        &mboxError,
        200, 200,
        text,
        NULL,
        mbox_event_handler
    );
    
    return true;
}

/*File open way*/
static void OpenTextFile(const char * filename)
{
    SdFile file;
    if(file.open(filename, O_RDONLY))
    {
        if(file.available() < TextGetSize())
        {
            TextClear();
            file.read(TextGetBuff(), TextGetSize());
            LuaCodeSet(TextGetBuff());
            page.PagePush(PAGE_LuaScript);
        }
        else
        {
            char str[50];
            sprintf(str, "file size too large!\n(%0.2fKB > buffer size(%0.2fKB))", (float)file.available() / 1024.0f, TextGetSize() / 1024.0f);
            MboxThorw(str);
        }
        file.close();
    }
}

/************************File Analyzer*************************/
typedef enum{
    FT_UNKONWN,
    FT_IMG,
    FT_VIDEO,
    FT_AUDIO,
    FT_TEXT,
    FT_DIR
}File_Type;

typedef struct{
    File_Type type;
    const char* sym;
    String name;
}FileInfo_TypeDef;
FileInfo_TypeDef FileInfo_Grp[50];

/*Extname to sym*/
typedef struct{
    String extName;
    const char* sym;
    File_Type type;
}FileExtSym_TypeDef;
FileExtSym_TypeDef ExtSym_Grp[] = {
    {"",     LV_SYMBOL_FILE,   FT_UNKONWN},
    {".bv",  LV_SYMBOL_VIDEO,  FT_VIDEO},
    {".wav", LV_SYMBOL_AUDIO,  FT_AUDIO},
    {".lua", LV_SYMBOL_EDIT,   FT_TEXT},
    {".txt", LV_SYMBOL_EDIT,   FT_TEXT},
    {".html",LV_SYMBOL_EDIT,   FT_TEXT},
    {".log", LV_SYMBOL_EDIT,   FT_TEXT},
    {".png", LV_SYMBOL_IMAGE,  FT_IMG},
    {".jpg", LV_SYMBOL_IMAGE,  FT_IMG},
    {".gif", LV_SYMBOL_IMAGE,  FT_IMG},
};

static void ChangePath(lv_obj_t * tab, lv_obj_t** list, const char *newPath);
String NowFilePath = "/";
static int AccessFolder(bool enter = false, String *folder = NULL)
{
    int index;
    if(enter)
    {
        NowFilePath = NowFilePath + "/" + *folder;
    }
    else
    {
        index = NowFilePath.lastIndexOf('/');
        if(index > 0)
        {
            NowFilePath = NowFilePath.substring(0, index);
        }
    }
    ChangePath(tabFileList, &listFiles, NowFilePath.c_str());
    return index;
}

static int GetFileInfoIndex(const char* filename)
{
    String Name = String(filename);
    Name.toLowerCase();
    
    for(uint8_t i = 1; i < __Sizeof(ExtSym_Grp); i++)
    {
        if(Name.endsWith(ExtSym_Grp[i].extName))
        {
            return i;
        }
    }
    return 0;
}

static bool LoadNextFileInfo(SdFile &file, int index)
{
    if(index >= __Sizeof(FileInfo_Grp))
        return false;
    
    char fileName[50];
    file.getName(fileName, sizeof(fileName));
    FileInfo_Grp[index].name = String(fileName);
    
    if(file.isDir())
    {
        FileInfo_Grp[index].type = FT_DIR;
        FileInfo_Grp[index].sym = LV_SYMBOL_DIRECTORY;
    }
    else
    {
        int i = GetFileInfoIndex(fileName);
        FileInfo_Grp[index].type = ExtSym_Grp[i].type;
        FileInfo_Grp[index].sym = ExtSym_Grp[i].sym;
    }
    
    return true;
}

static void FileEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(mboxError)
        {
            lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
            return;
        }
        
        int index = lv_list_get_btn_index(listFiles, obj);
        
        if(FileInfo_Grp[index].type == FT_TEXT)
        {
            String path = NowFilePath + "/" + FileInfo_Grp[index].name;
            OpenTextFile(path.c_str());
        }
        else if(FileInfo_Grp[index].type == FT_DIR)
        {
            AccessFolder(true, &FileInfo_Grp[index].name);
        }
    }
}

static void Creat_TabFileList(lv_obj_t * tab, lv_obj_t** list, const char *path)
{
    if (!root.open(path))
    {
        return;
    }
    
    /*Create a list*/
    *list = lv_list_create(tab, NULL);
    lv_obj_set_size(*list, lv_obj_get_width_fit(tab) - 10, lv_obj_get_height_fit(tab) - 10);
    lv_obj_align(*list, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(*list, true);
    lv_list_set_edge_flash(*list, true);
    
    /*Loading files*/
    SdFile file;
    int index = 0;
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden())
        {
            if(!LoadNextFileInfo(file, index))
            {
                return;
            }

            lv_obj_t * list_btn = lv_list_add_btn(
                *list, 
                FileInfo_Grp[index].sym, 
                FileInfo_Grp[index].name.c_str()
            );
            index++;
        
            lv_btn_set_ink_in_time(list_btn, 200);
            lv_btn_set_ink_out_time(list_btn, 200);
            lv_obj_set_event_cb(list_btn, FileEvent_Handler);
        }
        file.close();
    }
}

static void ChangePath(lv_obj_t * tab, lv_obj_t** list, const char *newPath)
{
    memset(FileInfo_Grp, 0, sizeof(FileInfo_Grp));
    lv_obj_del_safe(&listFiles);
    root.close();
    Creat_TabFileList(tab, &(*list), newPath);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    __ExecuteOnce(Creat_Tabview(&tabviewFm));
    
    lv_obj_set_hidden(tabviewFm, false);
    ChangePath(tabFileList, &listFiles, NowFilePath.c_str()); 
    
    __ExecuteOnce((
        Preloader_Activate(true, tabDrive),
        Creat_TabDrive(tabDrive),
        Preloader_Activate(false, NULL)
    ));
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    //lv_obj_del_safe(&tabviewFm);  
    lv_obj_del_safe(&listFiles);
    lv_obj_set_hidden(tabviewFm, true);
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
            if(mboxError)
            {
                lv_event_send(mboxError, LV_EVENT_CANCEL, 0);
                return ;
            }

            if(root.isOpen())
            {
                AccessFolder();
            }
            else
            {
                page.PagePop();
            }
        }
    }
    else if(event == LV_EVENT_LONG_PRESSED_REPEAT)
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
void PageRegister_FileExplorer(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
