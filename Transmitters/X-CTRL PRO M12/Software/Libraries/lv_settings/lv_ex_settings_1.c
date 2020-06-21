/**
 * @file lv_ex_settings_1.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../src/lv_settings/lv_settings.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mousewheel.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_SETTINGS_KEYBOARD
#define LV_SETTINGS_KEYBOARD 0
#endif

#ifndef LV_SETTINGS_MOUSEWHEEL
#define LV_SETTINGS_MOUSEWHEEL 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void root_event_cb(lv_obj_t * btn, lv_event_t e);
static void main_event_cb(lv_obj_t * btn, lv_event_t e);
static void submenu_event_cb(lv_obj_t * btn, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/

/*Declare items*/
static lv_settings_item_t root_item = {.name = "Settings", .value = ""};

static lv_settings_item_t main_menu_items[] =
{
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Button",          .value="Test button"},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Switch",          .value="Test switch"},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Drop down list",  .value="Test drop down list"},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Number set",      .value="Test number set"},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Slider",          .value="Test slider"},
       {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_ex_settings_1(void)
{
    lv_theme_t *th = lv_theme_material_init(210, NULL);

    /*Try with different theme too*/
//    lv_theme_t *th = lv_theme_material_init(10, NULL);
//    lv_theme_t *th = lv_theme_night_init(40, NULL);

    lv_theme_set_current(th);


/*Add keyboard or mouswheel input devices if enabled*/
#if LV_SETTINGS_KEYBOARD
    keyboard_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keyboard_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/

#elif LV_SETTINGS_MOUSEWHEEL
    mousewheel_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = mousewheel_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/
#endif

#if LV_SETTINGS_KEYBOARD || LV_SETTINGS_MOUSEWHEEL
    lv_indev_t * indev = lv_indev_drv_register(&indev_drv);

    lv_group_t * g = lv_group_create();
    lv_indev_set_group(indev, g);
    lv_settings_set_group(g);
#endif


    /*Create the settings menu with a root item*/
    lv_settings_create(&root_item, root_event_cb);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void root_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    if(e == LV_EVENT_CLICKED) {
        /*Get the caller item*/
        lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

        /*Create a new page in the menu*/
        lv_settings_open_page(act_item, main_event_cb);
    }
}

static void main_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    /*Add the items*/
    if(e == LV_EVENT_REFRESH) {
        uint32_t i;
        for(i = 0; main_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&main_menu_items[i]);
        }
   }
    /* Open submenus.
     * The submenus will be very simple so use a common event callback for them.
     * `act_item` is the clicked list button in the main menu.
     * It's name will be the title of the new page*/
    else if(e == LV_EVENT_CLICKED) {
        lv_settings_open_page(act_item, submenu_event_cb);
    }
}

static void submenu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    /*Add items to the submenus*/
    if(e == LV_EVENT_REFRESH) {
        if(strcmp("Button", act_item->name) == 0) {
            static lv_settings_item_t item = {
                    .type = LV_SETTINGS_TYPE_BTN,
                    .name = "System test",
                    .value = "Start"};
            lv_settings_add(&item);
        }
        else if(strcmp("Slider", act_item->name) == 0) {

            static char value[16] = {"100 V"};
            static lv_settings_item_t item = {
                    .type = LV_SETTINGS_TYPE_SLIDER,
                    .name = "Voltage",
                    .value = value,
                    .state = 0};

            lv_settings_add(&item);
        }
        else if(strcmp("Switch", act_item->name) == 0) {

            static lv_settings_item_t item = {
                    .type = LV_SETTINGS_TYPE_SW,
                    .name = "Valve",
                    .value = "Closed",
                    .state = 0};

            lv_settings_add(&item);
        }
        else if(strcmp("Number set", act_item->name) == 0) {
            static char value[16] = {"12"};
            static lv_settings_item_t item = {
                    .type = LV_SETTINGS_TYPE_NUMSET,
                    .name = "Age",
                    .value = value,
                    .state = 12};

            lv_settings_add(&item);
        }
        else if(strcmp("Drop down list", act_item->name) == 0) {
            static lv_settings_item_t item = {
                    .type = LV_SETTINGS_TYPE_DDLIST,
                    .name = "Size",
                    .value = "S\nM\nL\nXL",
                    .state = 0};

            lv_settings_add(&item);
        }
    }
    /*Handle the items' events*/
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Voltage", act_item->name) == 0) {
            sprintf(act_item->value, "%d V", act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Age", act_item->name) == 0) {
            if(act_item->state < 6) act_item->state = 6;
            if(act_item->state > 99) act_item->state = 99;

            sprintf(act_item->value, "%d", act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Valve", act_item->name) == 0) {
            if(act_item->state) act_item->value = "Open";
            else act_item->value = "Close";

            lv_settings_refr(act_item);
        }
        else if(strcmp("Size", act_item->name) == 0) {
            printf("Size: %d\n", act_item->state);
        }
    } else if(e == LV_EVENT_CLICKED) {
        if(strcmp("System test", act_item->name) == 0) {
            if(strcmp(act_item->value, "Start") == 0) {
                act_item->value = "Stop";
            } else {
                act_item->value = "Start";
            }
            lv_settings_refr(act_item);
        }
    }
}
