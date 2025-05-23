/**
 * \file pcm/pcm_dshare.c
 * \ingroup PCM_Plugins
 * \brief PCM Direct Sharing of Channels Plugin Interface
 * \author Jaroslav Kysela <perex@perex.cz>
 * \date 2003
 */
/*
 *  PCM - Direct Sharing of Channels
 *  Copyright (c) 2003 by Jaroslav Kysela <perex@perex.cz>
 *
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 2.1 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
  
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <grp.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
//#include <sys/shm.h>
#include "../../include/shm.h"
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include "pcm_direct.h"

extern int shmctl(int shmid, int cmd, struct shmid_ds* buf);
extern int shmget(key_t key, size_t size, int shmflg);
extern void *shmat(int shmid, void const* shmaddr, int shmflg);
extern int shmdt(void const* shmaddr);

#ifndef PIC
/* entry for static linking */
const char *_snd_module_pcm_dshare = "";
#endif

#ifndef DOC_HIDDEN
/* start is pending - this state happens when rate plugin does a delayed commit */
#define STATE_RUN_PENDING	1024
#endif

static void do_silence(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	const snd_pcm_channel_area_t *dst_areas;
	unsigned int chn, dchn, channels;
	snd_pcm_format_t format;

	dst_areas = snd_pcm_mmap_areas(dshare->spcm);
	channels = dshare->channels;
	format = dshare->shmptr->s.format;
	for (chn = 0; chn < channels; chn++) {
		dchn = dshare->bindings ? dshare->bindings[chn] : chn;
		if (dchn != UINT_MAX)
			snd_pcm_area_silence(&dst_areas[dchn], 0,
					     dshare->shmptr->s.buffer_size, format);
	}
}

static void share_areas(snd_pcm_direct_t *dshare,
		      const snd_pcm_channel_area_t *src_areas,
		      const snd_pcm_channel_area_t *dst_areas,
		      snd_pcm_uframes_t src_ofs,
		      snd_pcm_uframes_t dst_ofs,
		      snd_pcm_uframes_t size)
{
	unsigned int chn, dchn, channels;
	snd_pcm_format_t format;

	channels = dshare->channels;
	format = dshare->shmptr->s.format;
	if (dshare->interleaved) {
		unsigned int fbytes = snd_pcm_format_physical_width(format) / 8;
		memcpy(((char *)dst_areas[0].addr) + (dst_ofs * channels * fbytes),
		       ((char *)src_areas[0].addr) + (src_ofs * channels * fbytes),
		       size * channels * fbytes);
	} else {
		for (chn = 0; chn < channels; chn++) {
			dchn = dshare->bindings ? dshare->bindings[chn] : chn;
			if (dchn != UINT_MAX)
				snd_pcm_area_copy(&dst_areas[dchn], dst_ofs,
						  &src_areas[chn], src_ofs, size, format);

		}
	}
}

/*
 *  synchronize shm ring buffer with hardware
 */
static void snd_pcm_dshare_sync_area(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_uframes_t slave_hw_ptr, slave_appl_ptr, slave_size;
	snd_pcm_uframes_t appl_ptr, size;
	const snd_pcm_channel_area_t *src_areas, *dst_areas;
	
	/* calculate the size to transfer */
	size = pcm_frame_diff(dshare->appl_ptr, dshare->last_appl_ptr, pcm->boundary);
	if (! size)
		return;
	slave_hw_ptr = dshare->slave_hw_ptr;
	/* don't write on the last active period - this area may be cleared
	 * by the driver during write operation...
	 */
	slave_hw_ptr -= slave_hw_ptr % dshare->slave_period_size;
	slave_hw_ptr += dshare->slave_buffer_size;
	if (slave_hw_ptr >= dshare->slave_boundary)
		slave_hw_ptr -= dshare->slave_boundary;
	slave_size = pcm_frame_diff(slave_hw_ptr, dshare->slave_appl_ptr, dshare->slave_boundary);
	if (slave_size < size)
		size = slave_size;
	if (! size)
		return;

	/* add sample areas here */
	src_areas = snd_pcm_mmap_areas(pcm);
	dst_areas = snd_pcm_mmap_areas(dshare->spcm);
	appl_ptr = dshare->last_appl_ptr % pcm->buffer_size;
	dshare->last_appl_ptr += size;
	dshare->last_appl_ptr %= pcm->boundary;
	slave_appl_ptr = dshare->slave_appl_ptr % dshare->slave_buffer_size;
	dshare->slave_appl_ptr += size;
	dshare->slave_appl_ptr %= dshare->slave_boundary;
	for (;;) {
		snd_pcm_uframes_t transfer = size;
		if (appl_ptr + transfer > pcm->buffer_size)
			transfer = pcm->buffer_size - appl_ptr;
		if (slave_appl_ptr + transfer > dshare->slave_buffer_size)
			transfer = dshare->slave_buffer_size - slave_appl_ptr;
		share_areas(dshare, src_areas, dst_areas, appl_ptr, slave_appl_ptr, transfer);
		size -= transfer;
		if (! size)
			break;
		slave_appl_ptr += transfer;
		slave_appl_ptr %= dshare->slave_buffer_size;
		appl_ptr += transfer;
		appl_ptr %= pcm->buffer_size;
	}
}

