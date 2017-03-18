//Исходный файл создания файла в файловой системе /proc как для чтения, та и для записи

#include <linux/module.h>  				//билиотека для модулей
#include <linux/kernel.h>  				//для работы с ядром
#include <linux/proc_fs.h> 				//для работы с файловой системой /proc
#include <linux/sched.h>				//для euid
#include <asm/uaccess.h>   				//функции get_user и put_user для обращения к адресному пространствку пользоватля
#include <linux/net.h>
#include </usr/src/linux-4.9.11/fs/proc/internal.h>

/* 
 * Место хранения последнего принятого сообщения,
 * которое будет выводиться в файл, чтобы показать, что
 * модуль действительно может получать ввод от пользователя
 */

#define MESSAGE_LENGTH 80
static char Message[MESSAGE_LENGTH];
static struct proc_dir_entry *Proc_File;

#define PROC_ENTRY_FILENAME "rw_test"

static ssize_t module_output(struct file *filp, char *buffer,/*буфер с данными*/ size_t length,/*размер буфера*/ loff_t * offset)
{
	static int finished = 0;
	int i;
	char message[MESSAGE_LENGTH + 30];

	if (finished)					//если файл закончился возвращаем 0
	{
		finished = 0;
		return 0;
	}

	sprintf(message, "Last input:%s", Message);

	for (i = 0; i < length && message[i]; i++)
	{
		put_user(message[i], buffer + i);
	}

	finished = 1;
	return i;					//возвращает количкство принятых байт
}

static ssize_t module_input(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	int i;
  /* 
   * Переместить данные, полученные от пользователя в буфер, 
   * который позднее будет выведен функцией module_output.
   */
	for (i = 0; i < MESSAGE_LENGTH - 1 && i < len; i++)
	{
		get_user(Message[i], buff + i);
	}

	Message[i] = '\0';
	return i;
}



/* 
 * Эта функция принимает решение о праве на выполнение операций с файлом
 * 0 -- разрешено, ненулеое значение -- запрещено.
 *
 * Операции с файлом могут быть:
 * 0 - Исполнениеe (не имеет смысла в нашей ситуации)
 * 2 - Запись (передача от пользователя к модулю ядра)
 * 4 - Чтение (передача от модуля ядра к пользователю)
*/

static int module_permission(struct inode *inode, int op /*, struct nameidata *foo*/)
{
	if (op == 4 || (op == 2 /*&& current->euid == 0*/))			//читать - кто угодно, писать - только root
	{
		return 0;
	}

	return -EACCES;
}

//При ткрытии файла

int module_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

//При закрытии файла

int module_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations File_Ops_for_Proc_File = {
        .read = module_output,
        .write = module_input,
        .open = module_open,
        .release = module_close,
};

/*
 * Операции над индексной записью нашего файла. Необходима
 * для того, чтобы указать местоположение структуры
 * file_operations нашего файла, а так же, чтобы задать адрес
 * функции определения прав доступа к файлу. Здесь можно указать адреса
 * других функций-обработчиков, но нас они не интересуют.
 */

static struct inode_operations Inode_Ops_for_Proc_File = {
	.permission = module_permission,
};


//При создании и инициализации модуля

int init_module()
{
	int rv = 0;
	Proc_File = proc_create(PROC_ENTRY_FILENAME, 0644, NULL, &File_Ops_for_Proc_File);
	//Proc_File->owner = THIS_MODULE;
	Proc_File->proc_iops = &Inode_Ops_for_Proc_File;
	Proc_File->proc_fops = &File_Ops_for_Proc_File;
	Proc_File->mode = S_IFREG | S_IRUGO | S_IWUSR;
	//Proc_File->uid = 0;
	//Proc_File->gid = 0;
	Proc_File->size = 80;

	printk(KERN_ALERT"Модуль файла фс /proc загружен!/n");

	if (Proc_File == NULL)
	{
		rv = -ENOMEM;
		remove_proc_entry(PROC_ENTRY_FILENAME, NULL/*&proc_root*/);
		printk(KERN_INFO "Error: Could not initialize /proc/test\n");
	}

	return rv;
}

//При удалении модуля

void cleanup_module()
{
	remove_proc_entry(PROC_ENTRY_FILENAME, NULL/*&proc_root*/);
}

