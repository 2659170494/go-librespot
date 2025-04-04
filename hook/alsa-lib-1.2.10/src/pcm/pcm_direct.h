#ifdef __cplusplus
extern "C" {
#endif
/*
 *  PCM - Direct Stream Mixing
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
#include "pcm_local.h"  
#include "../timer/timer_local.h"

extern int semctl (int __semid, int __semnum, int __cmd, ...) ;
extern int semget (key_t __key, int __nsems, int __semflg) ;
extern int semop (int __semid, struct sembuf *__sops, size_t __nsops) ;
extern int shmget(key_t key, size_t size, int shmflg);
extern void *shmat(int shmid, void const* shmaddr, int shmflg);
extern int shmdt(void const* shmaddr);

#define DIRECT_IPC_SEMS         1
#define DIRECT_IPC_SEM_CLIENT   0
/* Seconds representing in Milli seconds */
#define SEC_TO_MS               1000
/* slave_period time for low latency requirements in ms */
#define LOW_LATENCY_PERIOD_TIME 10


typedef void (mix_areas_t)(unsigned int size,
			   volatile void *dst, void *src,
			   volatile signed int *sum, size_t dst_step,
			   size_t src_step, size_t sum_step);

typedef void (mix_areas_16_t)(unsigned int size,
			      volatile signed short *dst, signed short *src,
			      volatile signed int *sum, size_t dst_step,
			      size_t src_step, size_t sum_step);

typedef void (mix_areas_32_t)(unsigned int size,
			      volatile signed int *dst, signed int *src,
			      volatile signed int *sum, size_t dst_step,
			      size_t src_step, size_t sum_step);

typedef void (mix_areas_24_t)(unsigned int size,
			      volatile unsigned char *dst, unsigned char *src,
			      volatile signed int *sum, size_t dst_step,
			      size_t src_step, size_t sum_step);

typedef void (mix_areas_u8_t)(unsigned int size,
			      volatile unsigned char *dst, unsigned char *src,
			      volatile signed int *sum, size_t dst_step,
			      size_t src_step, size_t sum_step);

typedef enum snd_pcm_direct_hw_ptr_alignment {
	SND_PCM_HW_PTR_ALIGNMENT_NO = 0,	/* use the hw_ptr as is and do no rounding */
	SND_PCM_HW_PTR_ALIGNMENT_ROUNDUP = 1,	/* round the slave_appl_ptr up to slave_period */
	SND_PCM_HW_PTR_ALIGNMENT_ROUNDDOWN = 2,	/* round slave_hw_ptr and slave_appl_ptr down to slave_period */
	SND_PCM_HW_PTR_ALIGNMENT_AUTO = 3	/* automatic selection */
} snd_pcm_direct_hw_ptr_alignment_t;

struct slave_params {
	snd_pcm_format_t format;
	int rate;
	int channels;
	int period_time;
	int buffer_time;
	snd_pcm_sframes_t period_size;
	snd_pcm_sframes_t buffer_size;
	unsigned int periods;
};

/* shared among direct plugin clients - be careful to be 32/64bit compatible! */
typedef struct {
	unsigned int magic;			/* magic number */
	char socket_name[256];			/* name of communication socket */
	snd_pcm_type_t type;			/* PCM type (currently only hw) */
	int use_server;
	struct {
		unsigned int format;
		snd_interval_t rate;
		snd_interval_t buffer_size;
		snd_interval_t buffer_time;
		snd_interval_t period_size;
		snd_interval_t period_time;
		snd_interval_t periods;
	} hw;
	struct {
		/* copied to slave PCMs */
		snd_pcm_access_t access;
		snd_pcm_format_t format;
		snd_pcm_subformat_t subformat;
		unsigned int channels;
		unsigned int rate;
		unsigned int period_size;
		unsigned int period_time;
		snd_interval_t periods;
		snd_pcm_tstamp_t tstamp_mode;
		snd_pcm_tstamp_type_t tstamp_type;
		unsigned int period_step;
		unsigned int sleep_min; /* not used */
		unsigned int avail_min;
		unsigned int start_threshold;	
		unsigned int stop_threshold;	
		unsigned int silence_threshold;
		unsigned int silence_size;
		unsigned int recoveries;	/* no of executed recoveries on slave*/
		unsigned long long boundary;
		unsigned int info;
		unsigned int msbits;
		unsigned int rate_num;
		unsigned int rate_den;
		unsigned int hw_flags;
		unsigned int fifo_size;
		unsigned int buffer_size;
		snd_interval_t buffer_time;
		unsigned int sample_bits;
		unsigned int frame_bits;
	} s;
	union {
		struct {
			unsigned long long chn_mask;
		} dshare;
	} u;
} snd_pcm_direct_share_t;

typedef struct snd_pcm_direct snd_pcm_direct_t;

struct snd_pcm_direct {
	snd_pcm_type_t type;		/* type (dmix, dsnoop, dshare) */
	key_t ipc_key;			/* IPC key for semaphore and memory */
	mode_t ipc_perm;		/* IPC socket permissions */
	int ipc_gid;			/* IPC socket gid */
	int semid;			/* IPC global semaphore identification */
	int locked[DIRECT_IPC_SEMS];	/* local lock counter */
	int shmid;			/* IPC global shared memory identification */
	snd_pcm_direct_share_t *shmptr;	/* pointer to shared memory area */
	snd_pcm_t *spcm; 		/* slave PCM handle */
	snd_pcm_uframes_t appl_ptr;
	snd_pcm_uframes_t last_appl_ptr;
	snd_pcm_uframes_t hw_ptr;
	snd_pcm_uframes_t avail_max;
	snd_pcm_uframes_t slave_appl_ptr;
	snd_pcm_uframes_t slave_hw_ptr;
	snd_pcm_uframes_t slave_period_size;
	snd_pcm_uframes_t slave_buffer_size;
	snd_pcm_uframes_t slave_boundary;
	int (*sync_ptr)(snd_pcm_t *pcm);
	snd_pcm_state_t state;
	snd_htimestamp_t trigger_tstamp;
	snd_htimestamp_t update_tstamp;
	int server, client;
	int comm_fd;			/* communication file descriptor (socket) */
	int hw_fd;			/* hardware file descriptor */
	struct pollfd timer_fd;
	int poll_fd;
	int tread: 1;
	int timer_need_poll: 1;
	unsigned int timer_events;
	unsigned int timer_ticks;
	int server_fd;
	pid_t server_pid;
	snd_timer_t *timer; 		/* timer used as poll_fd */
	int interleaved;	 	/* we have interleaved buffer */
	int slowptr;			/* use slow but more precise ptr updates */
	int max_periods;		/* max periods (-1 = fixed periods, 0 = max buffer size) */
	int var_periodsize;		/* allow variable period size if max_periods is != -1*/
	unsigned int channels;		/* client's channels */
	unsigned int *bindings;
	unsigned int recoveries;	/* mirror of executed recoveries on slave */
	int direct_memory_access;	/* use arch-optimized buffer RW */
	snd_pcm_direct_hw_ptr_alignment_t hw_ptr_alignment;
	int tstamp_type;		/* cached from conf, can be -1(default) on top of real types */
	union {
		struct {
			int shmid_sum;			/* IPC global sum ring buffer memory identification */
			signed int *sum_buffer;		/* shared sum buffer */
			mix_areas_16_t *mix_areas_16;
			mix_areas_32_t *mix_areas_32;
			mix_areas_24_t *mix_areas_24;
			mix_areas_u8_t *mix_areas_u8;
			mix_areas_16_t *remix_areas_16;
			mix_areas_32_t *remix_areas_32;
			mix_areas_24_t *remix_areas_24;
			mix_areas_u8_t *remix_areas_u8;
			unsigned int use_sem;
		} dmix;
		struct {
			unsigned long long chn_mask;
		} dshare;
	} u;
	void (*server_free)(snd_pcm_direct_t *direct);
};

/* make local functions really local */
#define snd_pcm_direct_semaphore_create_or_connect \
	snd1_pcm_direct_semaphore_create_or_connect
#define snd_pcm_direct_shm_create_or_connect \
	snd1_pcm_direct_shm_create_or_connect
#define snd_pcm_direct_shm_discard \
	snd1_pcm_direct_shm_discard
#define snd_pcm_direct_server_create \
	snd1_pcm_direct_server_create
#define snd_pcm_direct_server_discard \
	snd1_pcm_direct_server_discard
#define snd_pcm_direct_client_connect \
	snd1_pcm_direct_client_connect
#define snd_pcm_direct_client_discard \
	snd1_pcm_direct_client_discard
#define snd_pcm_direct_initialize_slave \
	snd1_pcm_direct_initialize_slave
#define snd_pcm_direct_initialize_secondary_slave \
	snd1_pcm_direct_initialize_secondary_slave
#define snd_pcm_direct_initialize_poll_fd \
	snd1_pcm_direct_initialize_poll_fd
#define snd_pcm_direct_check_interleave \
	snd1_pcm_direct_check_interleave
#define snd_pcm_direct_parse_bindings \
	snd1_pcm_direct_parse_bindings
#define snd_pcm_direct_nonblock \
	snd1_pcm_direct_nonblock
#define snd_pcm_direct_async \
	snd1_pcm_direct_async
#define snd_pcm_direct_poll_descriptors \
	snd1_pcm_direct_poll_descriptors
#define snd_pcm_direct_poll_revents \
	snd1_pcm_direct_poll_revents
#define snd_pcm_direct_info \
	snd1_pcm_direct_info
#define snd_pcm_direct_hw_refine \
	snd1_pcm_direct_hw_refine
#define snd_pcm_direct_hw_params \
	snd1_pcm_direct_hw_params
#define snd_pcm_direct_hw_free \
	snd1_pcm_direct_hw_free
#define snd_pcm_direct_sw_params \
	snd1_pcm_direct_sw_params
#define snd_pcm_direct_channel_info \
	snd1_pcm_direct_channel_info
#define snd_pcm_direct_mmap \
	snd1_pcm_direct_mmap
#define snd_pcm_direct_munmap \
	snd1_pcm_direct_munmap
#define snd_pcm_direct_prepare \
	snd1_pcm_direct_prepare
#define snd_pcm_direct_resume \
	snd1_pcm_direct_resume
#define snd_pcm_direct_timer_stop \
	snd1_pcm_direct_timer_stop
#define snd_pcm_direct_clear_timer_queue \
	snd1_pcm_direct_clear_timer_queue
#define snd_pcm_direct_set_timer_params \
	snd1_pcm_direct_set_timer_params
#define snd_pcm_direct_open_secondary_client \
	snd1_pcm_direct_open_secondary_client
#define snd_pcm_direct_parse_open_conf \
	snd1_pcm_direct_parse_open_conf
#define snd_pcm_direct_query_chmaps \
	snd1_pcm_direct_query_chmaps
#define snd_pcm_direct_get_chmap \
	snd1_pcm_direct_get_chmap
#define snd_pcm_direct_set_chmap \
	snd1_pcm_direct_set_chmap
#define snd_pcm_direct_reset_slave_ptr \
	snd1_pcm_direct_reset_slave_ptr
#define snd_pcm_direct_check_xrun \
	snd1_pcm_direct_check_xrun
#define snd_pcm_direct_slave_recover \
	snd1_pcm_direct_slave_recover

int snd_pcm_direct_semaphore_create_or_connect(snd_pcm_direct_t *dmix);

static inline int snd_pcm_direct_semaphore_discard(snd_pcm_direct_t *dmix)
{
	if (dmix->semid >= 0) {
		if (semctl(dmix->semid, 0, IPC_RMID, NULL) < 0)
			return -errno;
		dmix->semid = -1;
	}
	return 0;
}

static inline int snd_pcm_direct_semaphore_down(snd_pcm_direct_t *dmix, int sem_num)
{
	struct sembuf op[2] = { { sem_num, 0, 0 }, { sem_num, 1, SEM_UNDO } };
	int err = semop(dmix->semid, op, 2);
	if (err == 0)
		dmix->locked[sem_num]++;
	else if (err == -1)
		err = -errno;
	return err;
}

static inline int snd_pcm_direct_semaphore_up(snd_pcm_direct_t *dmix, int sem_num)
{
	struct sembuf op = { sem_num, -1, SEM_UNDO | IPC_NOWAIT };
	int err = semop(dmix->semid, &op, 1);
	if (err == 0)
		dmix->locked[sem_num]--;
	else if (err == -1)
		err = -errno;
	return err;
}

static inline int snd_pcm_direct_semaphore_final(snd_pcm_direct_t *dmix, int sem_num)
{
	if (dmix->locked[sem_num] != 1) {
		SNDMSG("invalid semaphore count to finalize %d: %d", sem_num, dmix->locked[sem_num]);
		return -EBUSY;
	}
	return snd_pcm_direct_semaphore_up(dmix, sem_num);
}

int snd_pcm_direct_shm_create_or_connect(snd_pcm_direct_t *dmix);
int snd_pcm_direct_shm_discard(snd_pcm_direct_t *dmix);
int snd_pcm_direct_server_create(snd_pcm_direct_t *dmix);
int snd_pcm_direct_server_discard(snd_pcm_direct_t *dmix);
int snd_pcm_direct_client_connect(snd_pcm_direct_t *dmix);
int snd_pcm_direct_client_discard(snd_pcm_direct_t *dmix);
int snd_pcm_direct_initialize_slave(snd_pcm_direct_t *dmix, snd_pcm_t *spcm, struct slave_params *params);
int snd_pcm_direct_initialize_secondary_slave(snd_pcm_direct_t *dmix, snd_pcm_t *spcm, struct slave_params *params);
int snd_pcm_direct_initialize_poll_fd(snd_pcm_direct_t *dmix);
int snd_pcm_direct_check_interleave(snd_pcm_direct_t *dmix, snd_pcm_t *pcm);
int snd_pcm_direct_parse_bindings(snd_pcm_direct_t *dmix,
				  struct slave_params *params,
				  snd_config_t *cfg);
int snd_pcm_direct_nonblock(snd_pcm_t *pcm, int nonblock);
int snd_pcm_direct_async(snd_pcm_t *pcm, int sig, pid_t pid);
int snd_pcm_direct_poll_descriptors(snd_pcm_t *pcm, struct pollfd *pfds,
				    unsigned int space);
int snd_pcm_direct_poll_revents(snd_pcm_t *pcm, struct pollfd *pfds, unsigned int nfds, unsigned short *revents);
int snd_pcm_direct_info(snd_pcm_t *pcm, snd_pcm_info_t * info);
int snd_pcm_direct_hw_refine(snd_pcm_t *pcm, snd_pcm_hw_params_t *params);
int snd_pcm_direct_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t * params);
int snd_pcm_direct_hw_free(snd_pcm_t *pcm);
int snd_pcm_direct_sw_params(snd_pcm_t *pcm, snd_pcm_sw_params_t * params);
int snd_pcm_direct_channel_info(snd_pcm_t *pcm, snd_pcm_channel_info_t * info);
int snd_pcm_direct_mmap(snd_pcm_t *pcm);
int snd_pcm_direct_munmap(snd_pcm_t *pcm);
int snd_pcm_direct_prepare(snd_pcm_t *pcm);
int snd_pcm_direct_resume(snd_pcm_t *pcm);
int snd_pcm_direct_timer_stop(snd_pcm_direct_t *dmix);
int snd_pcm_direct_clear_timer_queue(snd_pcm_direct_t *dmix);
int snd_pcm_direct_set_timer_params(snd_pcm_direct_t *dmix);
int snd_pcm_direct_open_secondary_client(snd_pcm_t **spcmp, snd_pcm_direct_t *dmix, const char *client_name);

