/**
 * @file lv_settings.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_settings.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SETTINGS_ANIM_TIME   300 /*[ms]*/
#define LV_SETTINGS_MAX_WIDTH   250

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_btn_ext_t btn;
    lv_settings_item_t * item;
    lv_event_cb_t event_cb;
}root_ext_t;

typedef struct
{
    lv_btn_ext_t btn;
    lv_settings_item_t * item;
}list_btn_ext_t;

typedef struct
{
    lv_cont_ext_t cont;
    lv_settings_item_t * item;
}item_cont_ext_t;

typedef struct
{
    lv_cont_ext_t cont;
    lv_event_cb_t event_cb;
    lv_obj_t * menu_page;
}menu_cont_ext_t;

typedef struct {
    lv_event_cb_t event_cb;
    lv_settings_item_t * item;
}histroy_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_page(lv_settings_item_t * parent_item, lv_event_cb_t event_cb);

static void add_list_btn(lv_obj_t * page, lv_settings_item_t * item);
static void add_btn(lv_obj_t * page, lv_settings_item_t * item);
static void add_sw(lv_obj_t * page, lv_settings_item_t * item);
static void add_ddlist(lv_obj_t * page, lv_settings_item_t * item);
static void add_numset(lv_obj_t * page, lv_settings_item_t * item);
static void add_slider(lv_obj_t * page, lv_settings_item_t * item);

static void refr_list_btn(lv_settings_item_t * item);
static void refr_btn(lv_settings_item_t * item);
static void refr_sw(lv_settings_item_t * item);
static void refr_ddlist(lv_settings_item_t * item);
static void refr_numset(lv_settings_item_t * item);
static void refr_slider(lv_settings_item_t * item);

static void root_event_cb(lv_obj_t * btn, lv_event_t e);
static void list_btn_event_cb(lv_obj_t * btn, lv_event_t e);
static void slider_event_cb(lv_obj_t * slider, lv_event_t e);
static void sw_event_cb(lv_obj_t * sw, lv_event_t e);
static void btn_event_cb(lv_obj_t * btn, lv_event_t e);
static void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t e);
static void numset_event_cb(lv_obj_t * btn, lv_event_t e);

