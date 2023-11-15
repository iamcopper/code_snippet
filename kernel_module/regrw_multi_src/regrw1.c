#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>

#define ADDR_BASE 0x28180200
#define ADDR_SIZE 8

volatile uint32_t *regs = NULL;

static int __init regrw_init(void)
{
	/* ioremap 由物理地址转换为虚拟地址 */
	regs = (volatile uint32_t *)ioremap(ADDR_BASE, ADDR_SIZE);

	/* 读取寄存器 */
	printk(KERN_INFO "regs[0] = %x\n", regs[0]);
	/* 修改寄存器 */
	regs[0] |= 0x22000000;

	/* 再次读取寄存器 */
	printk(KERN_INFO "regs[0] = %x\n", regs[0]);
	printk(KERN_INFO "regs[4] = %x\n", regs[4]);

	return 0;
}

module_init(regrw_init);
MODULE_AUTHOR("iamcopper<kangpan519@gmail.com>");
MODULE_LICENSE("GPL v2");
