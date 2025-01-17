#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
//#include <linux/kernel.h>

static int hello_init(void)
{
	//printk(KERN_ALERT "Hello world\n");
	//printk(KERN_ALERT "VAR1=%d\n", VAR1);
	pr_debug("[DEBUG] init func=%s\n", __func__);
	return 0;
}

static void hello_exit(void)
{
	//printk(KERN_ALERT "Goodbye world\n");
	pr_debug("[DEBUG] exit func=%s\n", __func__);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("iamcopper<kangpan519@gmail.com>");
MODULE_LICENSE("GPL v2");
