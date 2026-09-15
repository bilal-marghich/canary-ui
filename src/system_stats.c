#include "system_stats.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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