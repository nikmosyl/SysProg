#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_AUTHOR( "NikMos IU4_27");
MODULE_LICENSE( "MosCorp." );


int init_module(void)
{
        printk(KERN_ALERT"\n\n");
        return 0;
}

void cleanup_module(void)
{
        printk(KERN_ALERT "\n");
}
