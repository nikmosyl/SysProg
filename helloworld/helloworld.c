#include <linux/module.h>
#include <linux/kernel.h>

MODULE_AUTHOR( "NikMos IU4_27");
MODULE_LICENSE( "MosCorp." );

int init_module(void)
{
	printk(KERN_ALERT "Hello world!!!\n");
	return 0;
}
void cleanup_module(void)
{
	printk(KERN_ALERT "Poka!!!\n");
}