static void header_back_event_cb(lv_obj_t * btn, lv_event_t e);
static lv_obj_t * item_cont_create(lv_obj_t * page, lv_settings_item_t * item);
static void old_cont_del_cb(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * act_cont;
static lv_obj_t * menu_btn;
static lv_style_t style_menu_bg;
static lv_style_t style_bg;
static lv_style_t style_item_cont;
static lv_ll_t history_ll;
static lv_group_t * group;
static lv_coord_t settings_max_width = LV_SETTINGS_MAX_WIDTH;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a settings application
 * @param root_item descriptor of the settings button. For example:
 * `lv_settings_menu_item_t root_item = {.name = "Settings", .event_cb = root_event_cb};`
 * @return the created settings button
 */
lv_obj_t * lv_settings_create(lv_settings_item_t * root_item, lv_event_cb_t event_cb)
{
    lv_theme_t *th = lv_theme_get_current();
    if(th) {
        lv_style_copy(&style_menu_bg, th->style.cont);
        lv_style_copy(&style_item_cont, th->style.list.btn.rel);
    } else {
        lv_style_copy(&style_menu_bg, &lv_style_pretty);
        lv_style_copy(&style_item_cont, &lv_style_transp);
    }

    lv_style_copy(&style_bg, &lv_style_transp_tight);
    style_menu_bg.body.radius = 0;

    style_item_cont.body.padding.left = LV_DPI / 5;
    style_item_cont.body.padding.right = LV_DPI / 5;
    style_item_cont.body.padding.top = LV_DPI / 10;
    style_item_cont.body.padding.bottom = LV_DPI / 10;
    style_item_cont.body.padding.inner = LV_DPI / 20;


    menu_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_fit(menu_btn, LV_FIT_TIGHT);
    root_ext_t * ext = lv_obj_allocate_ext_attr(menu_btn, sizeof(root_ext_t));
    ext->item = root_item;
    ext->event_cb = event_cb;

    lv_obj_set_event_cb(menu_btn, root_event_cb);
    if(group) lv_group_add_obj(group, menu_btn);

    lv_obj_t * menu_label = lv_label_create(menu_btn, NULL);
    lv_label_set_text(menu_label, LV_SYMBOL_LIST);

    lv_obj_set_pos(menu_btn, 0, 0);

    lv_ll_init(&history_ll, sizeof(histroy_t));

    return menu_btn;
}

/**
 * Automatically add the item to a group to allow navigation with keypad or encoder.
 * Should be called before `lv_settings_create`
 * The group can be change at any time.
 * @param g the group to use. `NULL` to not use this feature.
 */
void lv_settings_set_group(lv_group_t * g)
{
    group = g;
}

/**
 * Change the maximum width of settings dialog object
 * @param max_width maximum width of the settings container page
 */
void lv_settings_set_max_width(lv_coord_t max_width)
{
    settings_max_width = max_width;
}

/**
 * Create a new page ask `event_cb` to add the item with `LV_EVENT_REFRESH`
 * @param parent_item pointer to an item which open the the new page. Its `name` will be the title
 * @param event_cb event handler of the menu page
 */
void lv_settings_open_page(lv_settings_item_t * parent_item, lv_event_cb_t event_cb)
{
    /*Create a new page in the menu*/
    create_page(parent_item, event_cb);

    /*Add the items*/
    lv_event_send_func(event_cb, NULL, LV_EVENT_REFRESH, parent_item);
}

/**
 * Add a list element to the page. With `item->name` and `item->value` texts.
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings_add(lv_settings_item_t * item)
{
    if(act_cont == NULL) return;

    menu_cont_ext_t * ext = lv_obj_get_ext_attr(act_cont);
    lv_obj_t * page = ext->menu_page;


    switch(item->type) {
    case LV_SETTINGS_TYPE_LIST_BTN: add_list_btn(page, item); break;
    case LV_SETTINGS_TYPE_BTN:      add_btn(page, item); break;
    case LV_SETTINGS_TYPE_SLIDER:   add_slider(page, item); break;
    case LV_SETTINGS_TYPE_SW:       add_sw(page, item); break;
    case LV_SETTINGS_TYPE_DDLIST:   add_ddlist(page, item); break;
    case LV_SETTINGS_TYPE_NUMSET:   add_numset(page, item); break;
    default: break;
    }
}

/**
 * Refresh an item's name value and state.
 * @param item pointer to a an `lv_settings_item _t` item.
 */
void lv_settings_refr(lv_settings_item_t * item)
{
    /*Return if there is nothing to refresh*/
    if(item->cont == NULL) return;

    switch(item->type) {
    case LV_SETTINGS_TYPE_LIST_BTN: refr_list_btn(item); break;
    case LV_SETTINGS_TYPE_BTN:      refr_btn(item); break;
    case LV_SETTINGS_TYPE_SLIDER:   refr_slider(item); break;
    case LV_SETTINGS_TYPE_SW:       refr_sw(item); break;
    case LV_SETTINGS_TYPE_DDLIST:   refr_ddlist(item); break;
    case LV_SETTINGS_TYPE_NUMSET:   refr_numset(item); break;
    default: break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Create a new settings container with header, hack button ,title and an empty page
 * @param item pointer to a an `lv_settings_item_t` item.
 * `item->name` will be the title of the page.
 * `LV_EVENT_REFRESH` will be sent to `item->event_cb` to create the page again when the back button is pressed.
 */
static void create_page(lv_settings_item_t * parent_item, lv_event_cb_t event_cb)
{
    lv_coord_t w = LV_MATH_MIN(lv_obj_get_width(lv_scr_act()), settings_max_width);

    lv_obj_t * old_menu_cont = act_cont;

    act_cont = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_style(act_cont, LV_CONT_STYLE_MAIN, &style_menu_bg);
    lv_obj_set_size(act_cont, w, lv_obj_get_height(lv_scr_act()));

    menu_cont_ext_t * ext = lv_obj_allocate_ext_attr(act_cont, sizeof(menu_cont_ext_t));
    ext->event_cb = event_cb;
    ext->menu_page = NULL;

    lv_obj_t * header = lv_cont_create(act_cont, NULL);
    lv_cont_set_style(header, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);
    lv_cont_set_fit2(header, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(header, lv_obj_get_width(act_cont));

    lv_obj_t * header_back_btn = lv_btn_create(header, NULL);
    lv_btn_set_fit(header_back_btn, LV_FIT_TIGHT);
    lv_obj_set_event_cb(header_back_btn, header_back_event_cb);
    if(group) lv_group_add_obj(group, header_back_btn);
    lv_group_focus_obj(header_back_btn);

    lv_obj_t * header_back_label = lv_label_create(header_back_btn, NULL);

    lv_obj_t * header_title = lv_label_create(header, NULL);
    lv_label_set_text(header_title, parent_item->name);

    bool menu_btn_right = lv_obj_get_x(menu_btn) >= lv_obj_get_width(lv_scr_act())/2;

    if(!menu_btn_right) {
        lv_cont_set_layout(header, LV_LAYOUT_ROW_M);
        lv_label_set_text(header_back_label, LV_SYMBOL_LEFT);
    } else {
        lv_label_set_text(header_back_label, LV_SYMBOL_RIGHT);
        lv_obj_align(header_back_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
        lv_obj_align(header_title, header_back_btn, LV_ALIGN_OUT_LEFT_MID, -act_cont->style_p->body.padding.right, 0);
    }

    lv_obj_set_pos(header, 0, 0);

    lv_obj_t * page = lv_page_create(act_cont, NULL);
    lv_page_set_style(page, LV_PAGE_STYLE_BG, &style_bg);
    lv_page_set_style(page, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);
    lv_page_set_scrl_layout(page, LV_LAYOUT_COL_M);
    lv_list_set_edge_flash(page, true);
    lv_obj_set_size(page, lv_obj_get_width(act_cont), lv_obj_get_height(lv_scr_act()) - lv_obj_get_height(header));
    lv_obj_align(page, header, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    ext->menu_page = page;

    histroy_t * new_node = lv_ll_ins_head(&history_ll);
    new_node->event_cb = event_cb;
    new_node->item = parent_item;

    /*Delete the old menu container after some time*/
    if(old_menu_cont) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, old_menu_cont, NULL);
        lv_anim_set_values(&a, 0, 1);
        lv_anim_set_path_cb(&a, lv_anim_path_step);
        lv_anim_set_time(&a, LV_SETTINGS_ANIM_TIME, 0);
        lv_anim_set_ready_cb(&a, old_cont_del_cb);
        lv_anim_create(&a);
    }

    /*Float in the new menu*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, act_cont, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_coord_t w_cont = lv_obj_get_width(act_cont);
    lv_coord_t w_scr = lv_obj_get_width(lv_scr_act());
    uint32_t start = !menu_btn_right ? -w_cont : w_scr;
    uint32_t end = !menu_btn_right ? 0 : w_scr-w_cont;
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, LV_SETTINGS_ANIM_TIME, 0);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_create(&a);
}

/**
 * Add a list element to the page. With `item->name` and `item->value` texts.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_list_btn(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * liste = lv_btn_create(page, NULL);
    lv_btn_set_layout(liste, LV_LAYOUT_COL_L);
    lv_btn_set_fit2(liste, LV_FIT_FLOOD, LV_FIT_TIGHT);
    lv_page_glue_obj(liste, true);
    lv_obj_set_event_cb(liste, list_btn_event_cb);
    if(group) lv_group_add_obj(group, liste);

    list_btn_ext_t * ext = lv_obj_allocate_ext_attr(liste, sizeof(list_btn_ext_t));
    ext->item = item;
    ext->item->cont = liste;

    lv_obj_t * name = lv_label_create(liste, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(liste, NULL);
    lv_label_set_text(value, item->value);

    lv_theme_t * th = lv_theme_get_current();
    if(th) {
        lv_btn_set_style(liste, LV_BTN_STYLE_REL, th->style.list.btn.rel);
        lv_btn_set_style(liste, LV_BTN_STYLE_PR, th->style.list.btn.pr);
        lv_label_set_style(value, LV_LABEL_STYLE_MAIN, th->style.label.hint);
    }
}


/**
 * Create a button. Write `item->name` on create a button on the right with `item->value` text.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_btn(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * btn = lv_btn_create(cont, NULL);
    lv_btn_set_fit(btn, LV_FIT_TIGHT);
    lv_obj_set_event_cb(btn, btn_event_cb);
    if(group) lv_group_add_obj(group, btn);

    lv_obj_t * value = lv_label_create(btn, NULL);
    lv_label_set_text(value, item->value);

    lv_obj_align(btn, NULL, LV_ALIGN_IN_RIGHT_MID, -style_item_cont.body.padding.right, 0);
    lv_obj_align(name, NULL, LV_ALIGN_IN_LEFT_MID, style_item_cont.body.padding.left, 0);
}

/**
 * Create a switch with `item->name` text. The state is set from `item->state`.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_sw(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(cont, NULL);
    lv_label_set_text(value, item->value);

    lv_theme_t * th = lv_theme_get_current();
    if(th) {
        lv_label_set_style(value, LV_LABEL_STYLE_MAIN, th->style.label.hint);
    }

    lv_obj_t * sw = lv_sw_create(cont, NULL);
    lv_obj_set_event_cb(sw, sw_event_cb);
    lv_obj_set_size(sw, LV_DPI / 2, LV_DPI / 4);
    if(item->state) lv_sw_on(sw, LV_ANIM_OFF);
    if(group) lv_group_add_obj(group, sw);

    lv_obj_align(name, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left, style_item_cont.body.padding.top);
    lv_obj_align(value, name, LV_ALIGN_OUT_BOTTOM_LEFT, 0, style_item_cont.body.padding.inner);
    lv_obj_align(sw, NULL, LV_ALIGN_IN_RIGHT_MID, -style_item_cont.body.padding.right, 0);
}

/**
 * Create a drop down list with `item->name` title and `item->value` options. The `item->state` option will be selected.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_ddlist(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->name);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left, style_item_cont.body.padding.top);

    lv_obj_t * ddlist = lv_ddlist_create(cont, NULL);
    lv_ddlist_set_options(ddlist, item->value);
    lv_ddlist_set_draw_arrow(ddlist, true);
    lv_ddlist_set_fix_height(ddlist, lv_obj_get_height(page) / 2);
    lv_ddlist_set_fix_width(ddlist, lv_obj_get_width_fit(cont));
    lv_obj_align(ddlist, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, style_item_cont.body.padding.inner);
    lv_obj_set_event_cb(ddlist, ddlist_event_cb);
    lv_ddlist_set_selected(ddlist, item->state);
    if(group) lv_group_add_obj(group, ddlist);
}

/**
 * Create a drop down list with `item->name` title and `item->value` options. The `item->state` option will be selected.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_numset(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY);

    lv_obj_t * label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->name);
    lv_obj_set_protect(label, LV_PROTECT_FOLLOW);


    lv_obj_t * btn_dec = lv_btn_create(cont, NULL);
    lv_obj_set_size(btn_dec, LV_DPI / 2, LV_DPI / 2);
    lv_obj_set_event_cb(btn_dec, numset_event_cb);
    if(group) lv_group_add_obj(group, btn_dec);

    label = lv_label_create(btn_dec, NULL);
    lv_label_set_text(label, LV_SYMBOL_MINUS);

    label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->value);

    lv_obj_t * btn_inc = lv_btn_create(cont, btn_dec);
    lv_obj_set_size(btn_inc, LV_DPI / 2, LV_DPI / 2);
    if(group) lv_group_add_obj(group, btn_inc);

    label = lv_label_create(btn_inc, NULL);
    lv_label_set_text(label, LV_SYMBOL_PLUS);

}

/**
 * Create a slider with 0..256 range. Write `item->name` and `item->value` on top of the slider. The current value is loaded from `item->state`
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
static void add_slider(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(cont, NULL);
    lv_label_set_text(value, item->value);
    lv_obj_set_auto_realign(value, true);

    lv_obj_align(name, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left,
                                                   style_item_cont.body.padding.top);
    lv_obj_align(value, NULL, LV_ALIGN_IN_TOP_RIGHT, -style_item_cont.body.padding.right,
                                                      style_item_cont.body.padding.top);
    lv_obj_t * slider = lv_slider_create(cont, NULL);
    lv_obj_set_size(slider, lv_obj_get_width_fit(cont), LV_DPI / 4);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_MID, 0, lv_obj_get_y(name) +
                                                       lv_obj_get_height(name) +
                                                       style_item_cont.body.padding.inner);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 256);
    lv_slider_set_value(slider, item->state, LV_ANIM_OFF);
    if(group) lv_group_add_obj(group, slider);
}

static void refr_list_btn(lv_settings_item_t * item)
{
    lv_obj_t * name = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, name);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}

static void refr_btn(lv_settings_item_t * item)
{
    lv_obj_t * btn = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * name = lv_obj_get_child(item->cont, btn);
    lv_obj_t * value = lv_obj_get_child(btn, NULL);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}


static void refr_sw(lv_settings_item_t * item)
{
    lv_obj_t * sw = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, sw);
    lv_obj_t * name = lv_obj_get_child(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);

    bool tmp_state = lv_sw_get_state(sw) ? true : false;
    if(tmp_state != item->state) {
        if(tmp_state == false) lv_sw_off(sw, LV_ANIM_OFF);
        else lv_sw_on(sw, LV_ANIM_OFF);
    }
}

static void refr_ddlist(lv_settings_item_t * item)
{
    lv_obj_t * name = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * ddlist = lv_obj_get_child(item->cont, name);

    lv_label_set_text(name, item->name);

    lv_ddlist_set_options(ddlist, item->value);

    lv_ddlist_set_selected(ddlist, item->state);
}

static void refr_numset(lv_settings_item_t * item)
{
    lv_obj_t * name = lv_obj_get_child_back(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child_back(item->cont, name); /*It's the minus button*/
    value = lv_obj_get_child_back(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}

static void refr_slider(lv_settings_item_t * item)
{
    lv_obj_t * slider = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, slider);
    lv_obj_t * name = lv_obj_get_child(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);

    if(lv_slider_get_value(slider) != item->state) lv_slider_set_value(slider, item->state, LV_ANIM_OFF);
}

static void root_event_cb(lv_obj_t * btn, lv_event_t e)
{

    if(e == LV_EVENT_CLICKED) {
        root_ext_t * ext = lv_obj_get_ext_attr(btn);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(ext->event_cb, NULL, e,  ext->item);
    }
}

/**
 * List button event callback. The following events are sent:
 * - `LV_EVENT_CLICKED`
 * - `LV_EEVNT_SHORT_CLICKED`
 * - `LV_EEVNT_LONG_PRESSED`
 * @param btn pointer to the back button
 * @param e the event
 */
static void list_btn_event_cb(lv_obj_t * btn, lv_event_t e)
{
    /*Save the menu item because the button will be deleted in `menu_cont_create` and `ext` will be invalid */
    list_btn_ext_t * item_ext = lv_obj_get_ext_attr(btn);

    if(e == LV_EVENT_CLICKED ||
       e == LV_EVENT_SHORT_CLICKED ||
       e == LV_EVENT_LONG_PRESSED) {
        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Slider event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED`,
 * save the state and refresh the value label.
 * @param slider pointer to the slider
 * @param e the event
 */
static void slider_event_cb(lv_obj_t * slider, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(slider);
    item_cont_ext_t * item_ext = lv_obj_get_ext_attr(cont);

    if(e == LV_EVENT_VALUE_CHANGED) {
        item_ext->item->state = lv_slider_get_value(slider);
        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Switch event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED` and save the state.
 * @param sw pointer to the switch
 * @param e the event
 */
static void sw_event_cb(lv_obj_t * sw, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(sw);
    item_cont_ext_t * item_ext = lv_obj_get_ext_attr(cont);

    if(e == LV_EVENT_VALUE_CHANGED) {

        item_ext->item->state = lv_sw_get_state(sw);
        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Button event callback. Call the item's `event_cb`  with `LV_EVENT_VALUE_CHANGED` when clicked.
 * @param obj pointer to the switch or the container in case of `LV_EVENT_REFRESH`
 * @param e the event
 */
static void btn_event_cb(lv_obj_t * obj, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(obj);
    item_cont_ext_t * item_ext = lv_obj_get_ext_attr(cont);

    if(e == LV_EVENT_CLICKED) {
        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Drop down list event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED` and save the state.
 * @param ddlist pointer to the Drop down lsit
 * @param e the event
 */
static void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(ddlist);
    item_cont_ext_t * item_ext = lv_obj_get_ext_attr(cont);

    if(e == LV_EVENT_VALUE_CHANGED) {
        item_ext->item->state = lv_ddlist_get_selected(ddlist);

        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Number set buttons' event callback. Increment/decrement the state and call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED`.
 * @param btn pointer to the plus or minus button
 * @param e the event
 */
static void numset_event_cb(lv_obj_t * btn, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(btn);
    item_cont_ext_t * item_ext = lv_obj_get_ext_attr(cont);
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {

        lv_obj_t * label = lv_obj_get_child(btn, NULL);
        if(strcmp(lv_label_get_text(label), LV_SYMBOL_MINUS) == 0) item_ext->item->state--;
        else item_ext->item->state ++;

        menu_cont_ext_t * menu_ext = lv_obj_get_ext_attr(act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, LV_EVENT_VALUE_CHANGED, item_ext->item);

        /*Get the value label*/
        label = lv_obj_get_child(cont, NULL);
        label = lv_obj_get_child(cont, label);

        lv_label_set_text(label, item_ext->item->value);
    }
    else if(e == LV_EVENT_DELETE) {
        item_ext->item->cont = NULL;
    }
}

/**
 * Back button event callback. Load the previous menu on click and delete the current.
 * @param btn pointer to the back button
 * @param e the event
 */
static void header_back_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void) btn; /*Unused*/

    if(e != LV_EVENT_CLICKED) return;

    lv_obj_t * old_menu_cont = act_cont;

    /*Delete the current item form the history. The goal is the previous.*/
    histroy_t * act_hist;
    act_hist = lv_ll_get_head(&history_ll);
    if(act_hist) {
        lv_ll_rem(&history_ll, act_hist);
        lv_mem_free(act_hist);

        /*Get the real previous item and open it*/
        histroy_t * prev_hist =  lv_ll_get_head(&history_ll);

        if(prev_hist) {
            /* Create the previous menu.
             * Remove it from the history because `lv_settings_create_page` will add it again */
            lv_ll_rem(&history_ll, prev_hist);
            lv_settings_open_page( prev_hist->item, prev_hist->event_cb);
            lv_mem_free(prev_hist);
        }
        else {
            /*No previous menu, so no main container*/
            act_cont = NULL;
        }
    }

    /*Float out the old menu container*/
    if(old_menu_cont) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, old_menu_cont, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_coord_t w_scr = lv_obj_get_width(lv_scr_act());
        bool menu_btn_right = lv_obj_get_x(menu_btn) >= w_scr/2;
        lv_coord_t w_cont = lv_obj_get_width(old_menu_cont);
        uint32_t start = !menu_btn_right ? 0 : w_scr-w_cont;
        uint32_t end = !menu_btn_right ? -w_cont : w_scr;
        lv_anim_set_values(&a, start, end);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
        lv_anim_set_time(&a, LV_SETTINGS_ANIM_TIME, 0);
        lv_anim_set_ready_cb(&a, old_cont_del_cb);
        lv_anim_create(&a);

        /*Move the old menu to the for ground. */
        lv_obj_move_foreground(old_menu_cont);
    }

    if(act_cont) {
        lv_anim_del(act_cont, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_coord_t w_scr = lv_obj_get_width(lv_scr_act());
        bool menu_btn_right = lv_obj_get_x(menu_btn) >= w_scr/2;
        lv_coord_t w_cont = lv_obj_get_width(act_cont);
        lv_obj_set_x(act_cont, !menu_btn_right ? 0 : w_scr-w_cont);
    }
}

/**
 * Create a container for the items of a page
 * @param page pointer to a page where to create the container
 * @param item pointer to a `lv_settings_item_t` variable. The pointer will be saved in the container's `ext`.
 * @return the created container
 */
static lv_obj_t * item_cont_create(lv_obj_t * page, lv_settings_item_t * item)
{
    lv_obj_t * cont = lv_cont_create(page, NULL);
    lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &style_item_cont);
    lv_cont_set_fit2(cont, LV_FIT_FLOOD, LV_FIT_TIGHT);
    lv_obj_set_click(cont, false);

    item_cont_ext_t * ext = lv_obj_allocate_ext_attr(cont, sizeof(item_cont_ext_t));
    ext->item = item;
    ext->item->cont = cont;

    return cont;
}

/**
 * Delete the old main container when the animation is ready
 * @param a pointer to the animation
 */
static void old_cont_del_cb(lv_anim_t * a)
{
    lv_obj_del(a->var);
}

