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

static lv_obj_t *make_stat_row(lv_obj_t *parent, const char *name,
                                lv_subject_t *subject, const char *fmt,
                                lv_color_t accent)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_bottom(row, 6, 0);

    lv_obj_t *top = lv_obj_create(row);
    lv_obj_remove_style_all(top);
    lv_obj_set_width(top, lv_pct(100));
    lv_obj_set_height(top, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN,
                           LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = lv_label_create(top);
    lv_label_set_text(name_label, name);
    lv_obj_set_style_text_color(name_label, lv_color_white(), 0);

    lv_obj_t *value_label = lv_label_create(top);
    lv_label_bind_text(value_label, subject, fmt);
    lv_obj_set_style_text_color(value_label, accent, 0);
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_14, 0);

    lv_obj_t *bar = lv_bar_create(row);
    lv_obj_set_size(bar, lv_pct(100), 6);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x2a2a2a), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, accent, LV_PART_INDICATOR);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);

    return bar;
}

int main(void)
{
    lv_init();

    lv_display_t *disp = lv_sdl_window_create(480, 272);
    lv_indev_t *mouse = lv_sdl_mouse_create();

    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0a1929), 0);

    lv_obj_t *root = lv_obj_create(scr);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(root, 12, 0);
    lv_obj_set_style_pad_row(root, 4, 0);

    // Title bar 
    lv_obj_t *title = lv_label_create(root);
    lv_label_set_text(title, "CANARY");
    lv_obj_set_style_text_color(title, lv_color_hex(0xf4c430), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_pad_bottom(title, 6, 0);

    lv_subject_t *cpuValue = make_float_subject(_get_cpu_percent());
    lv_subject_t *memValue = make_float_subject(_get_mem_percent());
    lv_subject_t *diskValue = make_float_subject(_get_disk_percent());
    lv_subject_t *tempValue = make_int_subject(_get_temp_celsius());

    lv_color_t yellow = lv_color_hex(0xf4c430);

    lv_obj_t *cpuBar  = make_stat_row(root, "CPU",  cpuValue,  "%.0f%%", yellow);
    lv_obj_t *memBar  = make_stat_row(root, "MEM",  memValue,  "%.0f%%", yellow);
    lv_obj_t *diskBar = make_stat_row(root, "DISK", diskValue, "%.0f%%", yellow);
    lv_obj_t *tempBar = make_stat_row(root, "TEMP", tempValue, "%d C",   yellow);

    struct system_memory_stats initialMem = _get_system_memory_uptime();
    lv_obj_t *uptimeLabel = lv_label_create(root);
    lv_label_set_text_fmt(uptimeLabel, "UPTIME  %02lu:%02lu:%02lu",
                           initialMem.uptime_hours,
                           initialMem.uptime_minutes,
                           initialMem.uptime_seconds);
    lv_obj_set_style_text_color(uptimeLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_pad_top(uptimeLabel, 8, 0);

    while (1) {
        float cpu = _get_cpu_percent();
        float mem = _get_mem_percent();
        float disk = _get_disk_percent();
        int temp = _get_temp_celsius();

        lv_subject_set_float(cpuValue, cpu);
        lv_subject_set_float(memValue, mem);
        lv_subject_set_float(diskValue, disk);
        lv_subject_set_int(tempValue, temp);

        lv_bar_set_value(cpuBar,  (int)cpu,  LV_ANIM_ON);
        lv_bar_set_value(memBar,  (int)mem,  LV_ANIM_ON);
        lv_bar_set_value(diskBar, (int)disk, LV_ANIM_ON);
        lv_bar_set_value(tempBar, temp,      LV_ANIM_ON);

        struct system_memory_stats memNow = _get_system_memory_uptime();
        lv_label_set_text_fmt(uptimeLabel, "UPTIME  %02lu:%02lu:%02lu",
                               memNow.uptime_hours,
                               memNow.uptime_minutes,
                               memNow.uptime_seconds);

        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}