#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>

extern volatile uint32_t *regs;

static void __exit regrw_exit(void)
{
	iounmap(regs);
}

module_exit(regrw_exit);
