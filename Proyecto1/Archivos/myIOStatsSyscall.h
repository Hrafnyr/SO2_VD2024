#ifndef _LINUX_IO_STATS_H
#define _LINUX_IO_STATS_H

struct my_io_stats {
    unsigned long bytes_read;        // Bytes leídos
    unsigned long bytes_written;     // Bytes escritos
    unsigned long bytes_cancelled;   // Bytes Cancelados
    unsigned long maj_flt;   // Fallos de página mayores
    unsigned long min_flt;       // Fallos de página menores
};

#endif /* _LINUX_IO_STATS_H */