/*
 *  synchronize hardware pointer (hw_ptr) with ours
 */
static int snd_pcm_dshare_sync_ptr0(snd_pcm_t *pcm, snd_pcm_uframes_t slave_hw_ptr)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_uframes_t old_slave_hw_ptr, avail;
	snd_pcm_sframes_t diff;

	old_slave_hw_ptr = dshare->slave_hw_ptr;
	dshare->slave_hw_ptr = slave_hw_ptr;
	diff = pcm_frame_diff(slave_hw_ptr, old_slave_hw_ptr, dshare->slave_boundary);
	if (diff == 0)		/* fast path */
		return 0;
	if (dshare->state != SND_PCM_STATE_RUNNING &&
	    dshare->state != SND_PCM_STATE_DRAINING)
		/* not really started yet - don't update hw_ptr */
		return 0;
	dshare->hw_ptr += diff;
	dshare->hw_ptr %= pcm->boundary;
	// printf("sync ptr diff = %li\n", diff);
	if (pcm->stop_threshold >= pcm->boundary)	/* don't care */
		return 0;
	avail = snd_pcm_mmap_playback_avail(pcm);
	if (avail > dshare->avail_max)
		dshare->avail_max = avail;
	if (avail >= pcm->stop_threshold) {
		snd_timer_stop(dshare->timer);
		do_silence(pcm);
		gettimestamp(&dshare->trigger_tstamp, pcm->tstamp_type);
		if (dshare->state == SND_PCM_STATE_RUNNING) {
			dshare->state = SND_PCM_STATE_XRUN;
			return -EPIPE;
		}
		dshare->state = SND_PCM_STATE_SETUP;
		/* clear queue to remove pending poll events */
		snd_pcm_direct_clear_timer_queue(dshare);
	}
	return 0;
}

static int snd_pcm_dshare_sync_ptr(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_uframes_t slave_hw_ptr;
	int err;

	if (dshare->slowptr)
		snd_pcm_hwsync(dshare->spcm);
	slave_hw_ptr = *dshare->spcm->hw.ptr;
	err = snd_pcm_direct_check_xrun(dshare, pcm);
	if (err < 0)
		return err;

	return snd_pcm_dshare_sync_ptr0(pcm, slave_hw_ptr);
}

/*
 *  plugin implementation
 */

static snd_pcm_state_t snd_pcm_dshare_state(snd_pcm_t *pcm);

static int snd_pcm_dshare_status(snd_pcm_t *pcm, snd_pcm_status_t * status)
{
	snd_pcm_direct_t *dshare = pcm->private_data;

	memset(status, 0, sizeof(*status));
	snd_pcm_status(dshare->spcm, status);

	switch (dshare->state) {
	case SNDRV_PCM_STATE_DRAINING:
	case SNDRV_PCM_STATE_RUNNING:
		snd_pcm_dshare_sync_ptr0(pcm, status->hw_ptr);
		status->delay = snd_pcm_mmap_playback_delay(pcm);
		break;
	default:
		break;
	}
	status->state = snd_pcm_dshare_state(pcm);
	status->hw_ptr = *pcm->hw.ptr; /* boundary may be different */
	status->appl_ptr = *pcm->appl.ptr; /* slave PCM doesn't set appl_ptr */
	status->trigger_tstamp = dshare->trigger_tstamp;
	status->avail = snd_pcm_mmap_playback_avail(pcm);
	status->avail_max = status->avail > dshare->avail_max ? status->avail : dshare->avail_max;
	dshare->avail_max = 0;
	return 0;
}

