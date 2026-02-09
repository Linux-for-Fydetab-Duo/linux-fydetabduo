/* Copyright (C) MicroArray
 * MicroArray Fprint Driver Code
 * mtk-settings.h
 * Date: 2017-3-15
 * Version: v4.0.06
 * Author: guq
 * Contact: guq@microarray.com.cn
 */

#ifndef __MAFP_SETTINGS_H_
#define __MAFP_SETTINGS_H_

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

#include "madev.h"

#define MA_DRV_NAME "madev"

#define MA_DTS_NAME "mediatek,microarray_finger"

#define MA_EINT_DTS_NAME "mediatek,microarray_finger"

#define MA_INT_PIN_LABEL "finger_int_pin"
// macro settings end

// call madev function
extern int mas_plat_probe(struct platform_device *pdev);
extern void mas_plat_remove(struct platform_device *pdev);

extern int mas_probe(struct spi_device *spi);
extern void mas_remove(struct spi_device *spi);

struct mt_spi_t {
  struct platform_device *pdev;
  void __iomem *regs;
  int irq;
  int running;
  struct mt_chip_conf *config;
  struct spi_master *master;

  struct spi_transfer *cur_transfer;
  struct spi_transfer *next_transfer;

  spinlock_t lock;
  struct list_head queue;
  struct clk *clk_main;
};
void mt_spi_enable_clk(struct mt_spi_t *ms);
void mt_spi_disable_clk(struct mt_spi_t *ms);
void mt_spi_enable_master_clk(struct spi_device *spidev);
void mt_spi_disable_master_clk(struct spi_device *spidev);

/* add for spi cls ctl end this func only used in tee enviroment*/

// the interface called by madev
void mas_select_transfer(struct spi_device *spi, int len);
int mas_finger_get_gpio_info(struct platform_device *pdev);
int mas_finger_set_gpio_info(int cmd);
void mas_enable_spi_clock(struct spi_device *spi);
void mas_disable_spi_clock(struct spi_device *spi);

unsigned int mas_get_irq(struct device *dev);
int mas_get_platform(void);
int mas_remove_platform(void);
int mas_power(int cmd);
int get_screen(void);
void ma_spi_change(struct spi_device *spi, unsigned int speed, int flag);
int mas_get_interrupt_gpio(unsigned int index);
int mas_switch_power(unsigned int on_off);
int mas_do_some_for_probe(struct spi_device *spi);
void mas_finger_set_reset(int count);
void mas_free_dts_info(void);
int mas_tee_spi_transfer(u8 *txb, u8 *rxb, int len);
#endif  //__MAFP_SETTINGS_H_
