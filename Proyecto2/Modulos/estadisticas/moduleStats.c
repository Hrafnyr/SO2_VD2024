#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moises");
MODULE_DESCRIPTION("Módulo para obtener estadísticas de procesos y exportarlas en formato JSON");
MODULE_VERSION("1.0");

#define PROC_NAME "statsAllProcs"

// Estructura para almacenar la información del proceso
struct proc_stats {
    pid_t pid;
    char name[TASK_COMM_LEN];  // Nombre del proceso
    unsigned long vm_size;   // Memoria reservada (VmSize)
    unsigned long vm_rss;    // Memoria utilizada (VmRSS)
    int oom_score;    // OOM Score
};

// Función para obtener las estadísticas de un proceso específico
static int get_proc_stats(struct task_struct *task, struct proc_stats *stats) {
    struct mm_struct *mm;

    mm = task->mm;
    if (!mm) return -1;

    // Copiar el nombre del proceso y  calcular VmSize y VmRSS
    strncpy(stats->name, task->comm, TASK_COMM_LEN);
    stats->vm_size = mm->total_vm << (PAGE_SHIFT - 10); // Convertir de páginas a KB
    stats->vm_rss = get_mm_rss(task->mm) * (PAGE_SIZE/1024); // Páginas residenciales a KB

    // Obtener el OOM score
    stats->oom_score = task->signal->oom_score_adj;

    // Obtener el PID correcto
    stats->pid = task->pid;
    
    return 0;
}

// Función para mostrar estadísticas en formato JSON
static int procs_stats_show(struct seq_file *m, void *v) {
    struct task_struct *task;
    struct proc_stats stats;
    unsigned long total_vm_size = 0, total_vm_rss = 0;
    int total_procs = 0;

    seq_printf(m, "{\n");
    seq_printf(m, "  \"procs\": [\n");

    rcu_read_lock();
    for_each_process(task) {
        if (get_proc_stats(task, &stats) == 0) {
            // Mostrar estadísticas del proceso en formato JSON
            if (total_procs > 0) seq_printf(m, ",\n");
            seq_printf(m, "    {\n");
            seq_printf(m, "      \"pid\": %d,\n", stats.pid);
            seq_printf(m, "      \"name\": \"%s\",\n", stats.name);
            seq_printf(m, "      \"vm_size (KB)\": %lu,\n", stats.vm_size);
            seq_printf(m, "      \"vm_rss (KB)\": %lu,\n", stats.vm_rss);
            seq_printf(m, "      \"oom_score\": %d\n", stats.oom_score);
            seq_printf(m, "    }");

            total_vm_size += stats.vm_size;
            total_vm_rss += stats.vm_rss;
            total_procs++;
        }
    }
    rcu_read_unlock();

    // Resumen global
    seq_printf(m, "\n  ],\n");
    seq_printf(m, "  \"dataGlobal\": {\n");
    seq_printf(m, "    \"total_vm_size(KB)\": %lu,\n", total_vm_size);
    seq_printf(m, "    \"total_vm_rss(KB)\": %lu,\n", total_vm_rss);
    seq_printf(m, "    \"total_procs\": %d\n", total_procs);
    seq_printf(m, "  }\n");
    seq_printf(m, "}\n");

    return 0;
}

// Funciones de apertura y cierre del archivo /proc
static int procs_stats_open(struct inode *inode, struct file *file) {
    return single_open(file, procs_stats_show, NULL);
}

static const struct proc_ops procs_stats_ops = {
    .proc_open = procs_stats_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Inicialización del módulo
static int __init procs_stats_init(void) {
    proc_create(PROC_NAME, 0, NULL, &procs_stats_ops);
    printk(KERN_INFO "Módulo procs_stats cargado\n");
    return 0;
}

// Salida del módulo
static void __exit procs_stats_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Módulo procs_stats descargado\n");
}

module_init(procs_stats_init);
module_exit(procs_stats_exit);
