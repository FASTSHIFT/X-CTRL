CSRCS += lv_settings.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_apps/settings
VPATH += :$(LVGL_DIR)/lv_apps/lv_settings

CFLAGS += "-I$(LVGL_DIR)/lv_apps/lv_settings"
