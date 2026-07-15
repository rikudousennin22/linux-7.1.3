/*
 * Realtek SD/MMC/mini SD card driver Skeleton for Linux 7.1.3
 *
 * Authors:
 * Copyright (C) 2017 Realtek Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/blkdev.h>
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/irq.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/of.h>
#include <linux/pm_runtime.h>

#include "rtk-sdmmc-reg.h"
#include "rtk-sdmmc.h"

#define DRIVER_NAME "rtk-sdmmc"

static void rtk_sdmmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	/* Skeleton: Complete the request immediately with an error for now */
	mrq->cmd->error = -ENOSYS;
	mmc_request_done(mmc, mrq);
}

static void rtk_sdmmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	/* Skeleton: Set clock, bus width, power here */
}

static int rtk_sdmmc_get_ro(struct mmc_host *mmc)
{
	/* Skeleton: Return read-only status (0 means read/write) */
	return 0;
}

static int rtk_sdmmc_get_cd(struct mmc_host *mmc)
{
	/* Skeleton: Return card-detect status (1 means present) */
	return 1;
}

static const struct mmc_host_ops rtk_sdmmc_ops = {
	.request = rtk_sdmmc_request,
	.set_ios = rtk_sdmmc_set_ios,
	.get_ro = rtk_sdmmc_get_ro,
	.get_cd = rtk_sdmmc_get_cd,
};

static irqreturn_t rtk_sdmmc_irq(int irq, void *dev_id)
{
	struct rtk_sdmmc_host *rtk_host = dev_id;

	/* Skeleton: Handle interrupts */
	
	return IRQ_HANDLED;
}

static int rtk_sdmmc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mmc_host *mmc;
	struct rtk_sdmmc_host *rtk_host;
	int ret;

	dev_info(dev, "Realtek SD/MMC Host Driver Probe (Skeleton)\n");

	mmc = mmc_alloc_host(sizeof(struct rtk_sdmmc_host), dev);
	if (!mmc)
		return -ENOMEM;

	rtk_host = mmc_priv(mmc);
	rtk_host->mmc = mmc;
	rtk_host->dev = dev;

	spin_lock_init(&rtk_host->lock);

	/* Set up mmc capabilities */
	mmc->ops = &rtk_sdmmc_ops;
	mmc->f_min = 400000;
	mmc->f_max = 50000000;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED;

	/* Setup IRQ (dummy) */
	rtk_host->irq = platform_get_irq(pdev, 0);
	if (rtk_host->irq >= 0) {
		ret = devm_request_irq(dev, rtk_host->irq, rtk_sdmmc_irq, 0,
				       dev_name(dev), rtk_host);
		if (ret)
			goto free_host;
	}

	platform_set_drvdata(pdev, mmc);

	ret = mmc_add_host(mmc);
	if (ret)
		goto free_host;

	return 0;

free_host:
	mmc_free_host(mmc);
	return ret;
}

static void rtk_sdmmc_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	if (mmc) {
		mmc_remove_host(mmc);
		mmc_free_host(mmc);
	}
}

#ifdef CONFIG_OF
static const struct of_device_id rtk_sdmmc_match[] = {
	{ .compatible = "realtek,rtd1619-sdmmc", },
	{ .compatible = "realtek,rtk-sdmmc", },
	{},
};
MODULE_DEVICE_TABLE(of, rtk_sdmmc_match);
#endif

static struct platform_driver rtk_sdmmc_driver = {
	.probe = rtk_sdmmc_probe,
	.remove = rtk_sdmmc_remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = of_match_ptr(rtk_sdmmc_match),
	},
};

module_platform_driver(rtk_sdmmc_driver);

MODULE_DESCRIPTION("Realtek SD/MMC Host Driver Skeleton");
MODULE_LICENSE("GPL v2");
