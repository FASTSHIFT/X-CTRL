#include "FileGroup.h"
#include "FileSystem.h"
#include "DisplayPrivate.h"

/*实例化当前页面调度器*/
static MillisTaskManager mtm_FileExplorer(2);

/*文件总数量*/
int16_t FileCnt = 0;

/*文件根路径*/
static SdFile root;

/*文件打开成功*/
static bool FileMenuOpened = false;

/*支持打开的文件列表*/
typedef struct
{
    String ExtName;
    uint8_t PageID;
    String& LoaderPath;
} FileSupport_TypeDef;

static FileSupport_TypeDef FileSptGrp[] =
{
    {".bv",  PAGE_BvPlayer,  BvFilePath},
    {".wav", PAGE_WavPlayer, WavFilePath},
    {".lua", PAGE_LuaScript, LuaFilePath}
};

/*菜单管理器*/
MenuManager* FileMenu = NULL;

/*目录坐标*/
#define ItemStartY (StatusBar_Height+4)
#define ItemStartX (TEXT_WIDTH_1+8)
#define ItemEndX   (screen.width()-35)

/*当前选项显示的位置*/
static int16_t ItemDispalyPos;

/*实例化滚动条控件对象*/
/*ScrollBar(T *obj, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t dir, uint16_t l);*/
static LightGUI::ScrollBar<SCREEN_CLASS> Scroll(
    &screen,
    screen.width() + 5,
    StatusBar_Height + 4,
    4,
    0,
    1,
    0
);

/*实例化光标控件对象*/
static LightGUI::Cursor<SCREEN_CLASS> ItemCursor(&screen, ItemStartX, screen.height(), 8, 8);

/**
  * @brief  清除指定的选项区域
  * @param  pos:指定的选项区域
  * @retval 无
  */
static void ClearItemPos(uint16_t pos)
{
    screen.fillRect(
        ItemStartX,
        ItemStartY + pos * TEXT_HEIGHT_2,
        Scroll.X - ItemStartX,
        TEXT_HEIGHT_2,
        screen.Black
    );
}

/**
  * @brief  更新光标位置
  * @param  无
  * @retval 无
  */
static void UpadateCursor()
{
    ItemDispalyPos = FileMenu->GetItemDispalyPos();
}

/**
  * @brief  更新选项顶部
  * @param  无
  * @retval 无
  */
