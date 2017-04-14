#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_AUTHOR( "NikMos IU4_27");
MODULE_LICENSE( "MosCorp." );

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
/*loff_t Метод llseek используется для изменения текущего положения позиции чтения/записи
в файле. При успешном выполнения функция возвращает положительное значение новой позиции.*/
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0				//
#define DEVICE_NAME "chardev"			//Название устройства
#define BUF_LEN 80				//Максимальный размер месаги

static int Major;				//Верхний номер устройства
static int Device_Open = 0; 			//количество удачных попыток открытия
static char msg[BUF_LEN];			//буфер текста сообщения
static char *msg_Ptr;				//текущий символ
static char fullMsg[BUF_LEN];			//массив для хранения посланной пользователем строки
static int fullMsgCounter = 0;			//количество переданных пользователем символов

static struct file_operations fops = {		//создаем элемент структуры с
	.read = device_read,			//переобозначением библиотечных команд
	.write = device_write,			//в свои
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	printk(KERN_ALERT"    CHARDEV IS LOAD   ");

	Major = register_chrdev(0, DEVICE_NAME, &fops); //получаем Major Number

	if (Major < 0)
	{
		printk("Регистрация устройства провалена с Major = %d\n", Major);
		return Major;
	}

	printk("Для работы с устройством выполни команду:\n");
	printk("'mknod /dev/chardev c %d 0'.\n", Major);
	printk("Major %d\n", Major);
	printk("Чтоы узнать заработал ли модуль:\n");
	printk("'cat /dev/chardev'\n");
	printk("Чтобы отправить данные в устройство:\n");
	printk("'echo \"hi\" > /dev/chardev '\n");
	printk("Результаты можно посмотреть вот так:\n");
	printk("'grep \".*\" /var/log/kern.log'\n");
	printk("Не забудь удалить файл уст-ва и выгрузить модуль.\n");

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_ALERT "    CHARDEV IS UNLOAD   \n\n\n\n");
	unregister_chrdev(Major, DEVICE_NAME);
}

//Если открыть устройство командой cat /dev/chardev

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 1;				//счётчик попыток открытия

	if (Device_Open)
	{
		printk("Устройство занято\n");
		return -EBUSY;
	}
	Device_Open++;
	if(fullMsgCounter==0)
	{
		sprintf(msg, "Я уже сказал тебе %d раз: I LOADED!\n", counter++);
	}
	else
	{
		sprintf(msg, fullMsg);
		counter--;
	}
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

//Если закрывается файл устройства

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;				//Сигнализируем о выходе из устройства и о его свободе
	module_put(THIS_MODULE);
	return 0;
}

//При чтении данных с утройства

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	int bytes_read = 0;						//Количество байт в буфере
	fullMsgCounter = 0;

	if (*msg_Ptr == 0)						//При достижении
	{
		return 0;
	}

	while (length && *msg_Ptr) 					//Запись сообщения в буфер
	{
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	return bytes_read;
}

//Pri zapisi dannih v process

static ssize_t device_write(struct file *file, const char __user * buffer, size_t length, loff_t * offset)
{
	int i;

	for (i = 0; i < length && i < BUF_LEN; i++)
	{
		get_user(msg[i], buffer+i);
		printk("%c\n", msg[i]);
		fullMsg[i] = msg[i];
		fullMsgCounter = i;
	}

	msg_Ptr = msg;

	return i;							//Вернуть количество принятых байт
}

