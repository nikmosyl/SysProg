#include <linux/module.h>       /* Необходим для любого модуля ядра */
#include <linux/kernel.h>       /* Здесь находится определение KERN_ALERT */
#include <linux/init.h>         /* Здесь находятся определения макросов */

#define DRIVER_AUTHOR "niksmos iu4-27"

static int __init hello_2_init(void)
{
        printk(KERN_ALERT "Hello, world 2!!!\n");
        return 0;
}

static void __exit hello_2_exit(void)
{
        printk(KERN_ALERT "Poka 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
