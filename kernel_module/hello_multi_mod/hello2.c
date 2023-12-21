#define DEBUG

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

/* Access symbols exported from mod1 */
extern const char *hellostr;
extern void echo_func(const char *str);

static int hello_init(void)
{
	//printk(KERN_ALERT "Hello world\n");
	//printk(KERN_ALERT "VAR1=%d\n", VAR1);
	//pr_debug("[%s-DEBUG] init func=%s\n", module_name(THIS_MODULE), __func__);
	pr_info("[%s-INFO] in init=%s\n", THIS_MODULE->name, __func__);
	pr_info("[%s-INFO] hellostr=%s\n", THIS_MODULE->name, hellostr);
	echo_func(hellostr);

	return 0;
}

static void hello_exit(void)
{
	//printk(KERN_ALERT "Goodbye world\n");
	pr_debug("[%s-DEBUG] in exit=%s\n", THIS_MODULE->name, __func__);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("iamcopper<kangpan519@gmail.com>");
MODULE_LICENSE("GPL v2");
