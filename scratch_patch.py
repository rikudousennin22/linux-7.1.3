import re

kconfig_file = '/home/willy/linux-7.1.3/drivers/regulator/Kconfig'
with open(kconfig_file, 'r') as f:
    content = f.read()

config_str = '''
config REGULATOR_G22XX_CORE
	tristate " GMT G22XX regulator core
