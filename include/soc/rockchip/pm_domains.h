/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright 2022, The Chromium OS Authors. All rights reserved.
 */

#ifndef __SOC_ROCKCHIP_PM_DOMAINS_H__
#define __SOC_ROCKCHIP_PM_DOMAINS_H__

#ifdef CONFIG_ROCKCHIP_PM_DOMAINS

int rockchip_pmu_block(void);
int rockchip_pmu_idle_request(struct device *dev, bool idle);
void rockchip_pmu_unblock(void);
int rockchip_save_qos(struct device *dev);
int rockchip_restore_qos(struct device *dev);
int rockchip_pmu_pd_on(struct device *dev);
int rockchip_pmu_pd_off(struct device *dev);
bool rockchip_pmu_pd_is_on(struct device *dev);

#else /* CONFIG_ROCKCHIP_PM_DOMAINS */

static inline int rockchip_pmu_block(void)
{
	return 0;
}

static inline void rockchip_pmu_unblock(void) { }
static inline void int rockchip_save_qos(struct device *dev) {return 0;}
static inline void int rockchip_restore_qos(struct device *dev) {return 0;}

static inline int rockchip_pmu_pd_on(struct device *dev)
{
	return -ENOTSUPP;
}

static inline int rockchip_pmu_pd_off(struct device *dev)
{
	return -ENOTSUPP;
}

static inline bool rockchip_pmu_pd_is_on(struct device *dev)
{
	return true;
}

#endif /* CONFIG_ROCKCHIP_PM_DOMAINS */

#endif /* __SOC_ROCKCHIP_PM_DOMAINS_H__ */
