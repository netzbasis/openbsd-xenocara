/*
 * Copyright (C) 2014 Etnaviv Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Christian Gmeiner <christian.gmeiner@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <xf86drm.h>
#include <xf86atomic.h>

#include "etnaviv_priv.h"
#include "etnaviv_drmif.h"

static pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;

struct etna_device *etna_device_new(int fd)
{
	struct etna_device *dev = calloc(sizeof(*dev), 1);

	if (!dev)
		return NULL;

	atomic_set(&dev->refcnt, 1);
	dev->fd = fd;
	dev->handle_table = drmHashCreate();
	dev->name_table = drmHashCreate();
	etna_bo_cache_init(&dev->bo_cache);

	return dev;
}

struct etna_device *etna_device_ref(struct etna_device *dev)
{
	atomic_inc(&dev->refcnt);

	return dev;
}

static void etna_device_del_impl(struct etna_device *dev)
{
	etna_bo_cache_cleanup(&dev->bo_cache, 0);
	drmHashDestroy(dev->handle_table);
	drmHashDestroy(dev->name_table);

	free(dev);
}

drm_private void etna_device_del_locked(struct etna_device *dev)
{
	if (!atomic_dec_and_test(&dev->refcnt))
		return;

	etna_device_del_impl(dev);
}

void etna_device_del(struct etna_device *dev)
{
	if (!atomic_dec_and_test(&dev->refcnt))
		return;

	pthread_mutex_lock(&table_lock);
	etna_device_del_impl(dev);
	pthread_mutex_unlock(&table_lock);
}
