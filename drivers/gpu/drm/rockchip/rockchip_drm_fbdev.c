// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) Fuzhou Rockchip Electronics Co.Ltd
 * Author:Mark Yao <mark.yao@rock-chips.com>
 */

#include <drm/drm.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_client.h>

#include "rockchip_drm_drv.h"
#include "rockchip_drm_gem.h"
#include "rockchip_drm_fb.h"
#include "rockchip_drm_fbdev.h"

#define PREFERRED_BPP		32

static int rockchip_fbdev_mmap(struct fb_info *info,
			       struct vm_area_struct *vma)
{
	struct drm_fb_helper *helper = info->par;
	struct rockchip_drm_private *private = helper->dev->dev_private;

	return rockchip_gem_mmap_buf(private->fbdev_bo, vma);
}

__FB_GEN_DEFAULT_DEFERRED_OPS_DRAW(rockchip_drm_fbdev,
  drm_fb_helper_damage_area, cfb)

static const struct fb_ops rockchip_drm_fbdev_ops = {
	.owner		= THIS_MODULE,
	DRM_FB_HELPER_DEFAULT_OPS,
	.fb_mmap	= rockchip_fbdev_mmap,
  __FB_DEFAULT_DEFERRED_OPS_DRAW(rockchip_drm_fbdev),
};

static int rockchip_drm_fbdev_create(struct drm_fb_helper *helper,
				     struct drm_fb_helper_surface_size *sizes)
{
	struct rockchip_drm_private *private = helper->dev->dev_private;
	struct drm_mode_fb_cmd2 mode_cmd = { 0 };
	struct drm_device *dev = helper->dev;
	struct rockchip_gem_object *rk_obj;
	struct drm_framebuffer *fb;
	unsigned int bytes_per_pixel;
	unsigned long offset;
	struct fb_info *fbi;
	size_t size;
	int ret;

	bytes_per_pixel = DIV_ROUND_UP(sizes->surface_bpp, 8);

	mode_cmd.width = sizes->surface_width;
	mode_cmd.height = sizes->surface_height;
	mode_cmd.pitches[0] = sizes->surface_width * bytes_per_pixel;
	mode_cmd.pixel_format = drm_mode_legacy_fb_format(sizes->surface_bpp,
		sizes->surface_depth);

	size = mode_cmd.pitches[0] * mode_cmd.height;

	rk_obj = rockchip_gem_create_object(dev, size, true, 0);
	if (IS_ERR(rk_obj))
		return -ENOMEM;

	private->fbdev_bo = &rk_obj->base;

	fbi = drm_fb_helper_alloc_info(helper);
	if (IS_ERR(fbi)) {
		DRM_DEV_ERROR(dev->dev, "Failed to create framebuffer info.\n");
		ret = PTR_ERR(fbi);
		goto out;
	}

	helper->fb = rockchip_drm_framebuffer_init(dev, &mode_cmd,
						   private->fbdev_bo);
	if (IS_ERR(helper->fb)) {
		DRM_DEV_ERROR(dev->dev,
			      "Failed to allocate DRM framebuffer.\n");
		ret = PTR_ERR(helper->fb);
		goto out;
	}

	fbi->fbops = &rockchip_drm_fbdev_ops;

	fb = helper->fb;
	drm_fb_helper_fill_info(fbi, helper, sizes);

	offset = fbi->var.xoffset * bytes_per_pixel;
	offset += fbi->var.yoffset * fb->pitches[0];

	fbi->screen_base = rk_obj->kvaddr + offset;
	fbi->screen_size = rk_obj->base.size;
	fbi->fix.smem_len = rk_obj->base.size;

	DRM_DEBUG_KMS("FB [%dx%d]-%d kvaddr=%p offset=%ld size=%zu\n",
		      fb->width, fb->height, fb->format->depth,
		      rk_obj->kvaddr,
		      offset, size);

