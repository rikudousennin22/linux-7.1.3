#ifndef __LINUX_OF_GPIO_H
#define __LINUX_OF_GPIO_H
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
static inline int of_get_gpio_flags(struct device_node *np, int index, unsigned int *flags) {
    struct gpio_desc *desc = fwnode_gpiod_get_index(of_fwnode_handle(np), NULL, index, GPIOD_ASIS, NULL);
    if (IS_ERR(desc)) return PTR_ERR(desc);
    return desc_to_gpio(desc);
}
#endif
