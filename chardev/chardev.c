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
static char msg[BUF_LEN];			//бувер текста сообщения
static char *msg_Ptr;				//текущий символ

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
}





//Если открыть устройство командой cat /dev/chardev

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 1;				//счётчик попыток открытия

	printk(KERN_ALERT"Выполнен вход в фунцию device_open\n");

	if (Device_Open)
	{
		printk("Устройство занято\n");
		return -EBUSY;
	}
	Device_Open++;
	sprintf(msg, "Я уже сказал тебе %d раз: I LOADED!\n", counter++);
	printk("DEBUG-2! После вывода сообщения\n");
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	printk(KERN_ALERT"DEBUG Сейчас будет выполен return SUCCESS функция device_open\n\n");
	return SUCCESS;
}

//Если закрывается файл устройства

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT"Выполнен вход в фунцию device_release\n");
	Device_Open--;				//Сигнализируем о выходе из устройства и о его свободе
	module_put(THIS_MODULE);
	printk(KERN_ALERT"Выход из функции device-release Файл устройства закрыт\n\n");
	return 0;
}


//При чтении данных с утройства

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	int bytes_read = 0;						//Количество байт в буфере

	printk(KERN_ALERT"Выполнен вход в фунцию device_Read\n");

	if (*msg_Ptr == 0)						//При достижении 
	{
		printk("Конец сообщения выход из функции device_Read\n\n");
		return 0;
	}

	while (length && *msg_Ptr) 					//Запись сообщения в буфер
	{
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	printk(KERN_ALERT"DEBUG сейчас будет возвращен bytes_read из функции device_Read\n\n");
	return bytes_read;
}

//Pri zapisi dannih v process

static ssize_t device_write( struct file *filp, const char *buff, size_t len, loff_t * off )
{
	int byte_write = 0;

	printk(KERN_ALERT"DEBUG вход в функцию device_Write\n\n");

	if ( *msg_Ptr == 0 )
	{
		printk("DEBUG *msg_Ptr=0 Сейчас будет выполнен return 0 выход из функции device_Write\n\n");
		return 0;
	}

	while ( len && *msg_Ptr )
	{
		printk("%c", *buff++);
		len--;
		byte_write++;
	}
	printk(KERN_ALERT"DEBUG сейчас будет возвращен bytes_write из функции device_Write\n\n");
	return byte_write;
	//return -EINVAL;
}