static snd_pcm_state_t snd_pcm_dshare_state(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;

	snd_pcm_direct_check_xrun(dshare, pcm);
	if (dshare->state == STATE_RUN_PENDING)
		return SNDRV_PCM_STATE_RUNNING;
	return dshare->state;
}

static int snd_pcm_dshare_delay(snd_pcm_t *pcm, snd_pcm_sframes_t *delayp)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	int err;
	
	switch (dshare->state) {
	case SNDRV_PCM_STATE_DRAINING:
	case SNDRV_PCM_STATE_RUNNING:
		err = snd_pcm_dshare_sync_ptr(pcm);
		if (err < 0)
			return err;
		/* fallthru */
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_SUSPENDED:
	case STATE_RUN_PENDING:
		*delayp = snd_pcm_mmap_playback_delay(pcm);
		return 0;
	case SNDRV_PCM_STATE_XRUN:
		return -EPIPE;
	case SNDRV_PCM_STATE_DISCONNECTED:
		return -ENODEV;
	default:
		return -EBADFD;
	}
}

static int snd_pcm_dshare_hwsync(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;

	switch(dshare->state) {
	case SNDRV_PCM_STATE_DRAINING:
	case SNDRV_PCM_STATE_RUNNING:
		return snd_pcm_dshare_sync_ptr(pcm);
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_SUSPENDED:
		return 0;
	case SNDRV_PCM_STATE_XRUN:
		return -EPIPE;
	case SNDRV_PCM_STATE_DISCONNECTED:
		return -ENODEV;
	default:
		return -EBADFD;
	}
}

static int snd_pcm_dshare_reset(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	dshare->hw_ptr %= pcm->period_size;
	dshare->appl_ptr = dshare->last_appl_ptr = dshare->hw_ptr;
	snd_pcm_direct_reset_slave_ptr(pcm, dshare, *dshare->spcm->hw.ptr);
	return 0;
}

static int snd_pcm_dshare_start_timer(snd_pcm_t *pcm, snd_pcm_direct_t *dshare)
{
	int err;

	snd_pcm_hwsync(dshare->spcm);
	snd_pcm_direct_reset_slave_ptr(pcm, dshare, *dshare->spcm->hw.ptr);
	err = snd_timer_start(dshare->timer);
	if (err < 0)
		return err;
	dshare->state = SND_PCM_STATE_RUNNING;
	return 0;
}

static int snd_pcm_dshare_start(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_sframes_t avail;
	int err;
	
	if (dshare->state != SND_PCM_STATE_PREPARED)
		return -EBADFD;
	avail = snd_pcm_mmap_playback_hw_avail(pcm);
	if (avail == 0)
		dshare->state = STATE_RUN_PENDING;
	else if (avail < 0)
		return 0;
	else {
		err = snd_pcm_dshare_start_timer(pcm, dshare);
		if (err < 0)
			return err;
		snd_pcm_dshare_sync_area(pcm);
	}
	gettimestamp(&dshare->trigger_tstamp, pcm->tstamp_type);
	return 0;
}

static int snd_pcm_dshare_drop(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	if (dshare->state == SND_PCM_STATE_OPEN)
		return -EBADFD;
	dshare->state = SND_PCM_STATE_SETUP;
	snd_pcm_direct_timer_stop(dshare);
	do_silence(pcm);
	return 0;
}

/* locked version */
static int __snd_pcm_dshare_drain(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_uframes_t stop_threshold;
	int err;

	switch (snd_pcm_state(dshare->spcm)) {
	case SND_PCM_STATE_SUSPENDED:
		return -ESTRPIPE;
	default:
		break;
	}

	if (dshare->state == SND_PCM_STATE_OPEN)
		return -EBADFD;
	if (pcm->mode & SND_PCM_NONBLOCK)
		return -EAGAIN;
	if (dshare->state == SND_PCM_STATE_PREPARED) {
		if (snd_pcm_mmap_playback_hw_avail(pcm) > 0)
			snd_pcm_dshare_start(pcm);
		else {
			snd_pcm_dshare_drop(pcm);
			return 0;
		}
	}

	if (dshare->state == SND_PCM_STATE_XRUN) {
		snd_pcm_dshare_drop(pcm);
		return 0;
	}

	stop_threshold = pcm->stop_threshold;
	if (pcm->stop_threshold > pcm->buffer_size)
		pcm->stop_threshold = pcm->buffer_size;
	dshare->state = SND_PCM_STATE_DRAINING;
	do {
		err = snd_pcm_dshare_sync_ptr(pcm);
		if (err < 0) {
			snd_pcm_dshare_drop(pcm);
			break;
		}
		if (dshare->state == SND_PCM_STATE_DRAINING) {
			snd_pcm_dshare_sync_area(pcm);
			snd_pcm_wait_nocheck(pcm, SND_PCM_WAIT_DRAIN);
			snd_pcm_direct_clear_timer_queue(dshare); /* force poll to wait */

			switch (snd_pcm_state(dshare->spcm)) {
			case SND_PCM_STATE_SUSPENDED:
				return -ESTRPIPE;
			default:
				break;
			}
		}
	} while (dshare->state == SND_PCM_STATE_DRAINING);
	pcm->stop_threshold = stop_threshold;
	return 0;
}

