/*
 * ACSD shared include file
 *
 * Copyright (C) 2013, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: acsd.h 398225 2013-04-23 22:33:56Z $
 */

#ifndef _acsd_h_
#define _acsd_h_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <assert.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmtimer.h>
#include <bcmendian.h>

#include <shutils.h>
#include <bcmendian.h>
#include <bcmwifi_channels.h>
#include <wlioctl.h>
#include <wlutils.h>

#include <security_ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "acs_dfsr.h"

#define ACSD_DEBUG_ERROR	0x0001
#define ACSD_DEBUG_WARNING	0x0002
#define ACSD_DEBUG_INFO		0x0004
#define ACSD_DEBUG_DETAIL	0x0008
#define ACSD_DEBUG_CHANIM	0x0010
#define ACSD_DEBUG_FCS		0x0020
#define ACSD_DEBUG_DFSR		0x0040

#define ACSD_VERSION 1
#define ACSD_DFLT_FD			-1
#define ACSD_DFLT_CLI_PORT	  5916 /* need to double check if it is used */

extern bool acsd_swap;
#define htod32(i) (acsd_swap?bcmswap32(i):(uint32)(i))
#define htod16(i) (acsd_swap?bcmswap16(i):(uint16)(i))
#define dtoh32(i) (acsd_swap?bcmswap32(i):(uint32)(i))
#define dtoh16(i) (acsd_swap?bcmswap16(i):(uint16)(i))
#define htodchanspec(i) (acsd_swap?htod16(i):i)
#define dtohchanspec(i) (acsd_swap?dtoh16(i):i)
#define htodenum(i) (acsd_swap?((sizeof(i) == 4) ? \
			htod32(i) : ((sizeof(i) == 2) ? htod16(i) : i)):i)
#define dtohenum(i) (acsd_swap?((sizeof(i) == 4) ? \
			dtoh32(i) : ((sizeof(i) == 2) ? htod16(i) : i)):i)

extern int acsd_debug_level;
#define ACSD_ERROR(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_ERROR) \
			printf("ACSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define ACSD_WARNING(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_WARNING) \
			printf("ACSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define ACSD_INFO(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_INFO) \
			printf("ACSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define ACSD_DEBUG(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_DETAIL) \
			printf("ACSD >>%s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define ACSD_CHANIM(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_CHANIM) \
			printf(fmt, ##arg); } while (0)

#define ACSD_FCS(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_FCS) \
			printf(fmt, ##arg); } while (0)

#define ACSD_DFSR(fmt, arg...) \
		do { if (acsd_debug_level & ACSD_DEBUG_DFSR) \
			printf("DFSR:%20s(%d): "fmt, __FUNCTION__, __LINE__, ##arg); } while (0)

#define ACSD_PRINT(fmt, arg...) \
		do { printf("acsd: "fmt, ##arg); } while (0)

#define HERE ACSD_ERROR("trace\n")

#define ACS_FREE(data_ptr)	\
	do { 					\
		if (data_ptr) 		\
			free(data_ptr); \
		data_ptr = NULL; 	\
	} while (0)

#define ACS_ERR(ret, string) \
	do {	\
		if (ret < 0) {	\
			ACSD_ERROR(string "ret code: %d\n", ret); \
			return ret;	\
		} \
	} while (0)

#define SSID_FMT_BUF_LEN 4*32+1	/* Length for SSID format string */

/* other bss info derived from scan result */
typedef struct acs_chan_bssinfo {
	uint8 channel;
	uint8 aAPs;	  /* # of 20MHz LG/HT/VHT BSSs seen in 5G band */
	uint8 bAPs;   /* # of 20MHz 11b BSSs seen in 2.4G band */
	uint8 gAPs;	  /* # of 20MHz LG/HT BSSs seen in 2.4G band */
	uint8 lSBs;	  /* # of 40MHz HT/VHT BSSs using this as "lower" ctl channel */
	uint8 uSBs;	  /* # of 40MHz HT/VHT BSSs using this as "upper" ctl channel */
	uint8 nEXs;	  /* # of 40MHz HT/VTH BSSs using this as extension channel */
	uint8 llSBs;  /* # of 80MHz VHT BSSs using this as LL ctl channel */
	uint8 luSBs;  /* # of 80MHz VHT BSSs using this as LU ctl channel */
	uint8 ulSBs;  /* # of 80MHz VHT BSSs using this as UL ctl channel */
	uint8 uuSBs;  /* # of 80MHz VHT BSSs using this as UU ctl channel */
	uint8 wEX20s; /* # of 80MHz VHT BSSs using this as 20MHz extension */
	uint8 wEX40s; /* # of 80MHz VHT BSSs using this as (one side of) 40MHz extension */
} acs_chan_bssinfo_t;

typedef struct acs_channel {
	uint8 control;
	uint8 ext20;
	uint8 ext40[2];
} acs_channel_t;

typedef struct ch_score {
	int score;
	int weight;
} ch_score_t;

#define CH_SCORE_BSS		0	/* number of bss */
#define CH_SCORE_BUSY		1	/* channel occupancy */
#define CH_SCORE_INTF		2	/* interference */
#define CH_SCORE_INTFADJ	3	/* interference adjustment, include neighboring channels */
#define CH_SCORE_FCS		4	/* FCS */
#define CH_SCORE_TXPWR		5	/* TX pwr consideration */
#define CH_SCORE_BGNOISE	6
#define CH_SCORE_TOTAL		7
#define CH_SCORE_CNS		8
/* adjacent channel score(number of bss's using the adjacent channel spec) */
#define CH_SCORE_ADJ		9
#define CH_SCORE_MAX		10

#define ACS_INVALID_COEX	0x1
#define ACS_INVALID_INTF_CCA	0x2
#define ACS_INVALID_INTF_BGN	0x4
#define ACS_INVALID_OVLP	0x8
#define ACS_INVALID_NOISE	0x10
#define ACS_INVALID_ALIGN	0x20
#define ACS_INVALID_144		0x40
#define ACS_INVALID_DFS		0x80
#define ACS_INVALID_CHAN_FLOP_PERIOD	0x100
#define ACS_INVALID_EXCL		0x200
#define ACS_INVALID_MISMATCH_SB		0x400
#define ACS_INVALID_SAMECHAN		0x800

#define ACSD_BUFSIZE_4K	4096

typedef struct ch_candidate {
	chanspec_t chspec;
	bool valid;
	bool is_dfs;
	uint16 reason;
	uint8 in_use;	/* number of bss's using the identical channel spec */
	ch_score_t chscore[CH_SCORE_MAX];
} ch_candidate_t;

/* all the policy related configuration goes here */
extern int acs_safe_get_conf(char *outval, int outval_size, char *name);
extern void dump_networks(char *network_buf);
extern char * acsd_malloc(int bufsize);
extern void sleep_ms(const unsigned int ms);
extern int swrite(int fd, char *buf, unsigned int size);
extern int sread(int fd, char *buf, unsigned int size);
extern void acs_dump_score(ch_score_t* score_p);
extern int wl_format_ssid(char* ssid_buf, uint8* ssid, int ssid_len);
extern char *wl_ether_etoa(const struct ether_addr *n);
#endif /*  _acsd_h_ */
