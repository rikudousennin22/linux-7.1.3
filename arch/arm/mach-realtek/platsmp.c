#include <linux/init.h>
#include <linux/smp.h>
#include <linux/of.h>
#include <linux/io.h>
#include <asm/smp_plat.h>
#include <asm/barrier.h>

extern void secondary_startup(void);

static int rtd16xx_smp_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	struct device_node *dn;
	u64 release_addr;
	void __iomem *spin_table;

	dn = of_get_cpu_node(cpu, NULL);
	if (!dn)
		return -ENODEV;

	if (of_property_read_u64(dn, "cpu-release-addr", &release_addr)) {
		pr_err("CPU%u: missing cpu-release-addr\n", cpu);
		of_node_put(dn);
		return -ENOENT;
	}
	of_node_put(dn);

	spin_table = ioremap(release_addr, 8);
	if (!spin_table)
		return -ENOMEM;

	/* Write the 32-bit entry point to the lower 32-bits */
	writel_relaxed(__pa_symbol(secondary_startup), spin_table);
	/* Write 0 to the upper 32-bits to ensure it's a valid 64-bit address for U-Boot AArch64 */
	writel_relaxed(0, spin_table + 4);

	/* Ensure the write is visible to other CPUs */
	dsb(sy);
	sev();
	
	iounmap(spin_table);

	return 0;
}

static const struct smp_operations rtd16xx_smp_ops __initconst = {
	.smp_boot_secondary	= rtd16xx_smp_boot_secondary,
};

CPU_METHOD_OF_DECLARE(rtd16xx_spin_table, "spin-table", &rtd16xx_smp_ops);
