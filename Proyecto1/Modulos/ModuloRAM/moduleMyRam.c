#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sysinfo.h>
#include <linux/uaccess.h>
#include <linux/mm.h>       // funciones para manejar la memoria
#include <linux/sched.h>    // funciones para manejar los procesos

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moises");
MODULE_DESCRIPTION("Módulo de Estadísticas de Memoria");
MODULE_VERSION("1.0");

#define PROC_NAME "memory_stats"

static int memory_stats_show(struct seq_file *m, void *v) {
    struct sysinfo si;
    si_meminfo(&si);

    unsigned long total_ram = si.totalram * (PAGE_SIZE / 1024);
    unsigned long free_ram = si.freeram * (PAGE_SIZE / 1024);
    unsigned long used_ram = total_ram - free_ram;

    seq_printf(m, "{\n");
    seq_printf(m, "  \"total_ramKB\": \"%lu\",\n", total_ram);
    seq_printf(m, "  \"free_ramKB\": \"%lu\",\n", free_ram);
    seq_printf(m, "  \"used_ramKB\": \"%lu\"\n", used_ram);
    seq_printf(m, "}\n");

    return 0;
}

static int memory_stats_open(struct inode *inode, struct file *file) {
    return single_open(file, memory_stats_show, NULL);
}

static const struct proc_ops memory_stats_ops = {
    .proc_open = memory_stats_open,
    .proc_read = seq_read,
};

static int __init memory_stats_init(void) {
    proc_create(PROC_NAME, 0, NULL, &memory_stats_ops);
    printk(KERN_INFO "Memory stats module loaded\n");
    return 0;
}

static void __exit memory_stats_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Memory stats module unloaded\n");
}

module_init(memory_stats_init);
module_exit(memory_stats_exit);
