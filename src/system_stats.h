#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

struct system_disk_stats {
    unsigned long long total_disk;
    unsigned long long used_disk;
    unsigned long long available_disk;
};


struct system_memory_stats {
    unsigned long total_ram;
    unsigned long used_ram;
    unsigned long uptime_hours;
    unsigned long uptime_minutes;
    unsigned long uptime_seconds;
};


float _get_cpu_percent();
int _get_temp();
struct system_memory_stats _get_system_memory_uptime();
struct system_disk_stats get_system_disk_stats();

#endif