static int snd_pcm_dshare_drain(snd_pcm_t *pcm)
{
	int err;

	snd_pcm_lock(pcm);
	err = __snd_pcm_dshare_drain(pcm);
	snd_pcm_unlock(pcm);
	return err;
}

static int snd_pcm_dshare_pause(snd_pcm_t *pcm ATTRIBUTE_UNUSED, int enable ATTRIBUTE_UNUSED)
{
	return -EIO;
}

static snd_pcm_sframes_t snd_pcm_dshare_rewindable(snd_pcm_t *pcm)
{
	return snd_pcm_mmap_playback_hw_rewindable(pcm);
}

static snd_pcm_sframes_t snd_pcm_dshare_rewind(snd_pcm_t *pcm, snd_pcm_uframes_t frames)
{
	snd_pcm_sframes_t avail;

	avail = snd_pcm_dshare_rewindable(pcm);
	if (frames > (snd_pcm_uframes_t)avail)
		frames = avail;
	snd_pcm_mmap_appl_backward(pcm, frames);
	return frames;
}

static snd_pcm_sframes_t snd_pcm_dshare_forwardable(snd_pcm_t *pcm)
{
	return snd_pcm_mmap_playback_avail(pcm);
}

static snd_pcm_sframes_t snd_pcm_dshare_forward(snd_pcm_t *pcm, snd_pcm_uframes_t frames)
{
	snd_pcm_sframes_t avail;

	avail = snd_pcm_dshare_forwardable(pcm);
	if (frames > (snd_pcm_uframes_t)avail)
		frames = avail;
	snd_pcm_mmap_appl_forward(pcm, frames);
	return frames;
}

static snd_pcm_sframes_t snd_pcm_dshare_readi(snd_pcm_t *pcm ATTRIBUTE_UNUSED, void *buffer ATTRIBUTE_UNUSED, snd_pcm_uframes_t size ATTRIBUTE_UNUSED)
{
	return -ENODEV;
}

static snd_pcm_sframes_t snd_pcm_dshare_readn(snd_pcm_t *pcm ATTRIBUTE_UNUSED, void **bufs ATTRIBUTE_UNUSED, snd_pcm_uframes_t size ATTRIBUTE_UNUSED)
{
	return -ENODEV;
}

static int snd_pcm_dshare_close(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;

	if (dshare->timer)
		snd_timer_close(dshare->timer);
	if (dshare->bindings)
		do_silence(pcm);
	snd_pcm_direct_semaphore_down(dshare, DIRECT_IPC_SEM_CLIENT);
	dshare->shmptr->u.dshare.chn_mask &= ~dshare->u.dshare.chn_mask;
	snd_pcm_close(dshare->spcm);
 	if (dshare->server)
 		snd_pcm_direct_server_discard(dshare);
 	if (dshare->client)
 		snd_pcm_direct_client_discard(dshare);
	if (snd_pcm_direct_shm_discard(dshare)) {
		if (snd_pcm_direct_semaphore_discard(dshare))
			snd_pcm_direct_semaphore_final(dshare, DIRECT_IPC_SEM_CLIENT);
	} else
		snd_pcm_direct_semaphore_final(dshare, DIRECT_IPC_SEM_CLIENT);
	free(dshare->bindings);
	pcm->private_data = NULL;
	free(dshare);
	return 0;
}

