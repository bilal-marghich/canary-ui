#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include <unistd.h>

int main(void)
{
    lv_init();

    lv_display_t *disp = lv_sdl_window_create(480, 272);
    lv_indev_t *mouse = lv_sdl_mouse_create();

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello CANARY");
    lv_obj_center(label);

    while (1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}