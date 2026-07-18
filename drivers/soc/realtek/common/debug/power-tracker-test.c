#pragma GCC diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#define pr_fmt(fmt) "power-tracker-test: " fmt
#include <linux/module.h>
#include <linux/device.h>
#include <linux/ktime.h>
#include <linux/cpufreq.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <soc/realtek/power-tracker.h>
#include <soc/realtek/rtk_sb2_dbg.h>
#include "power-tracker.h"

static struct power_tracker pt_test = {
	.id = "test",
};

/* cpufreq policy callback removed because CPUFREQ_ADJUST is deprecated */

static int inv_callback(struct notifier_block *nb, unsigned long action, void *data)
{
	struct power_tracker *pt = &pt_test;
	struct sb2_inv_event_data *d = data;
	ktime_t delta;

	if (d->addr != 0x98000090)
		return NOTIFY_DONE;

	delta = ktime_sub(d->cur_time, pt->start);
	pr_err("sb2 inv takes %3lld us\n", ktime_to_us(delta));

	return NOTIFY_OK;
}

static struct notifier_block inv_nb = {
	.notifier_call = inv_callback,
};

static void do_pt_test(void)
{
	void *base = ioremap(0x98000000, 0x1000);
	struct power_tracker *pt = &pt_test;
	u32 val;

	if (!base)
		return;

	power_tracker_start(pt, 10);
	val = readl(base + 0x90);
	udelay(100);
	power_tracker_stop(pt);
	iounmap(base);
}

static int __init pt_test_init(void)
{
	int cpu = cpumask_first(cpu_online_mask);
	int ret;
	int i;

	ret = power_tracker_register(&pt_test);
	if (ret) {
		pr_err("power_tracker_register() returns %d\n", ret);
		return ret;
	}

	ret = sb2_dbg_register_inv_notifier(&inv_nb);
	if (ret) {
		pr_err("sb2_dbg_register_inv_notifier() returns %d\n", ret);
		goto unregister_power_tracker;
	}

	/* cpufreq_register_notifier removed */
	sb2_dbg_remove_default_handlers();
	for (i = 0; i < 10; i++) {
		pr_err("test #%d\n", i);
		do_pt_test();
		msleep(500);
	}
	msleep(500);
	sb2_dbg_add_default_handlers();

	/* cpufreq_unregister_notifier removed */
unregister_sb2_sdb_int_notifier:
	sb2_dbg_unregister_inv_notifier(&inv_nb);
unregister_power_tracker:
	power_tracker_unregister(&pt_test);
	return ret;
}
late_initcall(pt_test_init);