static snd_pcm_sframes_t snd_pcm_dshare_mmap_commit(snd_pcm_t *pcm,
						  snd_pcm_uframes_t offset ATTRIBUTE_UNUSED,
						  snd_pcm_uframes_t size)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	int err;

	err = snd_pcm_direct_check_xrun(dshare, pcm);
	if (err < 0)
		return err;
	if (! size)
		return 0;
	snd_pcm_mmap_appl_forward(pcm, size);
	if (dshare->state == STATE_RUN_PENDING) {
		err = snd_pcm_dshare_start_timer(pcm, dshare);
		if (err < 0)
			return err;
	} else if (dshare->state == SND_PCM_STATE_RUNNING ||
		   dshare->state == SND_PCM_STATE_DRAINING) {
		if ((err = snd_pcm_dshare_sync_ptr(pcm)) < 0)
			return err;
	}
	if (dshare->state == SND_PCM_STATE_RUNNING ||
	    dshare->state == SND_PCM_STATE_DRAINING) {
		/* ok, we commit the changes after the validation of area */
		/* it's intended, although the result might be crappy */
		snd_pcm_dshare_sync_area(pcm);
		/* clear timer queue to avoid a bogus return from poll */
		if (snd_pcm_mmap_playback_avail(pcm) < pcm->avail_min)
			snd_pcm_direct_clear_timer_queue(dshare);
	}
	return size;
}

static snd_pcm_sframes_t snd_pcm_dshare_avail_update(snd_pcm_t *pcm)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	int err;
	
	if (dshare->state == SND_PCM_STATE_RUNNING ||
	    dshare->state == SND_PCM_STATE_DRAINING) {
		if ((err = snd_pcm_dshare_sync_ptr(pcm)) < 0)
			return err;
	}
	if (dshare->state == SND_PCM_STATE_XRUN)
		return -EPIPE;

	return snd_pcm_mmap_playback_avail(pcm);
}

static int snd_pcm_dshare_htimestamp(snd_pcm_t *pcm,
				     snd_pcm_uframes_t *avail,
				     snd_htimestamp_t *tstamp)
{
	snd_pcm_direct_t *dshare = pcm->private_data;
	snd_pcm_uframes_t avail1;
	int ok = 0;
	
	while (1) {
		if (dshare->state == SND_PCM_STATE_RUNNING ||
		    dshare->state == SND_PCM_STATE_DRAINING)
			snd_pcm_dshare_sync_ptr(pcm);
		avail1 = snd_pcm_mmap_playback_avail(pcm);
		if (ok && *avail == avail1)
			break;
		*avail = avail1;
		*tstamp = snd_pcm_hw_fast_tstamp(dshare->spcm);
		ok = 1;
	}
	return 0;
}

static void snd_pcm_dshare_dump(snd_pcm_t *pcm, snd_output_t *out)
{
	snd_pcm_direct_t *dshare = pcm->private_data;

	snd_output_printf(out, "Direct Share PCM\n");
	if (pcm->setup) {
		snd_output_printf(out, "Its setup is:\n");
		snd_pcm_dump_setup(pcm, out);
	}
	if (dshare->spcm)
		snd_pcm_dump(dshare->spcm, out);
}

static const snd_pcm_ops_t snd_pcm_dshare_dummy_ops = {
	.close = snd_pcm_dshare_close,
};

static const snd_pcm_fast_ops_t snd_pcm_dshare_fast_dummy_ops;

static const snd_pcm_ops_t snd_pcm_dshare_ops = {
	.close = snd_pcm_dshare_close,
	.info = snd_pcm_direct_info,
	.hw_refine = snd_pcm_direct_hw_refine,
	.hw_params = snd_pcm_direct_hw_params,
	.hw_free = snd_pcm_direct_hw_free,
	.sw_params = snd_pcm_direct_sw_params,
	.channel_info = snd_pcm_direct_channel_info,
	.dump = snd_pcm_dshare_dump,
	.nonblock = snd_pcm_direct_nonblock,
	.async = snd_pcm_direct_async,
	.mmap = snd_pcm_direct_mmap,
	.munmap = snd_pcm_direct_munmap,
	.get_chmap = snd_pcm_direct_get_chmap,
	.set_chmap = snd_pcm_direct_set_chmap,
};

