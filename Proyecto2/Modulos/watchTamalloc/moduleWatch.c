#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moises");
MODULE_DESCRIPTION("Modulo para obtener VmSize, VmRSS y OOMScore de un PID");
MODULE_VERSION("1.0");

#define PROC_NAME "watch_202010833"

static int watch_pid = -1; // PID a monitorear (-1 para ninguno)

// Funcion para mostrar informacion del PID
static int watch_pid_usage_show(struct seq_file *m, void *v) {

    struct task_struct *task;
    struct mm_struct *mm;

    if (watch_pid == -1) {
        seq_printf(m, "{ \"error\": \"No se ha configurado un PID.\" }\n");
        return 0;
    }

    // Buscar el proceso por PID
    rcu_read_lock(); // Protege las operaciones de lectura en estructuras del kernel
    task = pid_task(find_vpid(watch_pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        seq_printf(m, "{ \"error\": \"PID no válido o proceso no encontrado.\" }\n");
        return 0;
    }

    // Acceder a la memoria del proceso
    mm = task->mm;
    if (!mm) {
        rcu_read_unlock();
        seq_printf(m, "{ \"error\": \"El proceso no tiene espacio de memoria asignado.\" }\n");
        return 0;
    }

    // Extraer estadísticas de memoria
    // Calcular VmSize y VmRSS
    unsigned long vmsize = mm->total_vm << (PAGE_SHIFT - 10); // Convertir de páginas a KB
    unsigned long vmrss = get_mm_rss(task->mm) * (PAGE_SIZE/1024); // Páginas residenciales a KB

    seq_printf(m, "{\n");
    seq_printf(m, "  \"VmSize\": \"%lu kB\",\n",vmsize );
    seq_printf(m, "  \"VmRSS\": \"%lu kB\",\n", vmrss);
    seq_printf(m, "  \"OOM_Score\": \"%d\"\n", task->signal->oom_score_adj);
    seq_printf(m, "}\n");

    rcu_read_unlock();
    return 0;
}

static ssize_t watch_pid_usage_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos) {
    char input[16];
    
    if (count >= sizeof(input)) {
        printk(KERN_WARNING "El valor es demasiado grande.\n");
        return -EINVAL;
    }

    if (copy_from_user(input, buffer, count)) {
        printk(KERN_WARNING "Error al copiar datos desde el espacio de usuario.\n");
        return -EFAULT;
    }

    input[count] = '\0'; // Asegurarse de que el string esté terminado en NULL.

    if (kstrtoint(input, 10, &watch_pid)) {
        printk(KERN_WARNING "Error al convertir el PID.\n");
        return -EINVAL;
    }

    printk(KERN_INFO "Nuevo PID a monitorear: %d\n", watch_pid);

    return count;
}

static int watch_pid_usage_open(struct inode *inode, struct file *file) {
    return single_open(file, watch_pid_usage_show, NULL);
}

static const struct proc_ops watch_pid_usage_ops = {
    .proc_open = watch_pid_usage_open,
    .proc_read = seq_read,
    .proc_write = watch_pid_usage_write,
};

static int __init watch_pid_usage_init(void) {
    proc_create(PROC_NAME, 0, NULL, &watch_pid_usage_ops);
    printk(KERN_INFO "watch usage module loaded\n");
    return 0;
}

static void __exit watch_pid_usage_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "watch usage module unloaded\n");
}

module_init(watch_pid_usage_init);
module_exit(watch_pid_usage_exit);
