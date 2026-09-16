#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

struct system_memory_stats {
    unsigned long total_ram;
    unsigned long used_ram;
    unsigned long uptime_hours;
    unsigned long uptime_minutes;
    unsigned long uptime_seconds;
};

struct system_disk_stats {
    unsigned long long total_disk;
    unsigned long long used_disk;
    unsigned long long available_disk;
};

float _get_cpu_percent(void);

int _get_temp_celsius(void);
struct system_memory_stats _get_system_memory_uptime(void);
struct system_disk_stats _get_system_disk_stats(void);

/* Convenience percentage helpers built on the structs above. */
float _get_mem_percent(void);
float _get_disk_percent(void);

#endif