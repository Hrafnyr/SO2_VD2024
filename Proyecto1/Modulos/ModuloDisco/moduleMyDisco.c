#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/statfs.h>
#include <linux/path.h>
#include <linux/namei.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Moises");
MODULE_DESCRIPTION("Modulo para mostrar el uso de disco");
MODULE_VERSION("1.0");

#define PROC_NAME "myDiskInfo_202010833"

// Función para calcular el tamaño usado y total
static int sysinfo_disk_show(struct seq_file *m, void *v) {
    struct kstatfs stat;
    struct file *file;
    unsigned long total, free, used;

    // Ruta del dispositivo montado
    const char *mount_path = "/";

    // Abrir el directorio raíz
    file = filp_open(mount_path, O_RDONLY, 0);
    if (IS_ERR(file)) {
        seq_printf(m, "Error: no se pudo abrir la ruta %s\n", mount_path);
        return PTR_ERR(file);
    }

    // Obtener información del sistema de archivos
    if (vfs_statfs(&file->f_path, &stat)) {
        seq_printf(m, "Error: no se pudo obtener la información del sistema de archivos\n");
        filp_close(file, NULL);
        return -EIO;
    }

    // Calcular tamaños en bytes
    total = stat.f_blocks * stat.f_bsize;
    free = stat.f_bfree * stat.f_bsize;
    used = total - free;

    // Convertir a MB para evitar punto flotante
    seq_printf(m, "Dispositivo montado: %s\n", mount_path);
    seq_printf(m, "Tamaño total: %lu MB\n", total / (1024 * 1024));
    seq_printf(m, "Espacio usado: %lu MB\n", used / (1024 * 1024));
    seq_printf(m, "Espacio disponible: %lu MB\n", free / (1024 * 1024));

    filp_close(file, NULL);
    return 0;
}

static int disk_usage_open(struct inode *inode, struct file *file) {
    return single_open(file, sysinfo_disk_show, NULL);
}

static const struct proc_ops disk_usage_ops = {
    .proc_open = disk_usage_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init disk_usage_init(void) {
    proc_create(PROC_NAME, 0, NULL, &disk_usage_ops);
    printk(KERN_INFO "DISK usage module loaded\n");
    return 0;
}

static void __exit disk_usage_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "DISK usage module unloaded\n");
}

module_init(disk_usage_init);
module_exit(disk_usage_exit);
