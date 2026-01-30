/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright 2025 Google LLC
 */

#ifndef __LINUX_REVOCABLE_H
#define __LINUX_REVOCABLE_H

#include <linux/cleanup.h>

struct device;
struct revocable;
struct revocable_provider;

struct revocable_provider *revocable_provider_alloc(void *res);
void revocable_provider_free(struct revocable_provider *rp);
struct revocable_provider *devm_revocable_provider_alloc(struct device *dev,
							 void *res);

struct revocable *revocable_alloc(struct revocable_provider *rp);
void revocable_free(struct revocable *rev);
void *revocable_try_access(struct revocable *rev) __acquires(&rev->rp->srcu);
void revocable_release(struct revocable *rev) __releases(&rev->rp->srcu);

DEFINE_FREE(revocable, struct revocable *, if (_T) revocable_release(_T))

#define _REVOCABLE(_rev, _label, _res)						\
	for (struct revocable *__UNIQUE_ID(name) __free(revocable) = _rev;	\
	     (_res = revocable_try_access(_rev)) || true; ({ goto _label; }))	\
		if (0) {							\
_label:										\
			break;							\
		} else

#define REVOCABLE(_rev, _res) _REVOCABLE(_rev, __UNIQUE_ID(label), _res)

#endif /* __LINUX_REVOCABLE_H */
