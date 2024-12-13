#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define SYS_CAPTURE_MEMORY_SNAPSHOT 548
#define SYS_track_syscall_usage 549
#define SYS_get_io_throttle 550

// Estructura para capturar el snapshot de memoria
struct my_memory_snapshot {
    unsigned long paginas_activas;
    unsigned long paginas_inactivas;
    unsigned long memoria_cache;
    unsigned long memoria_sucia;
    unsigned long paginas_en_disco;
    unsigned long swap_total;
    unsigned long swap_libre;
};

// Estructura para las estadísticas de las syscalls
struct syscall_usage_stats {
    unsigned long open_count;
    unsigned long write_count;
    unsigned long read_count;
    unsigned long fork_count;
};

// Estructura para las estadísticas de I/O
struct my_io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long bytes_cancelled;
    unsigned long maj_flt;  // Fallos de página mayores
    unsigned long min_flt;  // Fallos de página menores
};


void test_memory_snapshot() {
    struct my_memory_snapshot snapshot;

    long result = syscall(SYS_CAPTURE_MEMORY_SNAPSHOT, &snapshot);

    if (result == 0) {
        printf(" ++ ++ Memory Snapshot ++ ++:\n");
        printf("Cached Memory: %lu KB\n", snapshot.memoria_cache);
        printf("Active Pages: %lu KB\n", snapshot.paginas_activas);
        printf("Inactive Pages: %lu KB\n", snapshot.paginas_inactivas);
        printf("Dirty Memory: %lu KB\n", snapshot.memoria_sucia);
        printf("Writeback Memory: %lu KB\n", snapshot.paginas_en_disco);
        printf("Swap Total: %lu KB\n", snapshot.swap_total);
        printf("Swap Free: %lu KB\n", snapshot.swap_libre);
    } else {
        perror("syscall failed");
        printf("Error code: %ld\n", result);
    }
}

void test_syscall_usage() {
    struct syscall_usage_stats stats;

    int ret = syscall(SYS_track_syscall_usage, &stats);

    if (ret == 0) {
        printf("\n++ Conteo de llamadas del sistema ++\n");
        printf("Open count: %lu\n", stats.open_count);
        printf("Write count: %lu\n", stats.write_count);
        printf("Read count: %lu\n", stats.read_count);
        printf("Fork count: %lu\n", stats.fork_count);
    } else {
        perror("syscall");
    }
}

void test_io_throttle() {
    // Generar un PID aleatorio entre 1 y 50
    pid_t pid = rand() % 1000 + 1;

    struct my_io_stats stats;

    int ret = syscall(SYS_get_io_throttle, pid, &stats);

    if (ret == 0) {
        printf("\n+++ Estadísticas de I/O para el PID %d +++\n", pid);
        printf("Bytes leídos: %lu\n", stats.bytes_read);
        printf("Bytes escritos: %lu\n", stats.bytes_written);
        printf("Bytes cancelados (escritos): %lu\n", stats.bytes_cancelled);
        printf("Fallos de página mayores: %lu\n", stats.maj_flt);
        printf("Fallos de página menores: %lu\n", stats.min_flt);
    } else {
        printf("\n+++ Estadísticas de I/O para el PID %d +++\n", pid);
        perror("syscall");
    }
}

int main() {

    srand(time(NULL));

    // Ejecutar las tres pruebas
    test_memory_snapshot();
    test_syscall_usage();
    test_io_throttle();

    return 0;
}
