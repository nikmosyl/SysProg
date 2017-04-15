#include <linux/module.h>								//для модуля
#include <linux/kernel.h>								//для раоты с ядром
#include <linux/fs.h>
#include <asm/uaccess.h>								//гет и пут юзер

#include "chardev.h"
#define SUCCESS 0
#define DEVICE_NAME "mydev"
#define BUF_LEN 80


static int Device_Open = 0;								//флаг открытия устройтсва
static char Message[BUF_LEN];								//ответ на запрос

static char *Message_Ptr;								//позиция в буфере, нужна если сообщение переполнит буфер


//При открытии файла устройства
static int device_open(struct inode *inode, struct file *file)
{
	printk("device_open(%p)\n", file);

	if (Device_Open)
	{
		return -EBUSY;
	}

	Device_Open++;
	Message_Ptr = Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

//При закрытии файла устройства
static int device_release(struct inode *inode, struct file *file)
{
	printk("device_release(%p,%p)\n", inode, file);
	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

//При считывании данных из файла процессом
static ssize_t device_read(struct file *file, char __user * buffer, size_t length, loff_t * offset)
{
	int bytes_read = 0;							//кол-во байт в буфере

	printk("device_read(%p,%p,%d)\n", file, buffer, length);

	if (*Message_Ptr == 0)							//если достигнут конец строки
	{
		return 0;
	}

	while (length && *Message_Ptr)
	{
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	printk("Прочитано %d байт, %d потеряно\n", bytes_read, length);

	return bytes_read;							//колчество байт закинутых в буфер
}

//При записи в файл устройства
static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
	int i;
	printk("device_write(%p,%d)", file, length);

	for (i = 0; i < length && i < BUF_LEN; i++)
	{
		get_user(Message[i], buffer + i);
	}

	Message_Ptr = Message;
	return i;								//количество принятых байт
}

//При выполнении процессом операции ioctl
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i;
	char *temp;
	char ch;

	switch (ioctl_num)
	{
		case IOCTL_SET_MSG:						//переписать указатель на сообщение в буфер по адресу из аргумента
			temp = (char *)ioctl_param;
			get_user(ch, temp);

			for (i = 0; ch && i < BUF_LEN; i++, temp++)
			{
				get_user(ch, temp);
			}

			device_write(file, (char *)ioctl_param, i, 0);
		break;

		case IOCTL_GET_MSG:						//записать сообщение по адресу из аргумента
			i = device_read(file, (char *)ioctl_param, 99, 0);
			put_user('\0', (char *)ioctl_param + i);
		break;

		case IOCTL_GET_NTH_BYTE:					//ввод параметры и вывод значения функции
			return Message[ioctl_param];
		break;
	}

	return SUCCESS;
}

struct file_operations Fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,
};

int init_module()
{
	int ret_val;

	printk(KERN_ALERT"Устройство mydev загружено!\n");

	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	if (ret_val < 0)
	{
		printk("%s провал с %d\n", "Извините, регистрация символьного устройства ", ret_val);
		return ret_val;
	}

	printk("Регистрация удалась. Major number устройства: %d.\n", MAJOR_NUM);
	printk("Используй команду:\n");
	printk("mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);

	return 0;
}

void cleanup_module()
{
	printk(KERN_ALERT"Устройство mydev выгружено!\n\n\n");
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

