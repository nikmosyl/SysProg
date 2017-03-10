#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
	read: device_read,
	write: device_write,
	open: device_open,
	release: device_release
};

int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0)
	{
		printk("Registering the character device failed with %d\n", Major);
		return Major;
	}

	printk("<1>I was assigned major number %d.  To talk to\n", Major);
	printk("<1>the driver, create a dev file with\n");
	printk("'mknod /dev/chardev c %d 0'.\n", Major);
	printk("<1>Try various minor numbers.  Try to cat and echo to\n");
	printk("the device file.\n");
	printk("<1>Remove the device file and module when done.\n");

	return 0;
}

void cleanup_module(void)
{
	int ret = unregister_chrdev(Major, DEVICE_NAME);
	if (ret < 0)
	{
		printk("Error in unregister_chrdev: %d\n", ret);
	}
}


//Pri otkritii faila ustroistva

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	if (Device_Open)
	return -EBUSY;
	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

//Pri zakritii faila ustroistva processom

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;
	module_put(THIS_MODULE);

	return 0;
}


//Pri vizove otkritogo faila

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{

	int bytes_read = 0;			//kol-vo bait v buffere
	if (*msg_Ptr == 0)
	{
		return 0;			//vihod pri simvole konca stroki
	}

	while (length && *msg_Ptr) 		//peremeshenie dannih v buffer
	{
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	return bytes_read;
}

//Pri zapisi dannih v process

static ssize_t

device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk("<1>Sorry, this operation isn't supported.\n");
	return -EINVAL;
}
