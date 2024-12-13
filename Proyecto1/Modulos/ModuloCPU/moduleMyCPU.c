#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moises");
MODULE_DESCRIPTION("Modulo para mostrar el uso de la CPU");
MODULE_VERSION("1.0");

#define PROC_NAME "cpu_usage"

// Estructura para almacenar el uso anterior de la CPU
static unsigned long prev_total_time = 0;
static unsigned long prev_idle_time = 0;

static int cpu_usage_show(struct seq_file *m, void *v) {
    unsigned long user_time, nice_time, system_time, idle_time, iowait_time, irq_time, softirq_time, total_time;
    unsigned long total_time_diff, idle_time_diff;
    unsigned long cpu_usage;

    // Abrir el archivo /proc/stat y leer la primera línea
    struct file *file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        seq_printf(m, "Error al abrir /proc/stat\n");
        return PTR_ERR(file);
    }

    // Leer la línea correspondiente al uso de la CPU
    char buf[256];
    loff_t pos = 0;
    ssize_t len = kernel_read(file, buf, sizeof(buf) - 1, &pos);
    filp_close(file, NULL);

    if (len <= 0) {
        seq_printf(m, "Error al leer /proc/stat\n");
        return -1;
    }

    // Asegurarse de que se leyó correctamente la línea
    buf[len] = '\0';

    // Procesar la primera línea de /proc/stat
    if (sscanf(buf, "cpu  %lu %lu %lu %lu %lu %lu %lu", 
               &user_time, &nice_time, &system_time, &idle_time,
               &iowait_time, &irq_time, &softirq_time) != 7) {
        seq_printf(m, "Error al analizar la línea de CPU\n");
        return -1;
    }

    // Calcular el total de tiempo y la diferencia desde la última lectura
    total_time = user_time + nice_time + system_time + idle_time + iowait_time + irq_time + softirq_time;
    total_time_diff = total_time - prev_total_time;
    idle_time_diff = idle_time - prev_idle_time;

    // Calcular el porcentaje de uso de la CPU
    cpu_usage = 100 * (total_time_diff - idle_time_diff) / total_time_diff;

    // Actualizar los tiempos previos para la siguiente iteración
    prev_total_time = total_time;
    prev_idle_time = idle_time;

    // Imprimir el resultado en formato JSON
    seq_printf(m, "{\n");
    seq_printf(m, "  \"cpu_usage\": \"%lu%%\"\n", cpu_usage);
    seq_printf(m, "}\n");

    return 0;
}

static int cpu_usage_open(struct inode *inode, struct file *file) {
    return single_open(file, cpu_usage_show, NULL);
}

static const struct proc_ops cpu_usage_ops = {
    .proc_open = cpu_usage_open,
    .proc_read = seq_read,
};

static int __init cpu_usage_init(void) {
    proc_create(PROC_NAME, 0, NULL, &cpu_usage_ops);
    printk(KERN_INFO "CPU usage module loaded\n");
    return 0;
}

static void __exit cpu_usage_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "CPU usage module unloaded\n");
}

module_init(cpu_usage_init);
module_exit(cpu_usage_exit);
