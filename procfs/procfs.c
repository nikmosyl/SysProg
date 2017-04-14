#include <linux/module.h>						// либа для работы с модулем
#include <linux/kernel.h>						// либа для работы с ядром
#include <linux/proc_fs.h>						// для работы с proc
#include <asm/uaccess.h>						// либа для get_user и put_user
#include <linux/fs.h>							// либа для proc_dir_entry
#include </usr/src/linux-4.9.11/fs/proc/internal.h>			// либа для file_operations и inode_operations

#define MESSAGE_LENGTH 80
static char Message[MESSAGE_LENGTH];
static struct proc_dir_entry *Our_Proc_File;
int porogVhogdenia;

#define PROC_ENTRY_FILENAME "myfile"

static ssize_t module_output(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	static int finished = 0;
	int i;								// количество принятых байт
	char message[MESSAGE_LENGTH + 30];

	if (finished)							// если буфер закончился, выходим из цикла чтения
	{
		finished = 0;
		return 0;
	}
	if(porogVhogdenia == 0)
	{
		sprintf(message, "В файл myfile ещё ничего не приходило...%s\n", Message);
	}
	else
	{
		sprintf(message, "Последняя введенная строка: %s", Message);
	}

	for (i = 0; i < length && message[i]; i++)
	{
		put_user(message[i], buffer + i);
	}
	finished = 1;

	return i;
}


static ssize_t module_input(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	int i;

	porogVhogdenia = 1;

	for (i = 0; i < MESSAGE_LENGTH - 1 && i < len; i++)
	{
		get_user(Message[i], buff + i);
	}

	return i;
}


static int module_permission(struct inode *inode, int op)		// функция доступа к модулю
{
	return 0;
}


int module_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);					// счётчик образщения к модулю
	return 0;
}


int module_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations File_Ops_for_Our_Proc_File = {
        .read = module_output,
        .write = module_input,
        .open = module_open,
        .release = module_close,
};


static struct inode_operations Inode_Ops_for_Our_Proc_File = {
  .permission = module_permission,
};


int init_module()
{
	int rv = 0;
	porogVhogdenia = 0;

	printk(KERN_ALERT "Инициализация модуля procfs, доступного в /proc/lab_proc_file\n");

	Our_Proc_File = proc_create(PROC_ENTRY_FILENAME, 0644, NULL, &File_Ops_for_Our_Proc_File);
	Our_Proc_File->proc_iops = &Inode_Ops_for_Our_Proc_File;
	Our_Proc_File->proc_fops = &File_Ops_for_Our_Proc_File;
	Our_Proc_File->mode = S_IFREG | S_IRUGO | S_IWUSR;
	Our_Proc_File->size = 80;

	if (Our_Proc_File == NULL)
	{
		rv = -ENOMEM;
		remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/test\n");
	}
	return rv;
}

void cleanup_module()
{
	printk(KERN_ALERT "Модуль procfs выгружен\n\n");
	remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
}

