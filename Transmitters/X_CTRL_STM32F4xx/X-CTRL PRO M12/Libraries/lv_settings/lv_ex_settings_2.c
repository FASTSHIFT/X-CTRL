/**
 * @file lv_ex_settings_2.c
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
typedef enum {
    MOTOR_ITEM_RPM = 0,
    MOTOR_ITEM_CURRENT,
    MOTOR_ITEM_TYPE,
    MOTOR_ITEM_PROTECT,
    MOTOR_ITEM_TEST,
}motor_item_t;

typedef enum {
    MISC_ITEM_TIME = 0,
    MISC_ITEM_BRIGHTNESS,
}misc_item_t;

typedef enum {
    TIME_ITEM_HOURS = 0,
    TIME_ITEM_MINS,
}time_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void root_event_cb(lv_obj_t * btn, lv_event_t e);
static void main_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void motor_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void misc_menu_event_cb(lv_obj_t * btn, lv_event_t e);
static void time_menu_event_cb(lv_obj_t * btn, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/

/*Declare items*/
static lv_settings_item_t root_item = {.name = "Settings", .value = ""};

static lv_settings_item_t main_menu_items[] =
{
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Motor settings", .value="Current, RPM, etc"},
       {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Misc. settings", .value="Time, brightness, etc."},
       {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
};

static char max_rpm_value[8];
static char max_current_value[8];
/*Follow the order of `motor_item_t`*/
static lv_settings_item_t motor_menu_items[] = {
        {.type = LV_SETTINGS_TYPE_SLIDER, .name = "Max RPM", .value = max_rpm_value},
        {.type = LV_SETTINGS_TYPE_NUMSET, .name = "Max Current", .value = max_current_value},
        {.type = LV_SETTINGS_TYPE_DDLIST, .name = "Motor type", .value = "Type 1\n"
                                                                         "Type 2\n"
                                                                         "Type 3"},
        {.type = LV_SETTINGS_TYPE_SW, .name = "Short circuit protect", .value = ""},
        {.type = LV_SETTINGS_TYPE_BTN, .name = "Test motor", .value = "Start"},
        {.type = LV_SETTINGS_TYPE_INV},     /*Mark the last item*/
};


static char time_value[8];
static char brightness_value[8];
/*Follow the order of `mics_item_t`*/
static lv_settings_item_t misc_menu_items[] =
{
        {.type = LV_SETTINGS_TYPE_LIST_BTN, .name="Time", .value=time_value},
        {.type = LV_SETTINGS_TYPE_SLIDER, .name="Brightness", .value=brightness_value},
        {.type=LV_SETTINGS_TYPE_INV},       /*Mark the last item*/
};

static char hours_value[3];
static char mins_value[3];
/*Follow the order of `time_item_t`*/
static lv_settings_item_t time_menu_items[] =
{
        {.type = LV_SETTINGS_TYPE_NUMSET, .name="Hour", .value=hours_value},
        {.type = LV_SETTINGS_TYPE_NUMSET, .name="Minute", .value=mins_value},
        {.type=LV_SETTINGS_TYPE_INV},    /*Mark the last item*/
};



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_ex_settings_2(void)
{
    lv_theme_t *th = lv_theme_material_init(210, NULL);

    /*Try with different theme too*/
//    lv_theme_t *th = lv_theme_material_init(10, NULL);
//    lv_theme_t *th = lv_theme_night_init(40, NULL);

    lv_theme_set_current(th);


/*Add keyboard or mousewheel input devices if enabled*/
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


    /*Load the default values*/
    lv_settings_item_t * i;

    /*Motor menu*/
    i = &motor_menu_items[MOTOR_ITEM_RPM];
    i->state = 80;
    sprintf(i->value, "%dRPM", i->state);


    i = &motor_menu_items[MOTOR_ITEM_CURRENT];
    i->state = 3;
    sprintf(i->value, "%dA", i->state);

    i = &motor_menu_items[MOTOR_ITEM_TYPE];
    i->state = 1;

    i = &motor_menu_items[MOTOR_ITEM_PROTECT];
    i->state = 0;
    i->value = "Not protected";

    i = &motor_menu_items[MOTOR_ITEM_TEST];
    i->value = "Start";

    /*Time menu*/
    i = &time_menu_items[TIME_ITEM_HOURS];
    i->state  = 10;
    sprintf(i->value, "%d", i->state);

    i = &time_menu_items[TIME_ITEM_MINS];
    i->state  = 35;
    sprintf(i->value, "%d", i->state);

    /*Misc menu*/
    i = &misc_menu_items[MISC_ITEM_BRIGHTNESS];
    i->state = 128;
    sprintf(i->value, "%d%%", (i->state * 100) / 256);

    i = &misc_menu_items[MISC_ITEM_TIME];
    sprintf(i->value, "%d:%d", time_menu_items[TIME_ITEM_HOURS].state, time_menu_items[TIME_ITEM_MINS].state);


    /*Create the settings menu with a root item*/
    lv_obj_t *btn = lv_settings_create(&root_item, root_event_cb);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_settings_set_max_width(lv_obj_get_width(lv_scr_act())/2);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void root_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*If the root element was clicked or asks for create a main menu*/
    if(e == LV_EVENT_CLICKED) {
        /*Get the caller item*/
        lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

        lv_settings_open_page(act_item, main_menu_event_cb);
    }
}

static void main_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        uint32_t i;
        for(i = 0; main_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&main_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp(act_item->name, "Motor settings") == 0) {
           lv_settings_open_page(act_item, motor_menu_event_cb);
        }
        else if(strcmp(act_item->name, "Misc. settings") == 0) {
           lv_settings_open_page(act_item, misc_menu_event_cb);
        }
    }
}

