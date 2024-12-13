#ifndef _LINUX_MYMEMORY_H
#define _LINUX_MYMEMORY_H

#include <linux/types.h>

struct my_memory_snapshot {
    unsigned long paginas_activas;
    unsigned long paginas_inactivas;
    unsigned long memoria_cache;
    unsigned long memoria_sucia;
    unsigned long paginas_en_disco;
    unsigned long swap_total;
    unsigned long swap_libre;
};

#endif /* _LINUX_MYMEMORY_H */