static void UpdateItemTop()
{
    FileMenu->UpdateItemStrBuffer();
    Scroll.setScroll(1.0f - ((float)FileMenu->ItemTop / (float)(FileMenu->ItemNum_Max - FileMenu->ItemNumDisplay_Max)));
    screen.setTextColor(screen.White, screen.Black);
    for(int16_t i = 0; i < FileMenu->ItemNumDisplay_Max; i++)
    {
        ClearItemPos(i);
        screen.setCursor(ItemStartX, ItemStartY + i * TEXT_HEIGHT_2);
        screen.printfX(FileMenu->ItemStrBuffer[i]);

        int16_t nowItemNum = (FileMenu->ItemTop + i) % FileMenu->ItemNum_Max;
        if(FileMenu->ItemList[nowItemNum].Type == FileMenu->TYPE_PageJump)
        {
            screen.setCursor(ItemEndX + 6, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(">");
        }
    }
}

/**
  * @brief  更新指定行选项
  * @param  pos:行
  * @retval 无
  */
static void UpdateItemPos(uint8_t pos)
{
    FileMenu->UpdateItemStrBuffer();
    screen.setTextColor(screen.White, screen.Black);
    ClearItemPos(pos);
    screen.setCursor(ItemStartX, ItemStartY + pos * TEXT_HEIGHT_2);
    screen.printfX(FileMenu->ItemStrBuffer[pos]);
}

/**
  * @brief  选项被选中事件
  * @param  无
  * @retval 无
  */
static void When_ItemSelect()
{
    String filePath = FileMenu->ItemList[FileMenu->ItemSelect].LabelText;
    /*遍历列表*/
    for(int i = 0; i < __Sizeof(FileSptGrp); i++)
    {
        /*全转为小写*/
        filePath.toLowerCase();
        /*寻找是否有对应的扩展名*/
        if(filePath.endsWith(FileSptGrp[i].ExtName))
        {
            /*将文件路径赋值给对应的加载器路径*/
            FileSptGrp[i].LoaderPath = filePath;
        }
    }
}

/**
  * @brief  菜单更新任务
  * @param  无
  * @retval 无
  */
static void Task_MenuUpdate()
{
    if(FileMenu->ItemSelectUpdating)
    {
        UpadateCursor();
        FileMenu->ItemSelectUpdating = false;
    }

    if(FileMenu->ItemTopUpdating)
    {
        UpdateItemTop();
        FileMenu->ItemTopUpdating = false;
    }

    if(FileMenu->ItemXUpdating)
    {
        UpdateItemPos(ItemDispalyPos);
        FileMenu->ItemXUpdating = false;
    }
}

/*当前光标位置*/
static float CursorPosNow = ItemStartY;

/*上一次的光标位置*/
static float CursorPosLast = ItemStartY + FileMenu->ItemNumDisplay_Max * 10;

/**
  * @brief  光标位置更新任务
  * @param  无
  * @retval 无
  */
static void Task_DrawCursorUpdate()
{
    CursorPosNow = ItemStartY + ItemDispalyPos * TEXT_HEIGHT_2 + 5;

    if((CursorPosNow - CursorPosLast) != 0)
    {
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.4f;
    }

    ItemCursor.setPosition(ItemStartX - 8, CursorPosLast);
}

/**
  * @brief  自定义光标回调函数
  * @param  无
  * @retval 无
  */
static void DrawCursor_Callback()
{
    screen.setTextColor(screen.Yellow, screen.Black);
    screen.setCursor(ItemCursor.X, ItemCursor.Y);
    screen.print('>');
}

static uint8_t GetFilePage(String filename)
{
    /*遍历列表*/
    for(int i = 0; i < __Sizeof(FileSptGrp); i++)
    {
        filename.toLowerCase();
        /*寻找是否有对应的扩展名*/
        if(filename.endsWith(FileSptGrp[i].ExtName))
        {
            /*返回页面ID*/
            return FileSptGrp[i].PageID;
        }
    }
    return 0;
}

static bool LoadFileDir(const char *path)
{
    /*路径是否合法*/
    if (!root.open(path))
        return false;
    
    if(FileMenu)
        return true;

    /*获取当前目录下文件个数*/
    FileCnt = 0;
    /*文件目录*/
    SdFile file;

    /*遍历目录所有项*/
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden())
        {
            FileCnt ++;
        }
        file.close();
    }

    /*创建菜单对象*/
    static MenuManager menu = MenuManager(
        FileCnt,
        (screen.height() - StatusBar_Height) / TEXT_HEIGHT_2 - 1,
        &page
    );
    FileMenu = &menu;

    /*关闭自动序号*/
    FileMenu->ItemAutoSquence = false;
    /*滚动条自适应*/
    Scroll.Height = FileMenu->ItemNumDisplay_Max * TEXT_HEIGHT_2 + 2;
    Scroll.Long = FileMenu->ItemNumDisplay_Max * TEXT_HEIGHT_2 * (FileMenu->ItemNumDisplay_Max / (float)FileMenu->ItemNum_Max);

    /*菜单选项选中事件绑定*/
    FileMenu->CallbackFunc_ItemSelectEvent = When_ItemSelect;
    /*菜单选项注册*/
    root.rewind();
    dir_t FileDir;
    for(int i = 0; i < FileCnt; i++)
    {
        /*更新文件信息*/
        root.readDir(&FileDir);

        /*获取文件名*/
        char name[50];
        root.dirName(&FileDir, name);

        /*可打开的文件扩展名判断*/
        String fileName = String(name);
        uint8_t PAGE_ID = 0;
        if((PAGE_ID = GetFilePage(fileName)) != 0)
        {
            FileMenu->UpdateItem(i, fileName, FileMenu->TYPE_PageJump, PAGE_ID);
        }
        else
        {
            FileMenu->UpdateItem(i, fileName);
        }
    }
    root.close();
    return true;
}

