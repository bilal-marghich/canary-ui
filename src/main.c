#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "system_stats.h"

lv_subject_t *make_int_subject(int value)
{
    lv_subject_t *s = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(s, value);
    return s;
}

lv_subject_t *make_float_subject(float value)
{
    lv_subject_t *s = lv_subject_create(LV_SUBJECT_TYPE_FLOAT);
    lv_subject_set_float(s, value);
    return s;
}

int main(void)
{
    lv_init();

    lv_display_t *disp = lv_sdl_window_create(480, 272);
    lv_indev_t *mouse = lv_sdl_mouse_create();

    lv_subject_t *cpuValue = make_float_subject(_get_cpu_percent());
    lv_subject_t *memValue = make_float_subject(_get_mem_percent());
    lv_subject_t *diskValue = make_float_subject(_get_disk_percent());
    lv_subject_t *tempValue = make_int_subject(_get_temp_celsius());

    struct system_memory_stats initialMem = _get_system_memory_uptime();

    lv_obj_t *cpuLabel = lv_label_create(lv_screen_active());
    lv_label_bind_text(cpuLabel, cpuValue, "cpu value : %.2f%%");
    lv_obj_set_y(cpuLabel, 30);

    lv_obj_t *memlabel = lv_label_create(lv_screen_active());
    lv_label_bind_text(memlabel, memValue, "MEM value : %.2f%%");
    lv_obj_set_y(memlabel, 60);

    lv_obj_t *templabel = lv_label_create(lv_screen_active());
    lv_label_bind_text(templabel, tempValue, "TEMP Value: %d%%");
    lv_obj_set_y(templabel, 90);

    lv_obj_t *diskLabel = lv_label_create(lv_screen_active());
    lv_label_bind_text(diskLabel, diskValue, "DISK Value: %.2f%%");
    lv_obj_set_y(diskLabel, 120);

    lv_obj_t *uptimeLabel = lv_label_create(lv_screen_active());
    lv_label_set_text_fmt(uptimeLabel, "UPTIME Value: %02lu:%02lu:%02lu",
                           initialMem.uptime_hours,
                           initialMem.uptime_minutes,
                           initialMem.uptime_seconds);
    lv_obj_set_y(uptimeLabel, 150);

   
    while (1) {
        lv_subject_set_float(cpuValue, _get_cpu_percent());
        lv_subject_set_float(memValue, _get_mem_percent());
        lv_subject_set_float(diskValue, _get_disk_percent());
        lv_subject_set_int(tempValue, _get_temp_celsius());

        struct system_memory_stats memNow = _get_system_memory_uptime();
        lv_label_set_text_fmt(uptimeLabel, "UPTIME Value: %02lu:%02lu:%02lu",
                               memNow.uptime_hours,
                               memNow.uptime_minutes,
                               memNow.uptime_seconds);

        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}