	return 0;

out:
	drm_gem_object_put(&rk_obj->base);
	return ret;
}

static int rockchip_drm_fbdev_dirty(struct drm_fb_helper *helper, struct drm_clip_rect *clip)
{
  if (!(clip->x1 < clip->x2 && clip->y1 < clip->y2))
    return 0;
  if (helper->fb->funcs->dirty)
    return helper->fb->funcs->dirty(helper->fb, NULL, 0, 0, clip, 1);
  return 0;
}

static const struct drm_fb_helper_funcs rockchip_drm_fb_helper_funcs = {
	.fb_probe = rockchip_drm_fbdev_create,
  .fb_dirty = rockchip_drm_fbdev_dirty,
};

static void rockchip_drm_fbdev_client_unregister(struct drm_client_dev *client)
{
  struct drm_fb_helper *fb_helper = drm_fb_helper_from_client(client);
  struct rockchip_drm_private *private = fb_helper->dev->dev_private;
  drm_fb_helper_unregister_info(fb_helper);
  drm_fb_helper_unprepare(fb_helper);
  drm_client_release(&fb_helper->client);
  kfree(fb_helper);
  private->fbdev_helper = NULL;
}

static int rockchip_drm_fbdev_client_restore(struct drm_client_dev *client)
{
  struct drm_fb_helper *fb_helper = drm_fb_helper_from_client(client);
  struct rockchip_drm_private *private = fb_helper->dev->dev_private;
  if (!private->fbdev_helper)
    return -EINVAL;
  if (!private->fbdev_bo)
    return -ENOMEM;
  return drm_fb_helper_restore_fbdev_mode_unlocked(private->fbdev_helper);
}

static int rockchip_drm_fbdev_client_hotplug(struct drm_client_dev *client)
{
  struct drm_fb_helper *fb_helper = drm_fb_helper_from_client(client);
  struct drm_device *dev = client->dev;
  int ret;
  if (dev->fb_helper) {
    rockchip_drm_output_poll_changed(dev);
    return 0;
  }
  ret = drm_fb_helper_init(dev, fb_helper);
  if (ret)
    goto err_drm_err;
  ret = drm_fb_helper_initial_config(fb_helper);
  if (ret)
    goto err_drm_fb_helper_fini;
  return 0;
err_drm_fb_helper_fini:
  drm_fb_helper_fini(fb_helper);
err_drm_err:
  drm_err(dev, "Failed to setup rockchip fbdev emulation (ret=%d)\n", ret);
  return ret;
}

static const struct drm_client_funcs rockchip_drm_fbdev_client_funcs = {
    .owner        = THIS_MODULE,
    .unregister    = rockchip_drm_fbdev_client_unregister,
    .restore    = rockchip_drm_fbdev_client_restore,
    .hotplug    = rockchip_drm_fbdev_client_hotplug,
};


int rockchip_drm_fbdev_init(struct drm_device *dev)
{
	struct rockchip_drm_private *private = dev->dev_private;
	struct drm_fb_helper *helper;
	int ret;

	if (!dev->mode_config.num_crtc || !dev->mode_config.num_connector)
		return -EINVAL;

	helper = devm_kzalloc(dev->dev, sizeof(*helper), GFP_KERNEL);
	if (!helper)
		return -ENOMEM;
	private->fbdev_helper = helper;

	drm_fb_helper_prepare(dev, helper, PREFERRED_BPP, &rockchip_drm_fb_helper_funcs);

	ret = drm_client_init(dev, &helper->client, "rockchip-fbdev",
												&rockchip_drm_fbdev_client_funcs);
  if (ret) {
    drm_err(dev, "Failed to register client: %d\n", ret);
    goto err_drm_fb_helper_unprepare;
  }
  drm_client_register(&helper->client);

	return 0;

err_drm_fb_helper_unprepare:
	drm_fb_helper_unprepare(helper);
  kfree(helper);
  private->fbdev_helper = NULL;
	return ret;
}
