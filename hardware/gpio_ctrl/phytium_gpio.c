#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <byteswap.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define GPIO0_BASE_ADDR 0x28004000
#define GPIO1_BASE_ADDR 0x28005000

#define GPIOA_OUT_OFFSET 0x00
#define GPIOA_DIRECTION_OFFSET 0x04
#define GPIOA_IN_OFFSET 0x08

#define GPIOB_OUT_OFFSET 0X0C
#define GPIOB_DIRECTION_OFFSET 0x10
#define GPIOB_IN_OFFSET 0x14

#define PINMUX_CONFIG_BASE_ADDR 0x28180000

#define PINMUX_CONFIG_OFFSET_0 0x0200
#define PINMUX_CONFIG_OFFSET_1 0X0204
#define PINMUX_CONFIG_OFFSET_2 0x0208
#define PINMUX_CONFIG_OFFSET_3 0x020c
#define PINMUX_CONFIG_OFFSET_4 0x0214
#define PINMUX_CONFIG_OFFSET_5 0x0218

#define FUNC1 0x01
#define FUNC2 0x10

#define	SET_BIT(x, bit)	(x |= (1 << bit)) 
#define	CLEAR_BIT(x, bit)	(x &= ~(1 << bit))	/* 清零第bit位 */
/* ltoh: little endian to host */
/* htol: host to little endian */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ltohl(x)       (x)
#define ltohs(x)       (x)
#define htoll(x)       (x)
#define htols(x)       (x)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ltohl(x)     __bswap_32(x)
#define ltohs(x)     __bswap_16(x)
#define htoll(x)     __bswap_32(x)
#define htols(x)     __bswap_16(x)
#endif

#pragma pack(1)//设置默认对齐数为1

typedef struct _gpio
{
	unsigned char flag;
	unsigned char GPIO_MODULE;
	unsigned char GPIO_GROUP;
	int GPIO_PIN;
}GPIO_NUM;


/*
 *
 * return :
 *  0:write successfully!
 *  !=0 : fialed
 */
int mmap_write(off_t target,uint32_t  writeval)
{
	off_t PageSize,PageMask;
	void *gpio_map;
	int gpio_fd;
	unsigned char ret = 0;
	off_t target_aligned,offset;
	gpio_fd = open("/dev/mem",O_RDWR);
	if(gpio_fd == -1)
	{
		printf("can't open /dev/mem!\n");
		ret = -errno;
		return ret;
	}
	PageSize = sysconf(_SC_PAGESIZE);
	PageMask = (~(PageSize - 1));

	target_aligned = target & PageMask;
	offset = target & (~ PageMask);

	gpio_map = mmap(NULL,offset+4,PROT_READ|PROT_WRITE,MAP_SHARED,gpio_fd,target_aligned);
	if(gpio_map == (void *)-1)
	{
		printf("Memory 0x%lx mapped failed: %s.\n",
				target, strerror(errno));
		ret = -errno;
		goto close;

	}

	gpio_map +=offset;
	//write
	writeval = htoll(writeval);
	*((uint32_t *) gpio_map) = writeval;

	gpio_map -= offset;
	if (munmap(gpio_map, offset+4) == -1) {
		printf("Memory 0x%lx mapped failed: %s.\n",
				target, strerror(errno));
		ret = -errno;
	}

close:
	close(gpio_fd);
	return ret;

}
/*
 * return : <0 failed
 * 	    >=0 read value
 */
