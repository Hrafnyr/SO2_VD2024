#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

#define SYS_MOISES_TAMALLOC 551

int main() {

    printf("Programa para tamalloc PID: %d\n", getpid());
    printf("Este programa aloja memoria con tamalloc. ENTER para continuar...\n");
    getchar();

    size_t total_size = 10 * 1024 * 1024; // 10 MB

    // Llamada a la syscall
    char *buffer = (char *)syscall(SYS_MOISES_TAMALLOC, total_size);

    if ((long)buffer < 0) {
        perror("tamalloc error");
        return 1;
    }

    printf("Se alojan 10MB de memoria en la direccion: %p\n", buffer);
    printf("Press ENTER to start reading memory byte by byte...\n");
    getchar();

    srand(time(NULL)); // Inicializa el generador de números aleatorios con la hora actual

    // Lee la memoria byte por byte para verificar que fue inicializada a cero
    for (size_t i = 0; i < total_size; i++) {
        char t = buffer[i]; // Accede al byte en la posición i, esto activa la asignación perezosa (Lazy allocation)

        if (t != 0) { // Si no es cero, significa que no se inicializó correctamente
            printf("ERROR FATAL: Memory at byte %zu was not initialized to 0\n", i);
            return 10;
        }

        // Escribe una letra aleatoria A-Z para activar la técnica Copy-on-Write (CoW)
        char random_letter = 'A' + (rand() % 26); 
        buffer[i] = random_letter; 

        // Cada 1 MB, imprime el progreso y espera 1 segundo
        if (i % (1024 * 1024) == 0 && i > 0) { 
            printf("Revisado %zu MB...\n", i / (1024 * 1024));
            sleep(2);
        }
    }

    printf("Toda la memoria verificada inicializacion en cero.ENTER para salir.\n");
    getchar(); 

    return 0;
}