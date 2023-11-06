#ifndef __PHYTIUM_GPIO_H__
#define __PHYTIUM_GPIO_H__

#include <stdint.h>

#define GPIO_DIR_IN    0
#define GPIO_DIR_OUT   1

#define GPIO_CTRL_NUM  2  /* 0, 1 */
#define GPIO_PORT_NUM  2  /* a, b */
#define GPIO_PIN_NUM   8  /* 0 ~ 7 */

#define GPIO_NAME_LEN_MAX  8

/**
 *  gpioname: gpio description string
 *            it's format is "controller-port-pin"
 *            eg: 0-a-0, 1-b-7
 *  gpiodir : gpio direction
 *            0 input, 1 output
 */

int gpio_init(const char *gpioname, uint8_t gpiodir);
int gpio_set(const char *gpioname, uint8_t val);
int gpio_get(const char *gpioname, uint8_t *val);

#endif /* __PHYTIUM_GPIO_H__ */