static void motor_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {
       /*Add the motor_menu_items*/
       uint32_t i;
       for(i = 0; motor_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
           lv_settings_add(&motor_menu_items[i]);
       }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Max RPM", act_item->name) == 0) {
            sprintf(act_item->value, "%dRPM", act_item->state);
            lv_settings_refr(act_item);
        }
        if(strcmp("Max Current", act_item->name) == 0) {
            if(act_item->state > 10) act_item->state = 10;
            if(act_item->state < 1) act_item->state = 1;
            sprintf(act_item->value, "%dA", act_item->state);
            lv_settings_refr(act_item);
        }
        else if(strcmp("Motor type", act_item->name) == 0) {
            printf("ddlist: %d\n", act_item->state);
        }
        else if(strcmp("Short circuit protect", act_item->name) == 0) {
            if(act_item->state) act_item->value = "Protected";
            else act_item->value = "Not protected";
            lv_settings_refr(act_item);
        }
    }
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp("Test motor", act_item->name) == 0) {
            if(strcmp("Start", act_item->value) == 0) act_item->value = "Stop";
            else act_item->value = "Start";

            lv_settings_refr(act_item);
        }
    }
}

static void misc_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    (void)btn;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        /*Add the misc_menu_items*/
        uint32_t i;
        for(i = 0; misc_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&misc_menu_items[i]);
        }
    }
    /*Handle the events of the Misc menu items*/
    else if(e == LV_EVENT_CLICKED) {
        if(strcmp("Time", act_item->name) == 0) {
            lv_settings_open_page(act_item, time_menu_event_cb);
        }

    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Brightness", act_item->name) == 0) {
            sprintf(act_item->value, "%d%%", (act_item->state * 100) / 256);
            lv_settings_refr(act_item);
        }
    }
}

static void time_menu_event_cb(lv_obj_t * obj, lv_event_t e)
{
    (void)obj;  /*Unused*/

    /*Get the caller act_item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    if(e == LV_EVENT_REFRESH) {

        /*Add the items*/
        uint32_t i;
        for(i = 0; time_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&time_menu_items[i]);
        }
    }
    else if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Hour", act_item->name) == 0) {
            if(act_item->state > 23) act_item->state = 0;
            if(act_item->state < 0) act_item->state = 23;

            sprintf(act_item->value, "%d", act_item->state);
            lv_settings_refr(act_item);

            sprintf(misc_menu_items[0].value, "%02d:%02d", time_menu_items[0].state, time_menu_items[1].state);

        }
        else if(strcmp("Minute", act_item->name) == 0) {
            if(act_item->state > 59) act_item->state = 0;
            if(act_item->state < 0) act_item->state = 59;

            sprintf(act_item->value, "%d", act_item->state);
            lv_settings_refr(act_item);

            sprintf(misc_menu_items[0].value, "%02d:%02d", time_menu_items[0].state, time_menu_items[1].state);
        }
    }
}
