#ifndef MY_TRACK_SYSCALL_H
#define MY_TRACK_SYSCALL_H
#include <linux/atomic.h>

// Declaraciones globales de contadores
extern atomic_t syscall_open_count;
extern atomic_t syscall_write_count;
extern atomic_t syscall_read_count;
extern atomic_t syscall_fork_count;

struct syscall_usage_stats {
    unsigned long open_count;   // Contador para llamadas a open()
    unsigned long write_count;  // Contador para llamadas a write()
    unsigned long read_count;   // Contador para llamadas a read()
    unsigned long fork_count;   // Contador para llamadas a fork()
};

#endif // MY_TRACK_SYSCALL_H