/********** 基本 ************/
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    FileMenuOpened = false;
    if(!SD_GetReady())
        return;
    
    TextSetDefault();
    TextMidPrint(0.5f, 0.5f, "LOADIND...");
    FileMenuOpened = LoadFileDir("/");
    
    if(!FileMenuOpened)
        return;

    CursorPosNow = ItemStartY;
    CursorPosLast = screen.height();
    Scroll.Color_FM = Scroll.Color_SB = screen.White;

    if(page.LastPage == PAGE_Options)
    {
        ItemDispalyPos = FileMenu->ItemSelect = FileMenu->ItemTop = 0;
        FileMenu->ItemXUpdating = FileMenu->ItemSelectUpdating = false;
    }
    FileMenu->ItemTopUpdating = true;
    FileMenu->UpdateItemStrBuffer();

    ClearPage();

    for(uint8_t i = 0; i < FileMenu->ItemNumDisplay_Max; i++)
    {
        for(int16_t j = -20; j < ItemStartX - 2; j++)
        {
            screen.setOpacityX(__Map(j, -20, ItemStartX - 3, 0, 255));
            screen.setTextColor(screen.White, screen.Black);
            screen.setCursor(j, ItemStartY + i * TEXT_HEIGHT_2);
            screen.printfX(" " + FileMenu->ItemStrBuffer[i]);
            screen.setOpacityX(255);
            PageDelay(1);
        }
        BuzzTone(1000 + 100 * i, 5);
        PageDelay(10);
    }

    Scroll.setScroll(1.0);
    for(uint8_t i = screen.width() + 5; i > screen.width() - 7; i--)
    {
        Scroll.setPosition(i, Scroll.Y);
        PageDelay(10);
    }

    /*注册自定义光标回调函数*/
    ItemCursor.DisplayCallbackFunction = DrawCursor_Callback;

    /*注册任务*/
    mtm_FileExplorer.TaskRegister(0, Task_MenuUpdate, 50);
    mtm_FileExplorer.TaskRegister(1, Task_DrawCursorUpdate, 20);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    if(!FileMenuOpened)
    {
        page.PagePop();
        return;
    }
    
    mtm_FileExplorer.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    root.close();
    /*滚动条向左滑动动画*/
    for(float i = 1.0f; i > 0.0f; i -= ((i * 0.01f) + 0.001f))
    {
        Scroll.Color_FM = Scroll.Color_SB = StrPrinter_ColorMap(screen.Black, screen.White, (uint8_t)(i * 15));
        Scroll.setPosition(screen.width()*i, Scroll.Y);
        PageDelay(2);
    }
    ClearPage();
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(FileMenu == NULL && param != &btBACK)
    {
        return;
    }
    
    if(event == EVENT_ButtonPress)
    {
        if(btUP)
        {
            FileMenu->SelectMove(+1);//菜单光标下移
        }
        if(btDOWN)
        {
            FileMenu->SelectMove(-1);//菜单光标上移
        }

        if(btOK || btEcd)
        {
            FileMenu->ItemSelectEvent();//触发选项被选中事件
        }

        if(btBACK)
        {
            page.PagePop();
        }
    }
    if(event == EVENT_ButtonLongPressRepeat)
    {
        if(btUP)
        {
            FileMenu->SelectMove(+1);//菜单光标下移
        }
        if(btDOWN)
        {
            FileMenu->SelectMove(-1);//菜单光标上移
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
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
