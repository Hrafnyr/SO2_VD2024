#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

// Función para leer las estadísticas de un proceso específico
double get_stats_pid(const char *pid, const char *key) {
    char path[256], line[256], target[64];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);

    FILE *file = fopen(path, "r");
    if (!file) return -1;

    double value = -1;
    snprintf(target, sizeof(target), "%s:", key);

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, target, strlen(target)) == 0) {
            sscanf(line + strlen(target), "%lf", &value);
            break;
        }
    }

    fclose(file);
    return value;
}

// Función para obtener el OOM score de un proceso
int get_oom(const char *pid) {
    char path[256], line[16];
    snprintf(path, sizeof(path), "/proc/%s/oom_score", pid);

    FILE *file = fopen(path, "r");
    if (!file) return -1;

    int oomScore = -1;
    if (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d", &oomScore);
    }

    fclose(file);
    return oomScore;
}

// Función para mostrar estadísticas de un proceso
void showStats(FILE *output_file, const char *pid) {
    double reserved = get_stats_pid(pid, "VmSize");
    double committed = get_stats_pid(pid, "VmRSS");
    int oom_score = get_oom(pid);

    if (reserved < 0 || committed < 0 || oom_score < 0) {
        fprintf(output_file, "Error: No se pudieron leer las estadísticas para el PID %s.\n", pid);
        return;
    }

    fprintf(output_file, "+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+\n");
    fprintf(output_file, "PID: %s\n", pid);
    fprintf(output_file, "  Memoria Reservada: %.2f MB\n", reserved / 1024);
    fprintf(output_file, "  Memoria Utilizada: %.2f MB (%.2f%%)\n", committed / 1024, (committed / reserved) * 100);
    fprintf(output_file, "  OOM Score: %d\n\n", oom_score);
    fprintf(output_file, "*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*\n");
}

// Función para iterar sobre todos los procesos en el directorio /proc y escribir los resultados en un archivo
void iterate_all_processes() {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("Error al abrir /proc");
        return;
    }

    struct dirent *entry;
    FILE *output_file = fopen("process_stats.txt", "w");
    if (!output_file) {
        perror("Error al abrir el archivo de salida");
        closedir(proc_dir);
        return;
    }

    double total_reserved = 0, total_committed = 0;

    while ((entry = readdir(proc_dir)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue; // Solo procesar directorios numéricos (PIDs)

        double reserved = get_stats_pid(entry->d_name, "VmSize");
        double committed = get_stats_pid(entry->d_name, "VmRSS");

        if (reserved > 0 && committed > 0) {
            total_reserved += reserved;
            total_committed += committed;
            showStats(output_file, entry->d_name); // Mostrar las estadísticas del proceso en el archivo
        }
    }

    fprintf(output_file, "+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+|+\n");
    fprintf(output_file, "\nResumen de TODOS los procesos:\n");
    fprintf(output_file, "  Memoria Total Reservada: %.2f MB\n", total_reserved / 1024);
    fprintf(output_file, "  Memoria Total Utilizada: %.2f MB\n", total_committed / 1024);

    fclose(output_file);
    closedir(proc_dir);
}

int main() {
    printf("Iniciando la recolección de estadísticas de procesos...\n");

    iterate_all_processes();

    printf("Los datos de los procesos han sido guardados en 'process_stats.txt'.\n");

    return 0;
}
