/*
 * Realtek SD/MMC/mini SD card driver Skeleton for Linux 7.1.3
 */

#ifndef DRIVERS_MMC_HOST_RTK_SDMMC_H_
#define DRIVERS_MMC_HOST_RTK_SDMMC_H_

#include <linux/mmc/host.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/spinlock.h>

struct rtk_sdmmc_host {
    struct mmc_host *mmc;
    struct mmc_request *mrq;
    
    volatile u32 rtflags;

    void __iomem *sdmmc;
    void __iomem *pll;
    void __iomem *sysbrdg;

    spinlock_t lock;

    struct tasklet_struct req_end_tasklet;
    struct timer_list timer;

    struct device *dev;
    int irq;

    dma_addr_t paddr;
    u32 power_status;
};

#endif
