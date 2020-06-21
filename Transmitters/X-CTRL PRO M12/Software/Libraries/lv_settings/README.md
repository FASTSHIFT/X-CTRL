# Settings menu

**A collections of ready to use components to create smartphone-like menus**

![Settings menu with LittlevGL](../../examples/lv_ex_settings/lv_ex_settings_2.gif)

_Created by calling `lv_ex_settings_2()`_

### Table of content
- [Overview](#overview)
- [Create a menu](#create-a-menu)
- [Refresh items' data](#refresh-items-data)
- [Styling](#styling)
- [Use with keyboard](#use-with-keyboard)
- [Examples](#examples)

## Overview

The menu builds-up from 
1. A *root item* which opens the menu on click
2. Pages of the menu

There is a back button every page to go back to the previous page or on the first page to close the menu.

The descriptor elements of menu items have `lv_settings_item_t` type. It has the following fields:
- **type** of the item (`LV_SETTINGS_TYPE_...`). See below.
- **name** name of the item as a string
- **value** current value of the item as a string
- **state** the current or last state of the item as an integer (e.g. slider's value)
- **user_date** a `void *` pointer to store any custom data
- **cont** pointer to the created lvgl object if exists. (Used internally)

### Item types

#### List button
Referred as  `LV_SETTINGS_TYPE_LIST_BTN`,

A large button. Usually used to open new pages in the menu.  

![List button in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/raw/master/misc/lv_settings_list_btn.png)

It displays the `name` and value of the `item`. If a theme is set `value` will use `theme.style.label.hint` style.

The list buttons don't use `state` so it can be freely if required to keep track of the state of something.

`LV_EVENT_CLICKED`, `LV_EVENT_SHORT_CLICKED`, `LV_EVENT_LONG_PRESSED` is sent to the `event_cb`


#### Button
Referred as  `LV_SETTINGS_TYPE_BTN`,

Displays `name` and a button with `value` text on it.

![Button in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/blob/master/misc/lv_settings_btn.gif)

The buttons don't use `state` so it can be freely if required to keep track of the state of something.

In `event_cb` `LV_EVENT_CLICKED` is sent when the button is clicked 

#### Switch
Referred as  `LV_SETTINGS_TYPE_SW`,

Displays a switch with `name` and `value` texts.

![Switch in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/blob/master/misc/lv_settings_sw.gif)

The current state is stored in the `state` filed.

In `event_cb` `LV_EVENT_VALUE_CHANGED` is sent when the switch changes state.

#### Drop down list
Referred as  `LV_SETTINGS_TYPE_DDLIST`,

Displays a Drop down list with `name` as title and options stored in `value`.

![Drop down list in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/blob/master/misc/lv_settings_ddlist.gif)

The current options id is stored in the `state` filed.

In `event_cb` `LV_EVENT_VALUE_CHANGED` is sent when a new option is selected.

#### Number set
Referred as  `LV_SETTINGS_TYPE_NUMSET`,

Displays the `name` as title, a `value` and `+`, `-` buttons.

![Number set in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/blob/master/misc/lv_settings_numset.gif)

The current value is stored in the `state` filed.

In `event_cb` `LV_EVENT_VALUE_CHANGED` is sent when a new option is selected.


#### Slider
Referred as  `LV_SETTINGS_TYPE_SLIDER`,

Displays the `name` as title and `value` above a slider.

![Drop down list in settings application - LittlevGL](https://github.com/littlevgl/lv_apps/blob/master/misc/lv_settings_slider.gif)

The sliders range is `[0..255]`. The current value is stored in the `state` filed.

In `event_cb` `LV_EVENT_VALUE_CHANGED` is sent when a new option is selected.

## Create a menu

### Initialize items

To initialize an item the field of an `lv_settings_item_t` variable needs to be filled.
```c
static lv_settigns_item_t item1;
item1.type = LV_SETTINGS_TYPE_SLIDER;
item1.name = "Slider1";
item1.value = "30 %";
item1.state = (50 * 256) / 100;  /*30 %, default slider range is 256*/
```

It's a good practice to set a character array for `value` if it will change later.
```c
static char slider2_value[32] = {"50 %"};
static lv_settigns_item_t item2;
item2.type = LV_SETTINGS_TYPE_SLIDER;
item2.name = "Slider2";
item2.value = slider_value;
item2.state = (50 * 256) / 100 ;  /*50 %*/
``` 

The items can be initialized in compile time too.
```c
static char slider3_value[32] = {"70 %"};
static const lv_settigns_item_t item3 = {
  .type = LV_SETTINGS_TYPE_SLIDER;
  .name = "Slider3";
  .value = slider3_value;
  .state = (70 * 256) / 100 ;  /*70 %*/
};
```

And the item can be grouped into an array.
```c
static char slider_values[3][32] = {"90 %",  "91 %",  "92 %"};
static const lv_settigns_item_t items[3] = {
  { .type = LV_SETTINGS_TYPE_SLIDER, .name = "Slider4_1",  .value = slider_value[0] .state = (90 * 256) / 100},
  { .type = LV_SETTINGS_TYPE_SLIDER, .name = "Slider4_2",  .value = slider_value[1] .state = (91 * 256) / 100},
  { .type = LV_SETTINGS_TYPE_SLIDER, .name = "Slider4_3",  .value = slider_value[2] .state = (92 * 256) / 100},
};
```

### Root element
The root element the button which opens the settings menu. It also has to be an item desriptor:
```c
static lv_settings_item_t root_item = {.name = "Settings", .value = ""};
```
 The root element receives `LV_EVENT_CLICKED` event to create its menu.


### Create a page and add items
In the root element's `LV_EVENT_CLICKED` event `lv_settings_open_page(act_item, main_menu_event_cb);` needs to be called to create a menu page:
```c
void root_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        /*Get the caller item*/
        lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

        /*Create a new page in the menu*/
        lv_settings_open_page(act_item, main_menu_event_cb);
}

void main_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    /*Add the meain menu's items */
    if(e == LV_EVENT_REFRESH) {
        uint32_t i;
        for(i = 0; main_menu_items[i].type != LV_SETTINGS_TYPE_INV; i++) {
            lv_settings_add(&main_menu_items[i]);
        }
    }
```

The new page is created with `lv_settings_open_page` from the `act_item`. `act_item->name` will be the title of the menu page.
`lv_settings_open_page` sends a `LV_EVENT_REFRESH`  to the `event_cb` set in the second parameter. These callbacks will be used when:
- the items need to be added again (Open this page when the "Back" button is pressed)
- the items of this page are clicked or their value has been changed.

### Handle item events

It's up to the developer how to handle the the items' event in the`event_cb` of the page. The common point is the triggering element can be get by `lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();` 

To figure out which item was triggered the event:
1. Compare the pointer: `if(act_item == &my_slider_item) { ... }`
2. Compare the name of item: `if(strcmp(act_item->name, "My slider") == 0) { ... }`
3. Use the `user_data` as you wish. 

### Add the root menu button
Once the items are initialized the menu can be created with
```c
    lv_settings_create(&root_item, event_cb);
```

## Refresh items' data
In the event function probably some data of the item should be refreshed. 
Mainly the `value` string formatted as required and the `state` if it needs to be limited or overwritten.
The item's `name` ca ón be updated too.

The items can be refreshed with `lv_settings_refr(act_item);`.

For example for a "Number setting" item:
```c
void motor_menu_event_cb(lv_obj_t * btn, lv_event_t e)
{
    /*Get the caller item*/
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    /*Handle the events of the Motor settings items*/
    if(e == LV_EVENT_VALUE_CHANGED) {
        if(strcmp("Max Current", act_item->name) == 0) {
            /*Limit the values*/
            if(act_item->state > 10) act_item->state = 10;
            if(act_item->state < 1) act_item->state = 1;
     
            /*Format the value. Assuming `item.value` is set to an array when initialized*/
            sprintf(act_item->value, "%dA", act_item->state);
            
            /*Refresh the item with the new `state` and `value`*/
            lv_settings_refr(act_item);
        }
    }
}
```

## Styling
It's recommended to use a theme to make the menu stylish. `lv_theme_night` and `lv_theme_material` work well with it. Before calling `lv_settings_create` a theme can be applied like this:
```c
    lv_theme_t *th = lv_theme_material_init(210, NULL);

    /*Try with different theme too*/
//    lv_theme_t *th = lv_theme_material_init(10, NULL);
//    lv_theme_t *th = lv_theme_night_init(40, NULL);

    lv_theme_set_current(th);
    
    /*Create the settings menu with a root item*/
    lv_settings_create(&root_item, event_cb);

```

## Use with keyboard
The settingsmmenu workd well with keybords too using LittlevGL's [group system](https://docs.littlevgl.com/en/html/overview/indev.html#keypad-and-encoder). Once you have a group is created it can be set in the application. After that, the relevant objects of the items will be automatically added to the group. It workw with `LV_INDEV_TYPE_KEYPAD` and `LV_INDEV_TYPE_ENCODER` too.
```c
/*Add keyboard or mouswheel input devices if enabled*/
keyboard_init();
lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
indev_drv.type = LV_INDEV_TYPE_KEYPAD;
indev_drv.read_cb = keyboard_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/

lv_indev_t * indev = lv_indev_drv_register(&indev_drv);

lv_group_t * g = lv_group_create();
lv_indev_set_group(indev, g);

lv_settings_set_group(g);

/*Create the settings menu with a root item*/
lv_settings_create(&root_item, event_cb);
```

## Examples

See the [lv_ex_settings](https://github.com/littlevgl/lv_apps/tree/master/examples/lv_ex_settings) folder for examples.

Just simply call `lv_ex_settings_1()` or `lv_ex_settings_2()` to see some examples of what kind of settings menu you can create.
