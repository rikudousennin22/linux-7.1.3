import re

with open('drivers/mmc/host/rtk-sdmmc.c', 'r') as f:
    content = f.read()

# Add include
content = content.replace('#include <linux/pm_runtime.h>', '#include <linux/pm_runtime.h>\n#include <linux/gpio/consumer.h>')

# Probe GPIO requests
probe_old = '''	rtk_host->sdmmc_gpio = of_get_gpio_flags(sdmmc_node, 0, NULL);
	if (gpio_is_valid(rtk_host->sdmmc_gpio)) {
		ret = gpio_request(rtk_host->sdmmc_gpio, " sdmmc_gpio)
