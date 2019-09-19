#include "DisplayPrivate.h"

static lv_disp_buf_t disp_buf;
lv_color_t lv_disp_buf[LV_HOR_RES_MAX * 20];

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{

    Serial.printf("%s@%d->%s\r\n", file, line, dsc);
    delay(100);
}
#endif

/* Display flushing */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    screen.fastDrawRGBBitmap(area->x1, area->y1, (uint16_t*)color_p, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
    lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

bool my_touchpad_read(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    tp_dev.scan();

#define touchpad_is_pressed() tp_dev.sta&TP_PRES_DOWN

    /*Save the state and save the pressed coordinate*/
    data->state = touchpad_is_pressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(data->state == LV_INDEV_STATE_PR)
    {
        //touchpad_get_xy(&last_x, &last_y);
        last_x = tp_dev.x[0];
        last_y = tp_dev.y[0];
    }

    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = last_x;
    data->point.y = last_y;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

void lv_user_init()
{
    lv_init();

#if USE_LV_LOG != 0
    lv_log_register_print(my_print); /* register print function for debugging */
#endif

    lv_disp_buf_init(&disp_buf, lv_disp_buf, NULL, sizeof(lv_disp_buf)/sizeof(lv_color_t));

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);


    /*Initialize the touch pad*/
    lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/
}
