#include "system_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#define INTERVAL_SECONDS 1
#define CPU_FIELDS 10




float _get_cpu_percent()
{
    FILE *procfile;
    char buffer[1000];

    long *values1 = malloc(CPU_FIELDS * sizeof(long));
    long *values2 = malloc(CPU_FIELDS * sizeof(long));

    if (values1 == NULL || values2 == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(values1);
        free(values2);
        return -1.0f;
    }
    /* ---------- First CPU sample ---------- */
    procfile = fopen("/proc/stat", "r");
    if (procfile == NULL) {
        perror("Cannot open /proc/stat");
        free(values1);
        free(values2);
        return -1.0f;
    }

    if (fgets(buffer, sizeof(buffer), procfile)) {
        char *token = strtok(buffer, " ");
        /* Skip "cpu" */
        token = strtok(NULL, " ");
        int i = 0;
        while (token != NULL && i < CPU_FIELDS) {
            values1[i] = strtol(token, NULL, 10);
            i++;
            token = strtok(NULL, " ");
        }
    }
    fclose(procfile);
    /* Wait before taking the second sample */
    sleep(INTERVAL_SECONDS);


    /* ---------- Second CPU sample ---------- */

    procfile = fopen("/proc/stat", "r");
    if (procfile == NULL) {
        perror("Cannot open /proc/stat");
        free(values1);
        free(values2);
        return -1.0f;
    }

    if (fgets(buffer, sizeof(buffer), procfile)) {
        char *token = strtok(buffer, " ");
        /* Skip "cpu" */
        token = strtok(NULL, " ");
        int i = 0;
        while (token != NULL && i < CPU_FIELDS) {
            values2[i] = strtol(token, NULL, 10);
            i++;
            token = strtok(NULL, " ");
        }
    }
    fclose(procfile);
    /* ---------- Calculate CPU usage ---------- */
    long idle1 = values1[3] + values1[4];
    long idle2 = values2[3] + values2[4];

    long total1 = 0;
    long total2 = 0;
    /* Ignore guest and guest_nice */
    for (int i = 0; i < 8; i++) {
        total1 += values1[i];
        total2 += values2[i];
    }

    long delta_total = total2 - total1;
    long delta_idle = idle2 - idle1;

    float cpu_percent = 0.0f;

    if (delta_total > 0) {
        cpu_percent =
            ((float)(delta_total - delta_idle) / delta_total) * 100.0f;
    }
    
    free(values1);
    free(values2);

    return cpu_percent;
}

int _get_temp(){
    FILE *tempfile;
    // file location for st :/sys/devices/virtual/thermal/thermal_zone0/hwmon0/temp1_input  as for ubuntu /sys/devices/virtual/thermal/thermal_zone0/hwmon1/temp1_input
    tempfile=fopen("/sys/devices/virtual/thermal/thermal_zone0/hwmon1/temp1_input","r");
    if(tempfile== NULL)
    {
        printf("can't open temp file.\n");
        return -1;
    }
    int temp;
    if(fscanf(tempfile,"%d",&temp)!=1)
    {
        printf("can't read temp file.\n");
        fclose(tempfile);
        return -1;
    }
    fclose(tempfile);
    return temp/1000;
}

struct system_memory_stats _get_system_memory_uptime()
{
    struct sysinfo info;
    struct system_memory_stats stats = {0};
    
    if (sysinfo(&info) != 0) {
        perror("sysinfo");
        return stats;
    }
    // MEMORY
    unsigned long total_bytes = info.totalram * info.mem_unit;
    unsigned long free_bytes = info.freeram * info.mem_unit;
    unsigned long used_bytes = total_bytes - free_bytes;
    //UPTIME
    unsigned long uptime_seconds = info.uptime;
    unsigned long hours = uptime_seconds / 3600;
    unsigned long minutes = (uptime_seconds % 3600) / 60;
    unsigned long seconds = uptime_seconds % 60;

    stats.total_ram = total_bytes;
    stats.uptime_hours = hours;
    stats.uptime_minutes = minutes;
    stats.uptime_seconds = seconds;
    stats.used_ram = used_bytes;

    return stats;    
}



struct system_disk_stats get_system_disk_stats()
{
    const char *path = "/";
    struct statvfs buf;
    struct system_disk_stats stats = {0};

    if (statvfs(path, &buf) != 0) {
        perror("statvfs");
        return stats;
    }

    unsigned long long total =(unsigned long long)buf.f_blocks * buf.f_frsize;
    unsigned long long free =(unsigned long long)buf.f_bfree * buf.f_frsize;
    unsigned long long available =(unsigned long long)buf.f_bavail * buf.f_frsize;
    unsigned long long used = total - free;

    stats.total_disk = total;
    stats.used_disk = used;
    stats.available_disk = available;

    return stats;
}