static const snd_pcm_fast_ops_t snd_pcm_dshare_fast_ops = {
	.status = snd_pcm_dshare_status,
	.state = snd_pcm_dshare_state,
	.hwsync = snd_pcm_dshare_hwsync,
	.delay = snd_pcm_dshare_delay,
	.prepare = snd_pcm_direct_prepare,
	.reset = snd_pcm_dshare_reset,
	.start = snd_pcm_dshare_start,
	.drop = snd_pcm_dshare_drop,
	.drain = snd_pcm_dshare_drain,
	.pause = snd_pcm_dshare_pause,
	.rewindable = snd_pcm_dshare_rewindable,
	.rewind = snd_pcm_dshare_rewind,
	.forwardable = snd_pcm_dshare_forwardable,
	.forward = snd_pcm_dshare_forward,
	.resume = snd_pcm_direct_resume,
	.link = NULL,
	.link_slaves = NULL,
	.unlink = NULL,
	.writei = snd_pcm_mmap_writei,
	.writen = snd_pcm_mmap_writen,
	.readi = snd_pcm_dshare_readi,
	.readn = snd_pcm_dshare_readn,
	.avail_update = snd_pcm_dshare_avail_update,
	.mmap_commit = snd_pcm_dshare_mmap_commit,
	.htimestamp = snd_pcm_dshare_htimestamp,
	.poll_descriptors = snd_pcm_direct_poll_descriptors,
	.poll_descriptors_count = NULL,
	.poll_revents = snd_pcm_direct_poll_revents,
};

/**
 * \brief Creates a new dshare PCM
 * \param pcmp Returns created PCM handle
 * \param name Name of PCM
 * \param opts Direct PCM configurations
 * \param params Parameters for slave
 * \param root Configuration root
 * \param sconf Slave configuration
 * \param stream PCM Direction (stream)
 * \param mode PCM Mode
 * \retval zero on success otherwise a negative error code
 * \warning Using of this function might be dangerous in the sense
 *          of compatibility reasons. The prototype might be freely
 *          changed in future.
 */