uint32_t mmap_read(off_t target)
{
	off_t PageSize,PageMask;
	off_t target_aligned,offset;
	void *gpio_map;
	int gpio_fd;
	uint32_t ret;
	gpio_fd = open("/dev/mem",O_RDWR);
	if(gpio_fd == -1)
	{
		printf("can't open /dev/mem!\n");
		return -errno;
	}
	PageSize = sysconf(_SC_PAGESIZE);
	PageMask = (~(PageSize - 1));

	target_aligned = (target)& PageMask;
	offset = target & (~ PageMask);

	gpio_map = mmap(NULL,offset+4,PROT_READ|PROT_WRITE,MAP_SHARED,gpio_fd,target_aligned);
	if(gpio_map == (void *)-1)
	{
		printf("Memory 0x%lx mapped failed: %s.\n",
				target, strerror(errno));
		ret = -errno;
		goto close;

	}

	gpio_map +=offset;
	//read
	ret = *((uint32_t*)gpio_map);
	//printf("mmap_read 0x%lx is 0x%x\n",target,ret);
	ret = ltohl(ret);

	gpio_map -= offset;
	if (munmap(gpio_map, offset+4) == -1) {
		printf("Memory 0x%lx mapped failed: %s.\n",
				target, strerror(errno));
		ret = -errno;
	}

close:
	close(gpio_fd);
	return ret;

}

/******************************
 * return : 0:successfully
 * 	    !=0:failed
 *******************************/

int set_gpio_value(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP,uint32_t writevalue)
{
	off_t base_addr,offset,target;
	int ret = 0;
	if(GPIO_MODULE == 0)
		base_addr = GPIO0_BASE_ADDR;
	else
		base_addr = GPIO1_BASE_ADDR;
	if(GPIO_GROUP == 0)
		offset = GPIOA_OUT_OFFSET;
	else
		offset = GPIOB_OUT_OFFSET;
	target = base_addr + offset;
	ret = mmap_write(target,writevalue);
	if(ret != 0)
	{
		printf("0x%lx set gpio value 0x%x failed1\n",target,writevalue);
		return ret;
	}
	//printf("0x%lx set gpio value 0x%x successfully!\n",target,writevalue);
	return ret;

}

/*
 * direction : gpio7-0 direction
 * return : 0 : successfully
 * 	    !=0 failed
 */ 

int set_gpio_direction(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP,uint32_t direction)
{
	off_t base_addr,offset,target;
	int ret = 0;
	if(GPIO_MODULE == 0)
		base_addr = GPIO0_BASE_ADDR;
	else
		base_addr = GPIO1_BASE_ADDR;

	if(GPIO_GROUP == 0)
		offset = GPIOA_DIRECTION_OFFSET;
	else
		offset = GPIOB_DIRECTION_OFFSET;

	target = base_addr + offset;
	ret = mmap_write(target,direction);
	if(ret !=0)
	{
		printf("0x%lx set gpio direction 0x%x failed!\n",target,direction);
		return ret;
	}
	//printf("0x%lx set gpio direction 0x%x successfully!\n",target,direction);
	return ret;


}

/*************************
 * return：>=0 value
 *         <0  failed!
 **************************/

uint32_t get_gpio_value(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP)
{
	off_t base_addr,offset,target;
	uint32_t ret;
	if(GPIO_MODULE == 0)
		base_addr = GPIO0_BASE_ADDR;
	else
		base_addr = GPIO1_BASE_ADDR;

	if(GPIO_GROUP == 0)
		offset = GPIOA_IN_OFFSET;
	else
		offset = GPIOB_IN_OFFSET;
	target = base_addr + offset;
	ret = mmap_read(target);
	//printf("0x%lx read gpio value is 0x%x\n",target,ret);	
	return ret;

}

/* 
 * GPIO_MODULE : 0 / 1
 *
 * GPIO_GROUP : 
 * 		0:A
 * 		1:B
 *
 * 1 : output
 * 0 : input
 *
 * return : <0 failed
 * 	    >0 GPIO DIRECTION
 */
uint32_t get_gpio_direction(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP)
{
	off_t base_addr,offset,target;
	uint32_t direction = 0;

	if(GPIO_MODULE == 0)
		base_addr = GPIO0_BASE_ADDR;
	else
		base_addr = GPIO1_BASE_ADDR;

	if(GPIO_GROUP == 0)
		offset = GPIOA_DIRECTION_OFFSET;
	else
		offset = GPIOB_DIRECTION_OFFSET;
	target = base_addr + offset;
	direction = mmap_read(target);
	//printf("0x%lx get gpio direction is 0x%x\n",target,direction);
	return direction;

}