snd_pcm_chmap_query_t **snd_pcm_direct_query_chmaps(snd_pcm_t *pcm);
snd_pcm_chmap_t *snd_pcm_direct_get_chmap(snd_pcm_t *pcm);
int snd_pcm_direct_set_chmap(snd_pcm_t *pcm, const snd_pcm_chmap_t *map);
int snd_pcm_direct_slave_recover(snd_pcm_direct_t *direct);
int snd_pcm_direct_check_xrun(snd_pcm_direct_t *direct, snd_pcm_t *pcm);
int snd_timer_async(snd_timer_t *timer, int sig, pid_t pid);
struct timespec snd_pcm_hw_fast_tstamp(snd_pcm_t *pcm);
void snd_pcm_direct_reset_slave_ptr(snd_pcm_t *pcm, snd_pcm_direct_t *dmix, snd_pcm_uframes_t hw_ptr);

struct snd_pcm_direct_open_conf {
	key_t ipc_key;
	mode_t ipc_perm;
	int ipc_gid;
	int slowptr;
	int max_periods;
	int var_periodsize;
	int direct_memory_access;
	snd_pcm_direct_hw_ptr_alignment_t hw_ptr_alignment;
	int tstamp_type;
	snd_config_t *slave;
	snd_config_t *bindings;
};

int snd_pcm_direct_parse_open_conf(snd_config_t *root, snd_config_t *conf, int stream, struct snd_pcm_direct_open_conf *rec);

int _snd_pcm_direct_new(snd_pcm_t **pcmp, snd_pcm_direct_t **_dmix, int type,
			const char *name, struct snd_pcm_direct_open_conf *opts,
			struct slave_params *params, snd_pcm_stream_t stream, int mode);
#ifdef __cplusplus
}
#endif