int snd_pcm_dshare_open(snd_pcm_t **pcmp, const char *name,
			struct snd_pcm_direct_open_conf *opts,
			struct slave_params *params,
			snd_config_t *root, snd_config_t *sconf,
			snd_pcm_stream_t stream, int mode)
{
	snd_pcm_t *pcm, *spcm = NULL;
	snd_pcm_direct_t *dshare;
	int ret, first_instance;
	unsigned int chn;

	assert(pcmp);

	if (stream != SND_PCM_STREAM_PLAYBACK) {
		SNDERR("The dshare plugin supports only playback stream");
		return -EINVAL;
	}

	ret = _snd_pcm_direct_new(&pcm, &dshare, SND_PCM_TYPE_DSHARE, name, opts, params, stream, mode);
	if (ret < 0)
		return ret;
	first_instance = ret;

	if (!dshare->bindings) {
		pcm->ops = &snd_pcm_dshare_dummy_ops;
		pcm->fast_ops = &snd_pcm_dshare_fast_dummy_ops;
	} else {
		pcm->ops = &snd_pcm_dshare_ops;
		pcm->fast_ops = &snd_pcm_dshare_fast_ops;
	}
	pcm->private_data = dshare;
	dshare->state = SND_PCM_STATE_OPEN;
	dshare->slowptr = opts->slowptr;
	dshare->max_periods = opts->max_periods;
	dshare->var_periodsize = opts->var_periodsize;
	dshare->hw_ptr_alignment = opts->hw_ptr_alignment;
	dshare->sync_ptr = snd_pcm_dshare_sync_ptr;

 retry:
	if (first_instance) {
		/* recursion is already checked in
		   snd_pcm_direct_get_slave_ipc_offset() */
		ret = snd_pcm_open_slave(&spcm, root, sconf, stream,
					 mode | SND_PCM_NONBLOCK, NULL);
		if (ret < 0) {
			SNDERR("unable to open slave");
			goto _err;
		}
	
		if (snd_pcm_type(spcm) != SND_PCM_TYPE_HW) {
			SNDERR("dshare plugin can be only connected to hw plugin");
			goto _err;
		}
		
		ret = snd_pcm_direct_initialize_slave(dshare, spcm, params);
		if (ret < 0) {
			SNDERR("unable to initialize slave");
			goto _err;
		}

		dshare->spcm = spcm;
		
		if (dshare->shmptr->use_server) {
			ret = snd_pcm_direct_server_create(dshare);
			if (ret < 0) {
				SNDERR("unable to create server");
				goto _err;
			}
		}

		dshare->shmptr->type = spcm->type;
	} else {
		if (dshare->shmptr->use_server) {
			/* up semaphore to avoid deadlock */
			snd_pcm_direct_semaphore_up(dshare, DIRECT_IPC_SEM_CLIENT);
			ret = snd_pcm_direct_client_connect(dshare);
			if (ret < 0) {
				SNDERR("unable to connect client");
				goto _err_nosem;
			}
			
			snd_pcm_direct_semaphore_down(dshare, DIRECT_IPC_SEM_CLIENT);
			ret = snd_pcm_direct_open_secondary_client(&spcm, dshare, "dshare_client");
			if (ret < 0)
				goto _err;

		} else {

			ret = snd_pcm_open_slave(&spcm, root, sconf, stream,
						 mode | SND_PCM_NONBLOCK |
						 SND_PCM_APPEND,
						 NULL);
			if (ret < 0) {
				/* all other streams have been closed;
				 * retry as the first instance
				 */
				if (ret == -EBADFD) {
					first_instance = 1;
					goto retry;
				}
				SNDERR("unable to open slave");
				goto _err;
			}
			if (snd_pcm_type(spcm) != SND_PCM_TYPE_HW) {
				SNDERR("dshare plugin can be only connected to hw plugin");
				ret = -EINVAL;
				goto _err;
			}
		
			ret = snd_pcm_direct_initialize_secondary_slave(dshare, spcm, params);
			if (ret < 0) {
				SNDERR("unable to initialize slave");
				goto _err;
			}
		}

		dshare->spcm = spcm;
	}

	for (chn = 0; dshare->bindings && (chn < dshare->channels); chn++) {
		unsigned int dchn = dshare->bindings ? dshare->bindings[chn] : chn;
		if (dchn != UINT_MAX)
			dshare->u.dshare.chn_mask |= (1ULL << dchn);
	}
	if (dshare->shmptr->u.dshare.chn_mask & dshare->u.dshare.chn_mask) {
		SNDERR("destination channel specified in bindings is already used");
		dshare->u.dshare.chn_mask = 0;
		ret = -EINVAL;
		goto _err;
	}
	dshare->shmptr->u.dshare.chn_mask |= dshare->u.dshare.chn_mask;
		
	ret = snd_pcm_direct_initialize_poll_fd(dshare);
	if (ret < 0) {
		SNDERR("unable to initialize poll_fd");
		goto _err;
	}

	pcm->poll_fd = dshare->poll_fd;
	pcm->poll_events = POLLIN;	/* it's different than other plugins */
	pcm->tstamp_type = spcm->tstamp_type;
	pcm->mmap_rw = 1;
	snd_pcm_set_hw_ptr(pcm, &dshare->hw_ptr, -1, 0);
	snd_pcm_set_appl_ptr(pcm, &dshare->appl_ptr, -1, 0);
	
	snd_pcm_direct_semaphore_up(dshare, DIRECT_IPC_SEM_CLIENT);

	*pcmp = pcm;
	return 0;
	
 _err:
	if (dshare->shmptr != (void *) -1)
		dshare->shmptr->u.dshare.chn_mask &= ~dshare->u.dshare.chn_mask;
	if (dshare->timer)
		snd_timer_close(dshare->timer);
	if (dshare->server)
		snd_pcm_direct_server_discard(dshare);
	if (dshare->client)
		snd_pcm_direct_client_discard(dshare);
	if (spcm)
		snd_pcm_close(spcm);
	if ((dshare->shmid >= 0) && (snd_pcm_direct_shm_discard(dshare))) {
		if (snd_pcm_direct_semaphore_discard(dshare))
			snd_pcm_direct_semaphore_final(dshare, DIRECT_IPC_SEM_CLIENT);
	} else
		snd_pcm_direct_semaphore_up(dshare, DIRECT_IPC_SEM_CLIENT);
 _err_nosem:
	free(dshare->bindings);
	free(dshare);
	snd_pcm_free(pcm);
	return ret;
}

