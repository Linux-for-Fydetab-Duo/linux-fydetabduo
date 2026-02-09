# SPDX-License-Identifier: GPL-2.0-only
#
# Rockchip Soc drivers
#
obj-$(CONFIG_ROCKCHIP_GRF) += grf.o
obj-$(CONFIG_ROCKCHIP_IODOMAIN) += io-domain.o
obj-$(CONFIG_ROCKCHIP_DTPM) += dtpm.o
obj-$(CONFIG_ROCKCHIP_CPUINFO) += rockchip-cpuinfo.o
obj-$(CONFIG_ROCKCHIP_VENDOR_STORAGE) += rk_vendor_storage.o
ccflags-$(CONFIG_ROCKCHIP_MMC_VENDOR_STORAGE) += -I$(srctree)/drivers/mmc
mmc-vendor-storage-y += rk_sdmmc_ops.o sdmmc_vendor_storage.o
obj-$(CONFIG_ROCKCHIP_MMC_VENDOR_STORAGE) += mmc-vendor-storage.o
obj-$(CONFIG_ROCKCHIP_MTD_VENDOR_STORAGE) += mtd_vendor_storage.o
obj-$(CONFIG_ROCKCHIP_OPP) += rockchip_opp_select.o
obj-$(CONFIG_ROCKCHIP_PVTM) += rockchip_pvtm.o
obj-$(CONFIG_ROCKCHIP_DEBUG) += rockchip_debug.o
obj-$(CONFIG_ROCKCHIP_SUSPEND_MODE) += rockchip_pm_config.o
obj-$(CONFIG_ROCKCHIP_SYSTEM_MONITOR) += rockchip_system_monitor.o
obj-y += fiq_debugger/