GPIO_NUM get_gpio_num(char *gpio)
{
	GPIO_NUM gpio_num;
	gpio_num.flag = 0;	
	if(strcmp(gpio,"GPIO0_A0") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 0;
	}
	else if(strcmp(gpio,"GPIO0_A1") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 1;
	}
	else if((strcmp(gpio,"GPIO0_A2") == 0) || (strcmp(gpio,"GPIO0") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 2;
	}
	else if((strcmp(gpio,"GPIO0_A3") == 0) || (strcmp(gpio,"GPIO1") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 3;
	}
	else if((strcmp(gpio,"GPIO0_A4") == 0) || (strcmp(gpio,"GPIO2") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 4;
	}
	else if((strcmp(gpio,"GPIO0_A5") == 0) || (strcmp(gpio,"GPIO3") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 5;
	}
	else if((strcmp(gpio,"GPIO0_A6") == 0) || (strcmp(gpio,"GPIO4") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 6;
	}
	else if((strcmp(gpio,"GPIO0_A7") == 0) || (strcmp(gpio,"GPIO5") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 7;
	}
	else if(strcmp(gpio,"GPIO0_B0") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 0;
	}
	else if(strcmp(gpio,"GPIO0_B1") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 1;
	}
	else if(strcmp(gpio,"GPIO0_B2") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 2;
	}
	else if(strcmp(gpio,"GPIO0_B3") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 3;
	}
	else if(strcmp(gpio,"GPIO0_B4") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 4;
	}
	else if(strcmp(gpio,"GPIO0_B5") == 0)
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 5;
	}
	else if((strcmp(gpio,"GPIO0_B6") == 0) || (strcmp(gpio,"GPIO6") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 6;
	}
	else if((strcmp(gpio,"GPIO0_B7") == 0) || (strcmp(gpio,"GPIO7") == 0))
	{
		gpio_num.GPIO_MODULE = 0;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 7;
	}
	else if(strcmp(gpio,"GPIO1_A0") == 0)
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 0;
	}
	else if(strcmp(gpio,"GPIO1_A1") == 0)
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 1;

	}
	else if(strcmp(gpio,"GPIO1_A2") == 0)
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 2;
	}
	else if((strcmp(gpio,"GPIO1_A3") == 0) || (strcmp(gpio,"GPIO16") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 3;
	}
	else if((strcmp(gpio,"GPIO1_A4") == 0) || (strcmp(gpio,"GPIO17") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 4;
	}
	else if((strcmp(gpio,"GPIO1_A5") == 0) || (strcmp(gpio,"GPIO18") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 5;
	}
	else if(strcmp(gpio,"GPIO1_A6") == 0)
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 6;
	}
	else if((strcmp(gpio,"GPIO1_A7") == 0) || (strcmp(gpio,"GPIO19") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 0;
		gpio_num.GPIO_PIN = 7;
	}
	else if((strcmp(gpio,"GPIO1_B0") == 0) || (strcmp(gpio,"GPIO8") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 0;
	}
	else if((strcmp(gpio,"GPIO1_B1") == 0) || (strcmp(gpio,"GPIO9") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 1;
	}
	else if((strcmp(gpio,"GPIO1_B2") == 0) || (strcmp(gpio,"GPIO10") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 2;
	}
	else if((strcmp(gpio,"GPIO1_B3") == 0) || (strcmp(gpio,"GPIO11") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 3;
	}
	else if((strcmp(gpio,"GPIO1_B4") == 0) || (strcmp(gpio,"GPIO12") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 4;
	}
	else if((strcmp(gpio,"GPIO1_B5") == 0) || (strcmp(gpio,"GPIO13") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 5;
	}
	else if((strcmp(gpio,"GPIO1_B6") == 0) || (strcmp(gpio,"GPIO14") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 6;
	}
	else if((strcmp(gpio,"GPIO1_B7") == 0) || (strcmp(gpio,"GPIO15") == 0))
	{
		gpio_num.GPIO_MODULE = 1;
		gpio_num.GPIO_GROUP = 1;
		gpio_num.GPIO_PIN = 7;
	}
	else
	{
		gpio_num.flag = 1;
		//printf("%s is error!\n",gpio);
		return gpio_num;
	}
	//printf("GPIO_MODULE %d\nGPIO_GROUP %d\nGPIO_PIN %d\n",gpio_num.GPIO_MODULE,gpio_num.GPIO_GROUP,gpio_num.GPIO_PIN);

	return gpio_num;
}

uint32_t cmd_read(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP,int GPIO_PIN)
{
	uint32_t old_direction = 0;
	//uint32_t in_direction = 0;
	uint32_t read_value = 0;
	int value = 0;
	int direction = 0;
	//int ret = 0;

	old_direction = get_gpio_direction(GPIO_MODULE,GPIO_GROUP);
	if(old_direction <= 0)
		return old_direction;
	direction = (old_direction >> GPIO_PIN) &0x01;
	/*
	   ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,in_direction);
	   if(ret !=0 )
	   {
	   printf("set gpio all input direction error!\n");
	   return ret;
	   }
	   */
	read_value = get_gpio_value(GPIO_MODULE,GPIO_GROUP);
	/*
	   ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,old_direction);
	   if(ret != 0)
	   {
	   printf("set gpio old direction error!\n");
	   return ret;
	   }
	   */
	if(read_value < 0)
		return read_value;
	value = (read_value >> GPIO_PIN) & 0X01;
	if(direction == 0)
	{
		printf("direction : input\n");
	}
	else
		printf("direction : output\n");
	printf("value: %d\n",value);
	return read_value;

}

int cmd_write(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP,int GPIO_PIN,unsigned char writevalue)
{
	//uint32_t old_direction = 0;
	//uint32_t out_direction = 0XFF;
	//uint32_t in_direction = 0X00;
	uint32_t old_value = 0;
	uint32_t new_value = 0;
	int ret = 0;
	/*
	   old_direction = get_gpio_direction(GPIO_MODULE,GPIO_GROUP);
	   ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,in_direction);
	   if(ret != 0)
	   {
	   printf("set gpio all input direction error!\n");
	   return ret;
	   }
	   */
	old_value = get_gpio_value(GPIO_MODULE,GPIO_GROUP);

	if(writevalue == 0)
	{
		new_value = CLEAR_BIT(old_value,GPIO_PIN);
	}
	else if(writevalue == 1)
	{
		new_value = SET_BIT(old_value,GPIO_PIN);
	}
	/*
	   ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,out_direction);
	   if(ret != 0)
	   {
	   printf("set gpio all output direction error!\n");
	   return ret;
	   }
	   */
	ret = set_gpio_value(GPIO_MODULE,GPIO_GROUP,new_value);
	if(ret != 0)
	{
		printf("set gpio new value 0x%x error!\n",new_value);
		return ret;
	}
	/*
	   ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,old_direction);
	   if(ret != 0)
	   {
	   printf("set gpio old direction error!\n");
	   return ret;
	   }*/

	printf("write value  %d successfully!\n",writevalue);
	return ret;


}

int cmd_set(unsigned char GPIO_MODULE,unsigned char GPIO_GROUP,int GPIO_PIN,unsigned char direction)
{
	uint32_t old_direction = 0;
	uint32_t new_direction = 0;
	int ret = 0;
	old_direction = get_gpio_direction(GPIO_MODULE,GPIO_GROUP);
	if(direction == 0)
	{
		new_direction = CLEAR_BIT(old_direction,GPIO_PIN);
	}
	else if(direction == 1)
	{
		new_direction = SET_BIT(old_direction,GPIO_PIN);
	}

	ret = set_gpio_direction(GPIO_MODULE,GPIO_GROUP,new_direction);
	if(ret == 0)
	{
		if(direction == 0)
			printf("set direction input successfully!\n");
		else
			printf("set direction output successfully!\n");
	}
	else
		printf("set direction input/output failed!\n");
	return ret;

}

int init_gpio_input(unsigned char pull)
{
	off_t target;
	uint32_t old_config = 0;
	uint32_t new_config = 0;
	uint32_t config = 0;
	int ret = 0;

	/* pinmux_offset_2 0x0208
	 * GPIO1_B1 [3:2]   [1:0]   00 01 input
	 * GPIO1_B0 [7:6]   [5:4]   00 01 input
	 */
	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_2;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;

	for(int i = 0;i < 8;i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	for(int i = 0; i < 8 ;i +=4)
	{
		new_config = SET_BIT(config,i);
		config = new_config;
	}
	if(pull == 1)
	{
		for(int i = 2;i < 8; i +=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	else if(pull == 2)
	{
		for(int i = 3; i < 8; i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	/* pinmux_config_offset_4 0x0214
	 * GPIO0_B7 [7:6]   [5:4]   00 10 input
	 * GPIO0_B6 [11:10] [9:8]   00 10 input
	 */
	target =  PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_4;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;

	for(int i = 4; i < 12; i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	for(int i = 5; i < 12; i+=4)
	{
		new_config = SET_BIT(config,i);
		config = new_config;
	}
	if(pull == 1)
	{
		for(int i = 6; i < 12; i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	else if(pull == 2)
	{
		for(int i = 7; i < 16; i+=4)
		{
			new_config = SET_BIT(config,1);
			config = new_config;
		}
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);
	return ret;

}

int init_gpio0_a_output(unsigned char pull)
{
	off_t target;
	uint32_t old_config = 0;
	uint32_t new_config = 0;
	uint32_t config = 0;
	int ret = 0;
	/* register_offset_0 0x0200
	 * GPIO0_A6 [3:2]   [1:0]   00 01
	 * GPIO0_A5 [7:6]   [5:4]   00 01
	 * GPIO0_A4 [11:10] [9:8]   00 01
	 * GPIO0_A3 [15:14] [13:12] 00 01
	 * GPIO0_A2 [19:18] [17:16] 00 01
	 */
	//printf("*****GPIO0_A2 - GPIO0_A6 configuration*******\n");
	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_0;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;
	for(int i  = 0;i < 20; i++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	for(int i =0;i < 20;i+=4)
	{
		new_config = SET_BIT(config,i);
		config = new_config;
	}
	if(pull == 1)
	{
		for(int i = 2;i < 20;i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	else if(pull == 2)
	{
		for(int i = 3;i < 20;i +=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}

	}

	ret = mmap_write(target,new_config);
	if(ret != 0 )
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	/*pinmux_config_offset_1 0x0204
	 * GPIO0_A7 [31:30] [29 :28] 00 01
	 */
	//printf("*****GPIO0_A7 configuration******\n");
	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_1;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;
	for(int i = 28; i < 32; i++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	new_config = SET_BIT(config,28);
	if(pull == 1)
	{
		config = new_config;
		new_config = SET_BIT(config,30);
	}
	else if(pull == 2)
	{
		config = new_config;
		new_config = SET_BIT(config,31);
	}
	ret = mmap_write(target,new_config);
	if(ret != 0 )
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	return ret;

}

int init_gpio1_a_output(unsigned char pull)
{
	off_t target;
	uint32_t old_config = 0;
	uint32_t new_config = 0;
	uint32_t config = 0;
	int ret = 0;

	/* pinmux_config_offset_5 0x0218
	 * GPIO1_A3 [11:10] [9:8]   00 01
	 * GPIO1_A4 [15:14] [13:12] 00 01
	 */
	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_5;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;
	for(int i = 8;i < 16; i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	for(int i = 8;i < 16; i+=4)
	{
		new_config = SET_BIT(config,i);
		config = new_config;
	}
	if(pull == 1)
	{
		for(int i = 10;i < 16;i+=4 )
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	else if(pull == 2)
	{
		for(int i = 11;i < 16; i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	/* pinmux_offset_2 0x0208
	 * GPIO1_A7 [11:10] [9:8]   00 01
	 *
	 * GPIO1_A5 [19:18] [17:16] 00 01
	 */
	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_2;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;

	for(int i = 8;i < 12;i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	new_config = SET_BIT(config,8);
	config = new_config;
	for(int i = 16; i < 20;i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}

	new_config = SET_BIT(config,16);
	if(pull == 1)
	{
		new_config = SET_BIT(config,10);
		config = new_config;
		new_config = SET_BIT(config,18);
	}
	else if(pull == 2)
	{
		new_config = SET_BIT(config,11);
		config = new_config;
		new_config = SET_BIT(config,19);
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);
	return ret;
}


int init_gpio1_b_output(unsigned char pull)
{
	off_t target;
	uint32_t old_config = 0;
	uint32_t new_config = 0;
	uint32_t config = 0;
	int ret = 0;

	/* pinmux_config_offset_3 0x020c
	 * GPIO1_B6 [15:14] [13:12] 00 01
	 * GPIO1_B5 [19:18] [17:16] 00 01
	 * GPIO1_B4 [23:22] [21:20] 00 01
	 * GPIO1_B3 [27:26] [25:24] 00 01
	 * GPIO1_B2 [31:30] [29:28] 00 01
	 */

	target = PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_3;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;
	for(int i = 12; i < 32; i++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	for(int i = 12;i < 32; i+=4)
	{
		new_config = SET_BIT(config,i);
		config = new_config;
	}
	if(pull == 1)
	{
		for(int i = 14; i < 32;i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	else if(pull == 2)
	{
		for(int i = 15;i < 32; i+=4)
		{
			new_config = SET_BIT(config,i);
			config = new_config;
		}
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	/* pinmux_config_offset_4 0x0214
	 * GPIO1_B7 [15:14] [13:12] 00 01
	 */
	target =  PINMUX_CONFIG_BASE_ADDR + PINMUX_CONFIG_OFFSET_4;
	old_config = mmap_read(target);
	//printf("0x%lx old config is 0x%x\n",target,old_config);
	config = old_config;

	for(int i = 12; i < 16; i ++)
	{
		new_config = CLEAR_BIT(config,i);
		config = new_config;
	}
	new_config = SET_BIT(config,12);
	config = new_config;
	if(pull == 1)
	{
		new_config = SET_BIT(config,14);
	}
	else if(pull == 2)
	{
		new_config = SET_BIT(config,15);
	}
	ret = mmap_write(target,new_config);
	if(ret != 0)
	{
		printf("mmap 0x%lx new config 0x%x failed!\n",target,new_config);
		return ret;
	}
	//printf("mmap 0x%lx new config 0x%x successfully!\n",target,new_config);

	return ret;

}


/*
 * pull:
 * 	0:no-pull 
 * 	1:pull_down
 * 	2:pull up
 */


int init_gpio_output(unsigned char pull)
{
	int ret = 0;

	ret = init_gpio0_a_output(pull);
	if(ret !=0)
	{
		printf("init_gpio0_a_output failed!\n");
		return ret;
	}
	ret = init_gpio1_a_output(pull);
	if(ret != 0)
	{
		printf("init_gpio1_a_output failed!\n");
		return ret;
	}
	ret = init_gpio1_b_output(pull);
	if(ret !=0)
	{
		printf("init gpio1_boutput failed!\n");
		return ret;
	}
	return ret;
}

/*按照硬件设计直接设置GPIO方向*/
int init_direction()
{
	//GPIO0_A7 - GPIO0_A2 : output 1111 11xx
	uint32_t old_direction = 0;
	uint32_t new_direction = 0;
	int ret = 0;
	old_direction = get_gpio_direction(0,0);
	new_direction = (old_direction | 0xFC);
	ret = set_gpio_direction(0,0,new_direction);
	if(ret!=0)
	{
		printf("init gpio0_a output direction failed!\n");
		return ret;
	}
	//GPIO0_B7-GPIO0_B6 :input 00xx xxxx
	old_direction = get_gpio_direction(0,1);
	new_direction = (old_direction & 0x3F);
	ret = set_gpio_direction(0,1,new_direction);
	if(ret != 0 )
	{
		printf("init gpio0_b input direction failed!\n");
		return ret;
	}
	//GPIO1_A7 A5 A4 A3 :output 1x11 1xxx
	old_direction = get_gpio_direction(1,0);
	new_direction = (old_direction | 0xB8);
	ret = set_gpio_direction(1,0,new_direction);
	if(ret != 0 )
	{
		printf("init gpio1_a output direction failed!\n");
		return ret;
	}
	//GPIO1_B7 - GPIO1_B2 : output 1111 11xx  
	//GPIO1_B1 - GPIO1_B0 : input  xxxx xx00
	//0xFC
	new_direction = 0xFC;
	ret = set_gpio_direction(1,1,new_direction);
	if(ret != 0 )
	{
		printf("init gpio1_b direction failed!\n");
	}
	return ret;	

}

int main(int argc, char **argv)
{

	GPIO_NUM gpio_num;
	unsigned char direction = 0;
	unsigned char cmd = 0;
	unsigned char writevalue;
	int ret = 0;
	if(argc > 1)	
	{
		if(strcmp(argv[1],"set") == 0)
		{
			if(argc < 4)
			{
				printf("please input %s set GPIO in/out !\n",argv[0]);
				return -1;
			}	
			if(strcmp(argv[3],"in") == 0)
			{
				direction = 0;
			}
			else if(strcmp(argv[3],"out") == 0)
			{
				direction = 1;
			}
			else
			{
				printf("%s is error,please input in or out!\n",argv[3]);
				return -4;
			}
			cmd = 0;
		}
		else if(strcmp(argv[1],"write") == 0)
		{
			if(argc < 4)
			{
				printf("please input %s write GPIO value !\n",argv[0]);
				return -2;
			}
			writevalue = (unsigned char)atoi(argv[3]);
			cmd = 1;
		}
		else if(strcmp(argv[1],"read") == 0)
		{
			if(argc < 3)
			{
				printf("please input %s read GPIO !\n",argv[0]);
				return -3;
			}
			cmd = 2;
		}
		else if(strcmp(argv[1],"init") == 0)
		{
			ret = init_gpio_input(0);
			if(ret!=0)
			{
				printf("init_gpio_input failed!\n");
				return ret;
			}
			ret = init_gpio_output(1);
			if(ret!=0)
			{
				printf("init_gpio_output failed!\n");
				return ret;
			}
			ret = init_direction();
			if(ret !=0 )
			{
				printf("init_direction failed!\n");
				return ret;
			}
			printf("init successfully!\n");
			return ret;
		}
		else
		{
			printf("input error\n");
			return -5;
		}
	}
	else
	{
		printf("input error!\n");
		return -5;
	}
	gpio_num = get_gpio_num(argv[2]);
	if(gpio_num.flag == 1)
	{
		printf("GPIO is error!\n");
		return -1;
	}
	switch(cmd){
	case 0:cmd_set(gpio_num.GPIO_MODULE,gpio_num.GPIO_GROUP,gpio_num.GPIO_PIN,direction);break;
	case 1:cmd_write(gpio_num.GPIO_MODULE,gpio_num.GPIO_GROUP,gpio_num.GPIO_PIN,writevalue);break;
	case 2:cmd_read(gpio_num.GPIO_MODULE,gpio_num.GPIO_GROUP,gpio_num.GPIO_PIN);
	default:break;
	}


	return 0;
}