/*! \page pcm_plugins

\section pcm_plugins_dshare Plugin: dshare

This plugin provides sharing channels.
Unlike \ref pcm_plugins_share "share plugin", this plugin doesn't need
the explicit server program but accesses the shared buffer concurrently
from each client as well as \ref pcm_plugins_dmix "dmix" and
\ref pcm_plugins_dsnoop "dsnoop" plugins do.
The parameters below are almost identical with these plugins.

\code
pcm.name {
	type dshare		# Direct sharing
	ipc_key INT		# unique IPC key
	ipc_key_add_uid BOOL	# add current uid to unique IPC key
	ipc_perm INT		# IPC permissions (octal, default 0600)
	hw_ptr_alignment STR	# Slave application and hw pointer alignment type
		# STR can be one of the below strings :
		# no (or off)
		# roundup
		# rounddown
		# auto (default)
	tstamp_type STR		# timestamp type
				# STR can be one of the below strings :
				# default, gettimeofday, monotonic, monotonic_raw
	slave STR
	# or
	slave {			# Slave definition
		pcm STR		# slave PCM name
		# or
		pcm { }		# slave PCM definition
		format STR	# format definition
		rate INT	# rate definition
		channels INT
		period_time INT	# in usec
		# or
		period_size INT	# in frames
		buffer_time INT	# in usec
		# or
		buffer_size INT # in frames
		periods INT	# when buffer_size or buffer_time is not specified
	}
	bindings {		# note: this is client independent!!!
		N INT		# maps slave channel to client channel N
	}
	slowptr BOOL		# slow but more precise pointer updates
}
\endcode

<code>hw_ptr_alignment</code> specifies slave application and hw
pointer alignment type. By default hw_ptr_alignment is auto. Below are
the possible configurations:
- no: minimal latency with minimal frames dropped at startup. But
  wakeup of application (return from snd_pcm_wait() or poll()) can
  take up to 2 * period.
- roundup: It is guaranteed that all frames will be played at
  startup. But the latency will increase upto period-1 frames.
- rounddown: It is guaranteed that a wakeup will happen for each
  period and frames can be written from application. But on startup
  upto period-1 frames will be dropped.
- auto: Selects the best approach depending on the used period and
  buffer size.
  If the application buffer size is < 2 * application period,
  "roundup" will be selected to avoid under runs. If the slave_period
  is < 10ms we could expect that there are low latency
  requirements. Therefore "rounddown" will be chosen to avoid long
  wakeup times. Such wakeup delay could otherwise end up with Xruns in
  case of a dependency to another sound device (e.g. forwarding of
  microphone to speaker). Else "no" will be chosen.

\subsection pcm_plugins_dshare_funcref Function reference

<UL>
  <LI>snd_pcm_dshare_open()
  <LI>_snd_pcm_dshare_open()
</UL>

*/

/**
 * \brief Creates a new dshare PCM
 * \param pcmp Returns created PCM handle
 * \param name Name of PCM
 * \param root Root configuration node
 * \param conf Configuration node with dshare PCM description
 * \param stream PCM Stream
 * \param mode PCM Mode
 * \warning Using of this function might be dangerous in the sense
 *          of compatibility reasons. The prototype might be freely
 *          changed in future.
 */
int _snd_pcm_dshare_open(snd_pcm_t **pcmp, const char *name,
		       snd_config_t *root, snd_config_t *conf,
		       snd_pcm_stream_t stream, int mode)
{
	snd_config_t *sconf;
	struct slave_params params;
	struct snd_pcm_direct_open_conf dopen;
	int bsize, psize;
	int err;

	err = snd_pcm_direct_parse_open_conf(root, conf, stream, &dopen);
	if (err < 0)
		return err;

	/* the default settings, it might be invalid for some hardware */
	params.format = SND_PCM_FORMAT_S16;
	params.rate = 48000;
	params.channels = 2;
	params.period_time = -1;
	params.buffer_time = -1;
	bsize = psize = -1;
	params.periods = 3;
	err = snd_pcm_slave_conf(root, dopen.slave, &sconf, 8,
				 SND_PCM_HW_PARAM_FORMAT, SCONF_UNCHANGED, &params.format,
				 SND_PCM_HW_PARAM_RATE, 0, &params.rate,
				 SND_PCM_HW_PARAM_CHANNELS, 0, &params.channels,
				 SND_PCM_HW_PARAM_PERIOD_TIME, 0, &params.period_time,
				 SND_PCM_HW_PARAM_BUFFER_TIME, 0, &params.buffer_time,
				 SND_PCM_HW_PARAM_PERIOD_SIZE, 0, &psize,
				 SND_PCM_HW_PARAM_BUFFER_SIZE, 0, &bsize,
				 SND_PCM_HW_PARAM_PERIODS, 0, &params.periods);
	if (err < 0)
		return err;

	/* set a reasonable default */
	if (psize == -1 && params.period_time == -1)
		params.period_time = 125000;	/* 0.125 seconds */

	if (params.format == -2)
		params.format = SND_PCM_FORMAT_UNKNOWN;

	params.period_size = psize;
	params.buffer_size = bsize;

	err = snd_pcm_dshare_open(pcmp, name, &dopen, &params,
				  root, sconf, stream, mode);
	snd_config_delete(sconf);
	return err;
}
#ifndef DOC_HIDDEN
SND_DLSYM_BUILD_VERSION(_snd_pcm_dshare_open, SND_PCM_DLSYM_VERSION);
#endif
