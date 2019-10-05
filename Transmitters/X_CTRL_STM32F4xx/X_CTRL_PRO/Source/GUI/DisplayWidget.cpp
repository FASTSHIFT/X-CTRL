#include "FileGroup.h"
#include "DisplayPrivate.h"

/***************************Preloader****************************/
static lv_obj_t * preloader = NULL;

static void Creat_Preloader(lv_obj_t * parent, lv_obj_t** preload)
{
    /*Create a Preloader object*/
    *preload = lv_preload_create(parent, NULL);
    lv_obj_set_size(*preload, 50, 50);
    lv_obj_align(*preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_preload_set_type(*preload, LV_PRELOAD_TYPE_FILLSPIN_ARC);
}

void Preloader_Activate(bool isact, lv_obj_t * parent)
{
    if(isact)
    {
        if(!preloader)
        {
            Creat_Preloader(parent, &preloader);
        }
    }
    else
    {
        lv_obj_del_safe(&preloader);
    }
}

/***************************Keyboard****************************/
static void Creat_Keyboard(lv_obj_t** kb, lv_obj_t * parent, lv_obj_t * ta, lv_event_cb_t keyboard_event_cb)
{
    *kb = lv_kb_create(parent, NULL);
//    lv_obj_set_size(*kb, lv_obj_get_width_fit(parent), lv_obj_get_height_fit(parent) / 2);
    lv_obj_set_size(*kb, APP_WIN_WIDTH, APP_WIN_HEIGHT / 2);
    lv_obj_align(*kb, barNavigation, LV_ALIGN_OUT_TOP_MID, 0, 0);
    
    if(ta)
    {
        lv_kb_set_ta(*kb, ta);
    }
    
    if(keyboard_event_cb)
    {
        lv_obj_set_event_cb(*kb, keyboard_event_cb);
    }

    static lv_anim_t a;
    a.var = *kb;
    a.start = LV_VER_RES;
    a.end = lv_obj_get_y(*kb);
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.time = 300;
    lv_anim_create(&a);
}

static void kb_hide_anim_end(lv_anim_t * a)
{
    lv_obj_del((lv_obj_t*)a->var);
}

void Keyboard_Activate(
    lv_obj_t** kb, 
    bool isact, 
    lv_obj_t * parent,
    lv_obj_t * ta, 
    lv_event_cb_t keyboard_event_cb
)
{
    if(isact)
    {
        if(!(*kb))
        {
            Creat_Keyboard(&(*kb), parent, ta, keyboard_event_cb);
        }
    }
    else
    {
        if(*kb)
        {
            static lv_anim_t a;
            a.var = *kb;
            a.start = lv_obj_get_y(*kb);
            a.end = LV_VER_RES;
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
            a.path_cb = lv_anim_path_linear;
            a.ready_cb = kb_hide_anim_end;
            a.time = 300;
            lv_anim_create(&a);
        }
    }
}

/***************************MessageBox****************************/

void MessageBox_Activate(
    bool isact, 
    lv_obj_t * parent, 
    lv_obj_t** mbox,
    lv_coord_t w, lv_coord_t h,
    const char *text,
    const char** btns,
    lv_event_cb_t mbox_event_handler
)
{
    if(isact)
    {
        *mbox = lv_mbox_create(parent ? parent : lv_scr_act(), NULL);
        if(text)
        {
            lv_mbox_set_text(*mbox, text);
        }
        if(btns)
        {
            lv_mbox_add_btns(*mbox, btns);
        }
        
        if(mbox_event_handler)
        {
            lv_obj_set_event_cb(*mbox, mbox_event_handler);
        }
        
        lv_obj_set_drag(*mbox, true);
        lv_obj_set_size(*mbox, w, h);
        
        lv_obj_align(*mbox, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_auto_realign(*mbox, true);
    }
    else
    {
        lv_obj_del_safe(&(*mbox));
    }
}
