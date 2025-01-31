/*
    ipv6cp.c - PPP IPV6 Control Protocol.
    Copyright (C) 1999  Tommi Komulainen <Tommi.Komulainen@iki.fi>

    Redistribution and use in source and binary forms are permitted
    provided that the above copyright notice and this paragraph are
    duplicated in all such forms.  The name of the author may not be
    used to endorse or promote products derived from this software
    without specific prior written permission.
    THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
    WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

/*  Original version, based on RFC2023 :

    Copyright (c) 1995, 1996, 1997 Francis.Dupont@inria.fr, INRIA Rocquencourt,
    Alain.Durand@imag.fr, IMAG,
    Jean-Luc.Richier@imag.fr, IMAG-LSR.

    Copyright (c) 1998, 1999 Francis.Dupont@inria.fr, GIE DYADE,
    Alain.Durand@imag.fr, IMAG,
    Jean-Luc.Richier@imag.fr, IMAG-LSR.

    Ce travail a 閠?fait au sein du GIE DYADE (Groupement d'Int閞阾
    蒫onomique ayant pour membres BULL S.A. et l'INRIA).

    Ce logiciel informatique est disponible aux conditions
    usuelles dans la recherche, c'est-?dire qu'il peut
    阾re utilis? copi? modifi? distribu??l'unique
    condition que ce texte soit conserv?afin que
    l'origine de ce logiciel soit reconnue.

    Le nom de l'Institut National de Recherche en Informatique
    et en Automatique (INRIA), de l'IMAG, ou d'une personne morale
    ou physique ayant particip??l'閘aboration de ce logiciel ne peut
    阾re utilis?sans son accord pr閍lable explicite.

    Ce logiciel est fourni tel quel sans aucune garantie,
    support ou responsabilit?d'aucune sorte.
    Ce logiciel est d閞iv?de sources d'origine
    "University of California at Berkeley" et
    "Digital Equipment Corporation" couvertes par des copyrights.

    L'Institut d'Informatique et de Math閙atiques Appliqu閑s de Grenoble (IMAG)
    est une f閐閞ation d'unit閟 mixtes de recherche du CNRS, de l'Institut National
    Polytechnique de Grenoble et de l'Universit?Joseph Fourier regroupant
    sept laboratoires dont le laboratoire Logiciels, Syst鑝es, R閟eaux (LSR).

    This work has been done in the context of GIE DYADE (joint R & D venture
    between BULL S.A. and INRIA).

    This software is available with usual "research" terms
    with the aim of retain credits of the software. 
    Permission to use, copy, modify and distribute this software for any
    purpose and without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies,
    and the name of INRIA, IMAG, or any contributor not be used in advertising
    or publicity pertaining to this material without the prior explicit
    permission. The software is provided "as is" without any
    warranties, support or liabilities of any kind.
    This software is derived from source code from
    "University of California at Berkeley" and
    "Digital Equipment Corporation" protected by copyrights.

    Grenoble's Institute of Computer Science and Applied Mathematics (IMAG)
    is a federation of seven research units funded by the CNRS, National
    Polytechnic Institute of Grenoble and University Joseph Fourier.
    The research unit in Software, Systems, Networks (LSR) is member of IMAG.
*/

/*
 * Derived from :
 *
 *
 * ipcp.c - PPP IP Control Protocol.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


/*
 * TODO: 
 *
 * Proxy Neighbour Discovery.
 *
 * Better defines for selecting the ordering of
 *   interface up / set address. (currently checks for __linux__,
 *   since SVR4 && (SNI || __USLC__) didn't work properly)
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <net/if.h>
#include <signal.h>
#include "pppd.h"
#include "fsm.h"
#include "ipcp.h"
#include "ipv6cp.h"
#include "magic.h"
#include "pathnames.h"
#if __GLIBC__ >= 2
#include <asm/types.h>		/* glibc 2 conflicts with linux/types.h */
#else
#include <linux/types.h>
#endif
#include <sys/wait.h>
#include <fcntl.h>

#ifdef SUPPORT_ATP_FON
#include "foncmsapi.h"
#endif


#ifdef INET6
#ifndef _LINUX_IN6_H
/*
 *    This is in linux/include/net/ipv6.h.
 */
struct in6_ifreq {
    struct in6_addr ifr6_addr;
    __u32 ifr6_prefixlen;
    unsigned int ifr6_ifindex;
};
#endif
#endif /* INET6 */

extern int g_iIpv6Mode;
extern int g_iIpv6MAddr;
extern struct ip6_addr g_stStaticIP6Addr;
extern pid_t ipv6cp_dhcp_pid;
eui64_t eui64_ifc_id;
static int g_iDhcp6cPid = -1;


static const char rcsid[] = RCSID;

/* global vars */
ipv6cp_options ipv6cp_wantoptions[NUM_PPP];     /* Options that we want to request */
ipv6cp_options ipv6cp_gotoptions[NUM_PPP];	/* Options that peer ack'd */
ipv6cp_options ipv6cp_allowoptions[NUM_PPP];	/* Options we allow peer to request */
ipv6cp_options ipv6cp_hisoptions[NUM_PPP];	/* Options that we ack'd */
int no_ifaceid_neg = 0;
eui64_t ouroldid;
eui64_t hisoldid;

/* local vars */
static int ipv6cp_is_up;


/*
 * Callbacks for fsm code.  (CI = Configuration Information)
 */
static void ipv6cp_resetci __P((fsm *));	/* Reset our CI */
static int  ipv6cp_cilen __P((fsm *));	        /* Return length of our CI */
static void ipv6cp_addci __P((fsm *, u_char *, int *)); /* Add our CI */
static int  ipv6cp_ackci __P((fsm *, u_char *, int));	/* Peer ack'd our CI */
static int  ipv6cp_nakci __P((fsm *, u_char *, int));	/* Peer nak'd our CI */
static int  ipv6cp_rejci __P((fsm *, u_char *, int));	/* Peer rej'd our CI */
static int  ipv6cp_reqci __P((fsm *, u_char *, int *, int)); /* Rcv CI */
static void ipv6cp_up __P((fsm *));		/* We're UP */
static void ipv6cp_down __P((fsm *));		/* We're DOWN */
static void ipv6cp_finished __P((fsm *));	/* Don't need lower layer */

fsm ipv6cp_fsm[NUM_PPP];		/* IPV6CP fsm structure */

static fsm_callbacks ipv6cp_callbacks = { /* IPV6CP callback routines */
    ipv6cp_resetci,		/* Reset our Configuration Information */
    ipv6cp_cilen,		/* Length of our Configuration Information */
    ipv6cp_addci,		/* Add our Configuration Information */
    ipv6cp_ackci,		/* ACK our Configuration Information */
    ipv6cp_nakci,		/* NAK our Configuration Information */
    ipv6cp_rejci,		/* Reject our Configuration Information */
    ipv6cp_reqci,		/* Request peer's Configuration Information */
    ipv6cp_up,			/* Called when fsm reaches OPENED state */
    ipv6cp_down,		/* Called when fsm leaves OPENED state */
    NULL,			/* Called when we want the lower layer up */
    ipv6cp_finished,		/* Called when we want the lower layer down */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
    NULL,			/* Called to handle protocol-specific codes */
    "IPV6CP"			/* String name of protocol */
};

/*
 * Command-line options.
 */
static int setifaceid __P((char **arg));
static void printifaceid __P((option_t *,
			      void (*)(void *, char *, ...), void *));

static option_t ipv6cp_option_list[] = {
    { "ipv6", o_special, (void *)setifaceid,
      "Set interface identifiers for IPV6",
      OPT_A2PRINTER, (void *)printifaceid },

    { "+ipv6", o_bool, &ipv6cp_protent.enabled_flag,
      "Enable IPv6 and IPv6CP", OPT_PRIO | 1 },
    { "noipv6", o_bool, &ipv6cp_protent.enabled_flag,
      "Disable IPv6 and IPv6CP", OPT_PRIOSUB },
    { "-ipv6", o_bool, &ipv6cp_protent.enabled_flag,
      "Disable IPv6 and IPv6CP", OPT_PRIOSUB | OPT_ALIAS },

    { "ipv6cp-accept-local", o_bool, &ipv6cp_allowoptions[0].accept_local,
      "Accept peer's interface identifier for us", 1 },

    { "ipv6cp-use-ipaddr", o_bool, &ipv6cp_allowoptions[0].use_ip,
      "Use (default) IPv4 address as interface identifier", 1 },

#if defined(SOL2)
    { "ipv6cp-use-persistent", o_bool, &ipv6cp_wantoptions[0].use_persistent,
      "Use uniquely-available persistent value for link local address", 1 },
#endif /* defined(SOL2) */

    { "ipv6cp-restart", o_int, &ipv6cp_fsm[0].timeouttime,
      "Set timeout for IPv6CP", OPT_PRIO },
    { "ipv6cp-max-terminate", o_int, &ipv6cp_fsm[0].maxtermtransmits,
      "Set max #xmits for term-reqs", OPT_PRIO },
    { "ipv6cp-max-configure", o_int, &ipv6cp_fsm[0].maxconfreqtransmits,
      "Set max #xmits for conf-reqs", OPT_PRIO },
    { "ipv6cp-max-failure", o_int, &ipv6cp_fsm[0].maxnakloops,
      "Set max #conf-naks for IPv6CP", OPT_PRIO },

   { NULL }
};


/*
 * Protocol entry points from main code.
 */
static void ipv6cp_init __P((int));
static void ipv6cp_open __P((int));
static void ipv6cp_close __P((int, char *));
static void ipv6cp_lowerup __P((int));
static void ipv6cp_lowerdown __P((int));
static void ipv6cp_input __P((int, u_char *, int));
static void ipv6cp_protrej __P((int));
static int  ipv6cp_printpkt __P((u_char *, int,
			       void (*) __P((void *, char *, ...)), void *));
static void ipv6_check_options __P((void));
static int  ipv6_demand_conf __P((int));
static int  ipv6_active_pkt __P((u_char *, int));

struct protent ipv6cp_protent = {
    PPP_IPV6CP,
    ipv6cp_init,
    ipv6cp_input,
    ipv6cp_protrej,
    ipv6cp_lowerup,
    ipv6cp_lowerdown,
    ipv6cp_open,
    ipv6cp_close,
    ipv6cp_printpkt,
    NULL,
#ifdef SUPPORT_ATP_PPPC6
    1,
#else
    0,
#endif
    "IPV6CP",
    "IPV6",
    ipv6cp_option_list,
    ipv6_check_options,
    ipv6_demand_conf,
    ipv6_active_pkt
};

static void ipv6cp_clear_addrs __P((int, eui64_t, eui64_t));
static void ipv6cp_script __P((char *));
static void ipv6cp_script_done __P((void *));

/*
 * Lengths of configuration options.
 */
#define CILEN_VOID	2
#define CILEN_COMPRESS	4	/* length for RFC2023 compress opt. */
#define CILEN_IFACEID   10	/* RFC2472, interface identifier    */

#define CODENAME(x)	((x) == CONFACK ? "ACK" : \
			 (x) == CONFNAK ? "NAK" : "REJ")

/*
 * This state variable is used to ensure that we don't
 * run an ipcp-up/down script while one is already running.
 */
static enum script_state {
    s_down,
    s_up,
} ipv6cp_script_state;
static pid_t ipv6cp_script_pid;

/*
 * setifaceid - set the interface identifiers manually
 */
static int
setifaceid(argv)
    char **argv;
{
    char *comma, *arg, c;
    ipv6cp_options *wo = &ipv6cp_wantoptions[0];
    struct in6_addr addr;
    static int prio_local, prio_remote;

#define VALIDID(a) ( (((a).s6_addr32[0] == 0) && ((a).s6_addr32[1] == 0)) && \
			(((a).s6_addr32[2] != 0) || ((a).s6_addr32[3] != 0)) )
    
    arg = *argv;
    if ((comma = strchr(arg, ',')) == NULL)
	comma = arg + strlen(arg);
    
    /* 
     * If comma first character, then no local identifier
     */
    if (comma != arg) {
	c = *comma;
	*comma = '\0';

	if (inet_pton(AF_INET6, arg, &addr) == 0 || !VALIDID(addr)) {
	    option_error("Illegal interface identifier (local): %s", arg);
	    return 0;
	}

	if (option_priority >= prio_local) {
	    eui64_copy(addr.s6_addr32[2], wo->ourid);
	    wo->opt_local = 1;
	    prio_local = option_priority;
	}
	*comma = c;
    }
    
    /*
     * If comma last character, the no remote identifier
     */
    if (*comma != 0 && *++comma != '\0') {
	if (inet_pton(AF_INET6, comma, &addr) == 0 || !VALIDID(addr)) {
	    option_error("Illegal interface identifier (remote): %s", comma);
	    return 0;
	}
	if (option_priority >= prio_remote) {
	    eui64_copy(addr.s6_addr32[2], wo->hisid);
	    wo->opt_remote = 1;
	    prio_remote = option_priority;
	}
    }

    if (override_value("+ipv6", option_priority, option_source))
	ipv6cp_protent.enabled_flag = 1;
    return 1;
}


/*
 * Make a string representation of a network address.
 */
char *
llv6_ntoa(ifaceid)
    eui64_t ifaceid;
{
    static char b[64];

    /*Start of ATP 2011-5-4 8:46 for  by h00163136*/
#if 0
    sprintf(b, "fe80::%s", eui64_ntoa(ifaceid));
#else
    snprintf(b, sizeof(b), "fe80::%s", eui64_ntoa(ifaceid));
#endif
    /*End of ATP 2011-5-4 8:46 by h00163136*/
    return b;
}

static void
printifaceid(opt, printer, arg)
    option_t *opt;
    void (*printer) __P((void *, char *, ...));
    void *arg;
{
	ipv6cp_options *wo = &ipv6cp_wantoptions[0];

	if (wo->opt_local)
		printer(arg, "%s", llv6_ntoa(wo->ourid));
	printer(arg, ",");
	if (wo->opt_remote)
		printer(arg, "%s", llv6_ntoa(wo->hisid));
}


/*
 * ipv6cp_init - Initialize IPV6CP.
 */
static void
ipv6cp_init(unit)
    int unit;
{
    fsm *f = &ipv6cp_fsm[unit];
    ipv6cp_options *wo = &ipv6cp_wantoptions[unit];
    ipv6cp_options *ao = &ipv6cp_allowoptions[unit];

    f->unit = unit;
    f->protocol = PPP_IPV6CP;
    f->callbacks = &ipv6cp_callbacks;
    fsm_init(&ipv6cp_fsm[unit]);

    memset(wo, 0, sizeof(*wo));
    memset(ao, 0, sizeof(*ao));

    wo->accept_local = 1;
    wo->neg_ifaceid = 1;
    ao->neg_ifaceid = 1;

#ifdef IPV6CP_COMP
    wo->neg_vj = 1;
    ao->neg_vj = 1;
    wo->vj_protocol = IPV6CP_COMP;
#endif

    ouroldid.e32[0] = 0;
    ouroldid.e32[1] = 0;
    hisoldid.e32[0] = 0;
    hisoldid.e32[1] = 0;
}


/*
 * ipv6cp_open - IPV6CP is allowed to come up.
 */
static void
ipv6cp_open(unit)
    int unit;
{
#if defined(SUPPORT_ATP_PPP_DYN_DIAL) && defined(SUPPORT_ATP_PPP_DT_TOCPE0056)
    if (g_iEnableV6Dial)
#endif
    {
        pppoe_setstatus6(Connecting);   //设置状态为已连接
        fsm_open(&ipv6cp_fsm[unit]);
    }
}


/*
 * ipv6cp_close - Take IPV6CP down.
 */
static void
ipv6cp_close(unit, reason)
    int unit;
    char *reason;
{
    pppoe_setlasterror("ERROR_IP_CONFIGURATION");
    fsm_close(&ipv6cp_fsm[unit], reason);
}


/*
 * ipv6cp_lowerup - The lower layer is up.
 */
static void
ipv6cp_lowerup(unit)
    int unit;
{
    fsm_lowerup(&ipv6cp_fsm[unit]);
}


/*
 * ipv6cp_lowerdown - The lower layer is down.
 */
static void
ipv6cp_lowerdown(unit)
    int unit;
{
    fsm_lowerdown(&ipv6cp_fsm[unit]);
}


/*
 * ipv6cp_input - Input IPV6CP packet.
 */
static void
ipv6cp_input(unit, p, len)
    int unit;
    u_char *p;
    int len;
{
    g_iRecvIpv6cp = 1;//标记收到过ipv6cp的报文
#if defined(SUPPORT_ATP_PPP_DYN_DIAL) && defined(SUPPORT_ATP_PPP_DT_TOCPE0056)
    if (g_iEnableV6Dial)
#endif
    fsm_input(&ipv6cp_fsm[unit], p, len);
}


/*
 * ipv6cp_protrej - A Protocol-Reject was received for IPV6CP.
 *
 * Pretend the lower layer went down, so we shut up.
 */
static void
ipv6cp_protrej(unit)
    int unit;
{
#ifdef SUPPORT_ATP_FON
    FILE *fp = NULL;
#endif

	/*根据一线和DT讨论结果,hybrid HOMEGW-17091, reject时记录P004*/
	ATP_LOG_LogItemPrint(ATP_SYS_LOG_PPP_LOG_FAIL_WITHOUT_INFO);
    fsm_lowerdown(&ipv6cp_fsm[unit]);
    num_np_open--;
#ifdef SUPPORT_ATP_FON
    /*fon 心跳读到就不用去sleep()等ipv6 up,因为已明确拒绝*/
    fp = fopen(PPP_NO_IPV6_FOR_FON, "w+");
    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
#endif
}


/*
 * ipv6cp_resetci - Reset our CI.
 */
static void
ipv6cp_resetci(f)
    fsm *f;
{
    ipv6cp_options *wo = &ipv6cp_wantoptions[f->unit];
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];

    wo->req_ifaceid = wo->neg_ifaceid && ipv6cp_allowoptions[f->unit].neg_ifaceid;
    
    if (!wo->opt_local) {
#if 0
	eui64_magic_nz(wo->ourid);
#else
    memcpy(wo->ourid.e8, eui64_ifc_id.e8, 8); 
#endif
    }
    
    *go = *wo;
    eui64_zero(go->hisid);	/* last proposed interface identifier */
}


/*
 * ipv6cp_cilen - Return length of our CI.
 */
static int
ipv6cp_cilen(f)
    fsm *f;
{
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];

#define LENCIVJ(neg)		(neg ? CILEN_COMPRESS : 0)
#define LENCIIFACEID(neg)	(neg ? CILEN_IFACEID : 0)

    return (LENCIIFACEID(go->neg_ifaceid) +
	    LENCIVJ(go->neg_vj));
}


/*
 * ipv6cp_addci - Add our desired CIs to a packet.
 */
static void
ipv6cp_addci(f, ucp, lenp)
    fsm *f;
    u_char *ucp;
    int *lenp;
{
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    int len = *lenp;

#define ADDCIVJ(opt, neg, val) \
    if (neg) { \
	int vjlen = CILEN_COMPRESS; \
	if (len >= vjlen) { \
	    PUTCHAR(opt, ucp); \
	    PUTCHAR(vjlen, ucp); \
	    PUTSHORT(val, ucp); \
	    len -= vjlen; \
	} else \
	    neg = 0; \
    }

#define ADDCIIFACEID(opt, neg, val1) \
    if (neg) { \
	int idlen = CILEN_IFACEID; \
	if (len >= idlen) { \
	    PUTCHAR(opt, ucp); \
	    PUTCHAR(idlen, ucp); \
	    eui64_put(val1, ucp); \
	    len -= idlen; \
	} else \
	    neg = 0; \
    }

    ADDCIIFACEID(CI_IFACEID, go->neg_ifaceid, go->ourid);

    ADDCIVJ(CI_COMPRESSTYPE, go->neg_vj, go->vj_protocol);

    *lenp -= len;
}


/*
 * ipv6cp_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 */
static int
ipv6cp_ackci(f, p, len)
    fsm *f;
    u_char *p;
    int len;
{
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    u_short cilen, citype, cishort;
    eui64_t ifaceid;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

#define ACKCIVJ(opt, neg, val) \
    if (neg) { \
	int vjlen = CILEN_COMPRESS; \
	if ((len -= vjlen) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != vjlen || \
	    citype != opt)  \
	    goto bad; \
	GETSHORT(cishort, p); \
	if (cishort != val) \
	    goto bad; \
    }

#define ACKCIIFACEID(opt, neg, val1) \
    if (neg) { \
	int idlen = CILEN_IFACEID; \
	if ((len -= idlen) < 0) \
	    goto bad; \
	GETCHAR(citype, p); \
	GETCHAR(cilen, p); \
	if (cilen != idlen || \
	    citype != opt) \
	    goto bad; \
	eui64_get(ifaceid, p); \
	if (! eui64_equals(val1, ifaceid)) \
	    goto bad; \
    }

    ACKCIIFACEID(CI_IFACEID, go->neg_ifaceid, go->ourid);

    ACKCIVJ(CI_COMPRESSTYPE, go->neg_vj, go->vj_protocol);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    return (1);

bad:
    IPV6CPDEBUG(("ipv6cp_ackci: received bad Ack!"));
    return (0);
}

/*
 * ipv6cp_nakci - Peer has sent a NAK for some of our CIs.
 * This should not modify any state if the Nak is bad
 * or if IPV6CP is in the OPENED state.
 *
 * Returns:
 *	0 - Nak was bad.
 *	1 - Nak was good.
 */
static int
ipv6cp_nakci(f, p, len)
    fsm *f;
    u_char *p;
    int len;
{
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    u_char citype, cilen, *next;
    u_short cishort;
    eui64_t ifaceid;
    ipv6cp_options no;		/* options we've seen Naks for */
    ipv6cp_options try;		/* options to request next time */

    BZERO(&no, sizeof(no));
    try = *go;

    /*start:z00182249 for UDK0000287513：P003记录条件为当网关发的ipv6cp报文被服务器reject或NAK时 0130909*/
    ATP_LOG_LogItemPrint(ATP_SYS_LOG_PPP_LOG_FAIL_WITH_INFO);
	/*end:z00182249 for UDK0000287513：P003记录条件为当网关发的ipv6cp报文被服务器reject或NAK时 0130909*/
	
    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
#define NAKCIIFACEID(opt, neg, code) \
    if (go->neg && \
	len >= (cilen = CILEN_IFACEID) && \
	p[1] == cilen && \
	p[0] == opt) { \
	len -= cilen; \
	INCPTR(2, p); \
	eui64_get(ifaceid, p); \
	no.neg = 1; \
	code \
    }

#define NAKCIVJ(opt, neg, code) \
    if (go->neg && \
	((cilen = p[1]) == CILEN_COMPRESS) && \
	len >= cilen && \
	p[0] == opt) { \
	len -= cilen; \
	INCPTR(2, p); \
	GETSHORT(cishort, p); \
	no.neg = 1; \
        code \
    }

    /*
     * Accept the peer's idea of {our,his} interface identifier, if different
     * from our idea, only if the accept_{local,remote} flag is set.
     */
    NAKCIIFACEID(CI_IFACEID, neg_ifaceid,
	      if (go->accept_local) {
		  while (eui64_iszero(ifaceid) || 
			 eui64_equals(ifaceid, go->hisid)) /* bad luck */
		      eui64_magic(ifaceid);
		  try.ourid = ifaceid;
		  IPV6CPDEBUG(("local LL address %s", llv6_ntoa(ifaceid)));
	      }
	      );

#ifdef IPV6CP_COMP
    NAKCIVJ(CI_COMPRESSTYPE, neg_vj,
	    {
		if (cishort == IPV6CP_COMP) {
		    try.vj_protocol = cishort;
		} else {
		    try.neg_vj = 0;
		}
	    }
	    );
#else
    NAKCIVJ(CI_COMPRESSTYPE, neg_vj,
	    {
		try.neg_vj = 0;
	    }
	    );
#endif

    /*
     * There may be remaining CIs, if the peer is requesting negotiation
     * on an option that we didn't include in our request packet.
     * If they want to negotiate about interface identifier, we comply.
     * If they want us to ask for compression, we refuse.
     */
    while (len > CILEN_VOID) {
	GETCHAR(citype, p);
	GETCHAR(cilen, p);
	if( (len -= cilen) < 0 )
	    goto bad;
	next = p + cilen - 2;

	switch (citype) {
	case CI_COMPRESSTYPE:
	    if (go->neg_vj || no.neg_vj ||
		(cilen != CILEN_COMPRESS))
		goto bad;
	    no.neg_vj = 1;
	    break;
	case CI_IFACEID:
	    if (go->neg_ifaceid || no.neg_ifaceid || cilen != CILEN_IFACEID)
		goto bad;
	    try.neg_ifaceid = 1;
	    eui64_get(ifaceid, p);
	    if (go->accept_local) {
		while (eui64_iszero(ifaceid) || 
		       eui64_equals(ifaceid, go->hisid)) /* bad luck */
		    eui64_magic(ifaceid);
		try.ourid = ifaceid;
	    }
	    no.neg_ifaceid = 1;
	    break;
	}
	p = next;
    }

    /* If there is still anything left, this packet is bad. */
    if (len != 0)
	goto bad;

    /*
     * OK, the Nak is good.  Now we can update state.
     */
    if (f->state != OPENED)
	*go = try;

    return 1;

bad:
    IPV6CPDEBUG(("ipv6cp_nakci: received bad Nak!"));
    return 0;
}


/*
 * ipv6cp_rejci - Reject some of our CIs.
 */
static int
ipv6cp_rejci(f, p, len)
    fsm *f;
    u_char *p;
    int len;
{
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    u_char cilen;
    u_short cishort;
    eui64_t ifaceid;
    ipv6cp_options try;		/* options to request next time */

    try = *go;
	
	/*start:z00182249 for UDK0000287513：P003记录条件为当网关发的ipv6cp报文被服务器reject或NAK时 0130909*/
	ATP_LOG_LogItemPrint(ATP_SYS_LOG_PPP_LOG_FAIL_WITH_INFO);
	/*end:z00182249 for UDK0000287513：P003记录条件为当网关发的ipv6cp报文被服务器reject或NAK时 0130909*/
    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
#define REJCIIFACEID(opt, neg, val1) \
    if (go->neg && \
	len >= (cilen = CILEN_IFACEID) && \
	p[1] == cilen && \
	p[0] == opt) { \
	len -= cilen; \
	INCPTR(2, p); \
	eui64_get(ifaceid, p); \
	/* Check rejected value. */ \
	if (! eui64_equals(ifaceid, val1)) \
	    goto bad; \
	try.neg = 0; \
    }

#define REJCIVJ(opt, neg, val) \
    if (go->neg && \
	p[1] == CILEN_COMPRESS && \
	len >= p[1] && \
	p[0] == opt) { \
	len -= p[1]; \
	INCPTR(2, p); \
	GETSHORT(cishort, p); \
	/* Check rejected value. */  \
	if (cishort != val) \
	    goto bad; \
	try.neg = 0; \
     }

    REJCIIFACEID(CI_IFACEID, neg_ifaceid, go->ourid);

    REJCIVJ(CI_COMPRESSTYPE, neg_vj, go->vj_protocol);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    /*
     * Now we can update state.
     */
    if (f->state != OPENED)
	*go = try;
    return 1;

bad:
    IPV6CPDEBUG(("ipv6cp_rejci: received bad Reject!"));
    return 0;
}


/*
 * ipv6cp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.  If reject_if_disagree is non-zero, doesn't return
 * CONFNAK; returns CONFREJ if it can't return CONFACK.
 */
static int
ipv6cp_reqci(f, inp, len, reject_if_disagree)
    fsm *f;
    u_char *inp;		/* Requested CIs */
    int *len;			/* Length of requested CIs */
    int reject_if_disagree;
{
    ipv6cp_options *wo = &ipv6cp_wantoptions[f->unit];
    ipv6cp_options *ho = &ipv6cp_hisoptions[f->unit];
    ipv6cp_options *ao = &ipv6cp_allowoptions[f->unit];
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    u_char *cip, *next;		/* Pointer to current and next CIs */
    u_short cilen, citype;	/* Parsed len, type */
    u_short cishort;		/* Parsed short value */
    eui64_t ifaceid;		/* Parsed interface identifier */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    u_char *p;			/* Pointer to next char to parse */
    u_char *ucp = inp;		/* Pointer to current output char */
    int l = *len;		/* Length left */

    /*
     * Reset all his options.
     */
    BZERO(ho, sizeof(*ho));
    
    /*
     * Process all his options.
     */
    next = inp;
    while (l) {
	orc = CONFACK;			/* Assume success */
	cip = p = next;			/* Remember begining of CI */
	if (l < 2 ||			/* Not enough data for CI header or */
	    p[1] < 2 ||			/*  CI length too small or */
	    p[1] > l) {			/*  CI length too big? */
	    IPV6CPDEBUG(("ipv6cp_reqci: bad CI length!"));
	    orc = CONFREJ;		/* Reject bad CI */
	    cilen = l;			/* Reject till end of packet */
	    l = 0;			/* Don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* Parse CI type */
	GETCHAR(cilen, p);		/* Parse CI length */
	l -= cilen;			/* Adjust remaining length */
	next += cilen;			/* Step to next CI */

	switch (citype) {		/* Check CI type */
	case CI_IFACEID:
	    IPV6CPDEBUG(("ipv6cp: received interface identifier "));

	    if (!ao->neg_ifaceid ||
		cilen != CILEN_IFACEID) {	/* Check CI length */
		orc = CONFREJ;		/* Reject CI */
		break;
	    }

	    /*
	     * If he has no interface identifier, or if we both have same 
	     * identifier then NAK it with new idea.
	     * In particular, if we don't know his identifier, but he does,
	     * then accept it.
	     */
	    eui64_get(ifaceid, p);
	    IPV6CPDEBUG(("(%s)", llv6_ntoa(ifaceid)));
	    if (eui64_iszero(ifaceid) && eui64_iszero(go->ourid)) {
		orc = CONFREJ;		/* Reject CI */
		break;
	    }
	    if (!eui64_iszero(wo->hisid) && 
		!eui64_equals(ifaceid, wo->hisid) && 
		eui64_iszero(go->hisid)) {
		    
		orc = CONFNAK;
		ifaceid = wo->hisid;
		go->hisid = ifaceid;
		DECPTR(sizeof(ifaceid), p);
		eui64_put(ifaceid, p);
	    } else
	    if (eui64_iszero(ifaceid) || eui64_equals(ifaceid, go->ourid)) {
		orc = CONFNAK;
		if (eui64_iszero(go->hisid))	/* first time, try option */
		    ifaceid = wo->hisid;
		while (eui64_iszero(ifaceid) || 
		       eui64_equals(ifaceid, go->ourid)) /* bad luck */
		    eui64_magic(ifaceid);
		go->hisid = ifaceid;
		DECPTR(sizeof(ifaceid), p);
		eui64_put(ifaceid, p);
	    }

	    ho->neg_ifaceid = 1;
	    ho->hisid = ifaceid;
	    break;

	case CI_COMPRESSTYPE:
	    IPV6CPDEBUG(("ipv6cp: received COMPRESSTYPE "));
	    if (!ao->neg_vj ||
		(cilen != CILEN_COMPRESS)) {
		orc = CONFREJ;
		break;
	    }
	    GETSHORT(cishort, p);
	    IPV6CPDEBUG(("(%d)", cishort));

#ifdef IPV6CP_COMP
	    if (!(cishort == IPV6CP_COMP)) {
		orc = CONFREJ;
		break;
	    }

	    ho->neg_vj = 1;
	    ho->vj_protocol = cishort;
	    break;
#else
	    orc = CONFREJ;
	    break;
#endif

	default:
	    orc = CONFREJ;
	    break;
	}

endswitch:
	IPV6CPDEBUG((" (%s)\n", CODENAME(orc)));

	if (orc == CONFACK &&		/* Good CI */
	    rc != CONFACK)		/*  but prior CI wasnt? */
	    continue;			/* Don't send this one */

	if (orc == CONFNAK) {		/* Nak this CI? */
	    if (reject_if_disagree)	/* Getting fed up with sending NAKs? */
		orc = CONFREJ;		/* Get tough if so */
	    else {
		if (rc == CONFREJ)	/* Rejecting prior CI? */
		    continue;		/* Don't send this one */
		if (rc == CONFACK) {	/* Ack'd all prior CIs? */
		    rc = CONFNAK;	/* Not anymore... */
		    ucp = inp;		/* Backup */
		}
	    }
	}

	if (orc == CONFREJ &&		/* Reject this CI */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* Backup */
	}

	/* Need to move CI? */
	if (ucp != cip)
	    BCOPY(cip, ucp, cilen);	/* Move it */

	/* Update output pointer */
	INCPTR(cilen, ucp);
    }

    /*
     * If we aren't rejecting this packet, and we want to negotiate
     * their identifier and they didn't send their identifier, then we
     * send a NAK with a CI_IFACEID option appended.  We assume the
     * input buffer is long enough that we can append the extra
     * option safely.
     */
    if (rc != CONFREJ && !ho->neg_ifaceid &&
	wo->req_ifaceid && !reject_if_disagree) {
	if (rc == CONFACK) {
	    rc = CONFNAK;
	    ucp = inp;				/* reset pointer */
	    wo->req_ifaceid = 0;		/* don't ask again */
	}
	PUTCHAR(CI_IFACEID, ucp);
	PUTCHAR(CILEN_IFACEID, ucp);
	eui64_put(wo->hisid, ucp);
    }

    *len = ucp - inp;			/* Compute output length */
    IPV6CPDEBUG(("ipv6cp: returning Configure-%s", CODENAME(rc)));
    return (rc);			/* Return final code */
}


/*
 * ipv6_check_options - check that any IP-related options are OK,
 * and assign appropriate defaults.
 */
static void
ipv6_check_options()
{
    ipv6cp_options *wo = &ipv6cp_wantoptions[0];

    if (!ipv6cp_protent.enabled_flag)
	return;

#if defined(SOL2)
    /*
     * Persistent link-local id is only used when user has not explicitly
     * configure/hard-code the id
     */
    if ((wo->use_persistent) && (!wo->opt_local) && (!wo->opt_remote)) {

	/* 
	 * On systems where there are no Ethernet interfaces used, there
	 * may be other ways to obtain a persistent id. Right now, it
	 * will fall back to using magic [see eui64_magic] below when
	 * an EUI-48 from MAC address can't be obtained. Other possibilities
	 * include obtaining EEPROM serial numbers, or some other unique
	 * yet persistent number. On Sparc platforms, this is possible,
	 * but too bad there's no standards yet for x86 machines.
	 */
	if (ether_to_eui64(&wo->ourid)) {
	    wo->opt_local = 1;
	}
    }
#endif

    if (!wo->opt_local) {	/* init interface identifier */
	if (wo->use_ip && eui64_iszero(wo->ourid)) {
	    eui64_setlo32(wo->ourid, ntohl(ipcp_wantoptions[0].ouraddr));
	    if (!eui64_iszero(wo->ourid))
		wo->opt_local = 1;
	}
	
	while (eui64_iszero(wo->ourid))
	    eui64_magic(wo->ourid);
    }

    if (!wo->opt_remote) {
	if (wo->use_ip && eui64_iszero(wo->hisid)) {
	    eui64_setlo32(wo->hisid, ntohl(ipcp_wantoptions[0].hisaddr));
	    if (!eui64_iszero(wo->hisid))
		wo->opt_remote = 1;
	}
    }

    if (demand && (eui64_iszero(wo->ourid) || eui64_iszero(wo->hisid))) {
	option_error("local/remote LL address required for demand-dialling\n");
	exit(1);
    }
}


/*
 * ipv6_demand_conf - configure the interface as though
 * IPV6CP were up, for use with dial-on-demand.
 */
static int
ipv6_demand_conf(u)
    int u;
{
    ipv6cp_options *wo = &ipv6cp_wantoptions[u];
    char acCmd[128] = {0};
    
    char path[128];
    char acFilePath[128] = {0};
    int lfd = 0;

#if defined(__linux__) || defined(SOL2) || (defined(SVR4) && (defined(SNI) || defined(__USLC__)))
#if defined(SOL2)
    if (!sif6up(u))
	return 0;
#else
    if (!sifup(u))
	return 0;
#endif /* defined(SOL2) */
#endif    

    wo->ourid.e32[0] = eui64_ifc_id.e32[0];
    wo->ourid.e32[1] = eui64_ifc_id.e32[1];
    /*Start of ATP 2012-12-7 for DTS2012120600028  by  : 网关回复多余的NAK导致服务器不再回应*/ 
    /*此处添加默认值会导致收到服务器第一个req后回复NAK报文,如果服务器设置
    为收到NAK后就不再响应，就会连接不上*/
    wo->hisid.e32[0] = 0;
    /*End of ATP 2012-12-7 for DTS2012120600028*/
    wo->hisid.e32[1] = 0;

    ipv6cp_clear_addrs(u, ouroldid, hisoldid);
    
    if (!sif6addr(u, wo->ourid, wo->hisid))
	//return 0;
	notice("ipv6_demand_conf sif6addr failed.");
#if !defined(__linux__) && !(defined(SVR4) && (defined(SNI) || defined(__USLC__)))
    if (!sifup(u))
	return 0;
#endif
    if (!sifnpmode(u, PPP_IPV6, NPMODE_QUEUE))
	return 0;

    notice("ipv6_demand_conf");
    notice("local  LL address %s", llv6_ntoa(wo->ourid));
    notice("remote LL address %s", llv6_ntoa(wo->hisid));    


    snprintf(acFilePath, sizeof(acFilePath), "/proc/sys/net/ipv6/conf/%s/enable", ifname);
    lfd = open(acFilePath, O_CREAT | O_WRONLY | O_TRUNC, 0755);
    write(lfd, "1", 1);
    close(lfd);
#ifdef SUPPORT_ATP_IPV6_PPP_DIAL_ON_DEMAND
    ATP_MSG_SendMsgData(ATP_CBBID_CMS_NAME, ATP_MSG_ONDEMAND_IPV6_DNS_TRIGGER, ifname, PATHLENGTH);

	snprintf(path, sizeof(path), "%s%s/%s", WAN_PATH, wanif_name, "ipaddr6");
    WriteToFile(path, llv6_ntoa(wo->ourid));
#endif

    /*Start of ATP 2011-5-4 8:46 for  by h00163136*/
#if 0
    sprintf(acCmd, "ip -6 route add default dev %s metric 4096 > /dev/null", ifname);
#else
    snprintf(acCmd, sizeof(acCmd), "ip -6 route add default dev %s metric 4096 > /dev/null", ifname);
#endif
    /*End of ATP 2011-5-4 8:46 by h00163136*/
    /*Start of ATP 2011-5-3 20:17 for  by h00163136*/
#if 0
    system(acCmd);
#else
    ATP_UTIL_ForkProcess(acCmd, -1, NULL, NULL);
#endif
    /*End of ATP 2011-5-3 20:17 by h00163136*/
    notice("cmd: %s", acCmd);

    return 1;
}

#if 0
static int SetIPv6Addr(int iSet)
{
    int iSockfd = -1;
    int iIfindex = -1;
    int iCmd = -1;
	struct in6_ifreq ireq;

    iIfindex = if_nametoindex(wanif_name);
    if (0 >= iIfindex)
    {
        notice("error: if_nametoindex error: [%s]", strerror(errno));
        return (-1);
    }

    iSockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (0 > iSockfd)
    {
        notice("error: socket error [%s]", strerror(errno));
        return (-1);
    }
    memset(&ireq, 0, sizeof(struct in6_ifreq));
    memcpy(&ireq.ifr6_addr, &g_stStaticIP6Addr.stAddr, sizeof(struct in6_addr));
    ireq.ifr6_ifindex = iIfindex;
    ireq.ifr6_prefixlen = g_stStaticIP6Addr.iPrefixLen;

    if (iSet)
    {
        iCmd = SIOCSIFADDR;
    }
    else
    {
        iCmd = SIOCDIFADDR;
    }

    if (0 > ioctl(iSockfd, iCmd, &ireq))
    {
        notice("ioctl error: set ip addr error: %s", strerror(errno));
        close(iSockfd);
    }

    close(iSockfd);
}
#endif

ipv6cp_down_sig(int unit)
{
    
    fsm *f = &ipv6cp_fsm[unit];

    //UNTIMEOUT(ipv6cp_check_ra_prefix,f);
    //UNTIMEOUT(ipv6cp_check_pd_prefix,f);
    
    if (ipv6cp_is_up)
    {
        //ipv6cp_dhcp_pid = 0;
        ipv6cp_close(f->unit,"ipv6cp_close");
        sleep(4);

        ipv6cp_fsm[f->unit].flags = OPT_RESTART;
        ipv6cp_open(f->unit);

    }

}

static int start_dhcp6c(void)
{
    char *ppargv[12];
    int i = 0;
    int rtn = 0;

    ATP_MSG_HEADER_ST       *pstMsg;
    ATP_MSG_HEADER_ST       *pstResp = NULL;
    char    acBuff[sizeof(ATP_MSG_HEADER_ST) + sizeof(struct stDhcp6cData)] = {0};

    FILE *pidfp;
    char acPidFile[ATP_BUFFER_32] = {0};
    int  lCurrPid = 0; 
    char acCmd[ATP_BUFFER_128] = {0};    
    unsigned int  ulRet = 0; 
    
    struct stDhcp6cData stDhcp6cMsgData;
    memset(&stDhcp6cMsgData,0, sizeof(stDhcp6cMsgData));

    pstMsg = (ATP_MSG_HEADER_ST *)acBuff;

    ppargv[i++] = "f";
    ppargv[i++] = "F";
    /* BEGIN: Added by z67728, 2010/2/2 add dhcp6c decline function .*/    
    ppargv[i++] = "Z";
    /*Start of Protocol 2014-3-18 20:0 for DTS2014030403587 路由机制修改 by t00189036*/
    if (!demand)
    {
        ppargv[i++] = "G";
    }
    /*End of Protocol 2014-3-18 20:0 for DTS2014030403587 路由机制修改 by t00189036*/
    /* END:   Added by z67728, 2010/2/2 */
    //ppargv[i++] = "-D";
    
    
        if (ATP_PPPC_IPV6_ADDR_TYPE_SLAAC == g_iIpv6Mode)
        {
            ppargv[i++] = "R";
            ppargv[i++] = "A";
            
            if (NULL == strstr(service_list, "INTERNET"))
            {
                ppargv[i++] = "G";
            }
/*BAS还不支持, 先不加option*/
#ifdef SUPPORT_ATP_DSLITE_TUNNEL
            /*请求DS-Lite option*/
            if (1)
            {
                ppargv[i++] = "S";
            }
#endif
            if (ATP_PPPC_IPV6_ADDR_OPTION_BOTH == g_iIpv6MAddr)
            {
                ppargv[i++] = "N";
                ppargv[i++] = "P";
            }
            else if (ATP_PPPC_IPV6_ADDR_OPTION_IANA == g_iIpv6MAddr)
            {
                ppargv[i++] = "N";
            }
            else if (ATP_PPPC_IPV6_ADDR_OPTION_IAPD == g_iIpv6MAddr)
            {
                ppargv[i++] = "P";
            }
            /* 暂不支持 */
            else if (ATP_PPPC_IPV6_ADDR_OPTION_AUTO == g_iIpv6MAddr)
            {
                return (-1);
            }
            else
            {
                return (-1);
            }
            
            //ppargv[i++] = wanif_name;
            ppargv[i] = NULL;
        }
        else if (ATP_PPPC_IPV6_ADDR_TYPE_DHCP == g_iIpv6Mode)
        {
            if (NULL == strstr(service_list, "INTERNET"))
            {
                ppargv[i++] = "G";
            }
#ifdef SUPPORT_ATP_DSLITE_TUNNEL
            /*请求DS-Lite option*/
            if (1)
            {
                ppargv[i++] = "S";
            }
#endif                    
            if (ATP_PPPC_IPV6_ADDR_OPTION_BOTH == g_iIpv6MAddr)
            {
                ppargv[i++] = "N";
                ppargv[i++] = "P";
            }
            else if (ATP_PPPC_IPV6_ADDR_OPTION_IANA == g_iIpv6MAddr)
            {
                ppargv[i++] = "N";
            }
            else if (ATP_PPPC_IPV6_ADDR_OPTION_IAPD == g_iIpv6MAddr)
            {
                ppargv[i++] = "P";
            }
            /* 暂不支持 */
            else if (ATP_PPPC_IPV6_ADDR_OPTION_AUTO == g_iIpv6MAddr)
            {
                return (-1);
            }
            else
            {
                return (-1);
            }
            
            //ppargv[i++] = wanif_name;
            ppargv[i] = NULL;
        }

        snprintf(stDhcp6cMsgData.acInterface, sizeof(stDhcp6cMsgData.acInterface), "%s", wanif_name);
        
        i = 0;
        while(NULL != ppargv[i])
        {
            snprintf(stDhcp6cMsgData.inputoptions+strlen(stDhcp6cMsgData.inputoptions), 128, "%s", ppargv[i]);
            i++;
        }
        snprintf(pstMsg->aucRecvApp,(ATP_MSG_MAX_NAME_LEN + 1), "%s",ATP_CBBID_DHCP6C_NAME);  
        pstMsg->ulDataLen = (VOS_UINT32)(sizeof(struct stDhcp6cData));
        memcpy((struct stDhcp6cData *)(pstMsg + 1), &stDhcp6cMsgData, sizeof(struct stDhcp6cData));       

#if 0
	rtn = ATP_MSG_SendMsgData(ATP_CBBID_CMS_NAME, ATP_MSG_START_DHCP6C_PRO, &stDhcp6cMsgData, sizeof(struct stDhcp6cData));
        if ( rtn != VOS_OK )
        {
            printf(" WanSendMsg2Dhcp6c() send start msg failure %x\n", rtn);
            return (-1);
        }
#endif

        if ((lCurrPid = GPL_UTIL_GetPid(ATP_CBBID_DHCP6C_NAME)) <= 0)
        {
            snprintf(acCmd, sizeof(acCmd), "%s %s &", ATP_CBBID_DHCP6C_NAME, ATP_CBBID_PPPC_NAME);
            lCurrPid = ATP_UTIL_ForkProcess(acCmd, 0, NULL, NULL);
            printf("pppc start dhcp6c m_lDhcp6cPid %d\n", lCurrPid);    

            //保证dhcp6c进程在收到消息之前启动
            pstMsg->ulMsgType = ATP_MSG_DHCP6C_INITOVER;
            rtn = ATP_MSG_SendAndRecv(pstMsg, &pstResp, 2 * 1000);
            if (0 != rtn)
            {
                printf(" pppc start dhcp6c Recv msg failure ==%x\n", rtn);
                return rtn;
            }
            if (NULL != pstResp)
            {
                lCurrPid = * (int *)(pstResp + 1);
                free((VOS_VOID *)pstResp);
            }
        }
        g_iDhcp6cPid = lCurrPid;

        rtn = ATP_MSG_SendMsgData(ATP_CBBID_DHCP6C_NAME, ATP_MSG_DHCP6C_START, &stDhcp6cMsgData, sizeof(struct stDhcp6cData));
        if ( rtn != VOS_OK )
        {
            printf(" WanSendMsg2Dhcp6c() send start msg failure %x\n", rtn);
            return (-1);
        }   
               
        return (0);
}


int stop_dhcp6c(void)
{
    ATP_MSG_HEADER_ST       *pstMsg;
    ATP_MSG_HEADER_ST       *pstResp = NULL; 
    char acMsg[sizeof(ATP_MSG_HEADER_ST) + 256];
    char acCmd[ATP_BUFFER_128] = {0};    

    int rtn = 0;

    int  lDhcp6cPid = g_iDhcp6cPid;
        
    /* 如果是静态IP */
    if ((ATP_PPPC_IPV6_ADDR_TYPE_SLAAC != g_iIpv6Mode)
        && (ATP_PPPC_IPV6_ADDR_TYPE_DHCP != g_iIpv6Mode))
    {
        return (0);
    }

#ifdef SUPPORT_ATP_DHCP6C
    if ( 0 >= lDhcp6cPid )
    {
        return (0);
    }           
    /*先让dhcp6c退出，防止pppc先退出对dhcp6c退出造成影响*/
    else
    {
        pstMsg = (ATP_MSG_HEADER_ST *)acMsg;

        snprintf(pstMsg->aucRecvApp,(ATP_MSG_MAX_NAME_LEN + 1), "%s",ATP_CBBID_DHCP6C_NAME);  
        pstMsg->ulDataLen = (VOS_UINT32)(strlen(wanif_name) + 1);
        snprintf((VOS_CHAR *)(pstMsg + 1), 256, "%s", wanif_name);
        pstMsg->ulMsgType = ATP_MSG_DHCP6C_STOP;

        rtn = ATP_MSG_SendAndRecv(pstMsg, &pstResp, 5 * 1000);       
        if (0 != rtn)
        {
	        //如果等待超时，直接kill dhcp6c进程，不再等待dhcp6c自己退出
            //kill(lDhcp6cPid, SIGKILL);
            PPPC6SendStautsChangeMsg((VOS_UINT32)Disconnected);
            g_iDhcp6cPid = -1;
            return rtn;
        }
        PPPC6SendStautsChangeMsg((VOS_UINT32)Disconnected); 
        if (NULL != pstResp)
        {
            free((VOS_VOID *)pstResp);
        }
        g_iDhcp6cPid = -1;
    }

#endif
       
    return (0);
}

#define IPV6_START_DHCP6C_TIMEOUT  200000
/*
 * ipv6cp_up - IPV6CP has come UP.
 *
 * Configure the IPv6 network interface appropriately and bring it up.
 */
static void
ipv6cp_up(f)
    fsm *f;
{
    char acFilePath[128] = {0};
    int lfd = 0;
	
    ipv6cp_options *ho = &ipv6cp_hisoptions[f->unit];
    ipv6cp_options *go = &ipv6cp_gotoptions[f->unit];
    ipv6cp_options *wo = &ipv6cp_wantoptions[f->unit];

    IPV6CPDEBUG(("ipv6cp: up"));

    /*
     * We must have a non-zero LL address for both ends of the link.
     */
    if (!ho->neg_ifaceid)
	ho->hisid = wo->hisid;

    if(!no_ifaceid_neg) {
	if (eui64_iszero(ho->hisid)) {
	    error("Could not determine remote LL address");
	    ipv6cp_close(f->unit, "Could not determine remote LL address");
	    return;
	}
	if (eui64_iszero(go->ourid)) {
	    error("Could not determine local LL address");
	    ipv6cp_close(f->unit, "Could not determine local LL address");
	    return;
	}
	if (eui64_equals(go->ourid, ho->hisid)) {
	    error("local and remote LL addresses are equal");
	    ipv6cp_close(f->unit, "local and remote LL addresses are equal");
	    return;
	}
    }
    script_setenv("LLLOCAL", llv6_ntoa(go->ourid), 0);
    script_setenv("LLREMOTE", llv6_ntoa(ho->hisid), 0);

#ifdef IPV6CP_COMP
    /* set tcp compression */
    sif6comp(f->unit, ho->neg_vj);
#endif

    /*
     * If we are doing dial-on-demand, the interface is already
     * configured, so we put out any saved-up packets, then set the
     * interface to pass IPv6 packets.
     */
    if (demand) {
        #if 0
	if (! eui64_equals(go->ourid, wo->ourid) || 
	    ! eui64_equals(ho->hisid, wo->hisid)) {
	    #else
        if(1){
        #endif
	    if (! eui64_equals(go->ourid, wo->ourid))
		warn("Local LL address changed to %s", 
		     llv6_ntoa(go->ourid));
	    if (! eui64_equals(ho->hisid, wo->hisid))
		warn("Remote LL address changed to %s", 
		     llv6_ntoa(ho->hisid));
	    //ipv6cp_clear_addrs(f->unit, go->ourid, ho->hisid);
        ipv6cp_clear_addrs(f->unit, ouroldid, hisoldid);

	    /* Set the interface to the new addresses */
	    if (!sif6addr(f->unit, go->ourid, ho->hisid)) {
		if (debug)
		    warn("sif6addr failed");
		ipv6cp_close(f->unit, "Interface configuration failed");
		return;
	    }

	}
	demand_rexmit(PPP_IPV6);
	sifnpmode(f->unit, PPP_IPV6, NPMODE_PASS);

    } else {
	/*
	 * Set LL addresses
	 */
#if !defined(__linux__) && !defined(SOL2) && !(defined(SVR4) && (defined(SNI) || defined(__USLC__)))
	if (!sif6addr(f->unit, go->ourid, ho->hisid)) {
	    if (debug)
		warn("sif6addr failed");
	    ipv6cp_close(f->unit, "Interface configuration failed");
	    return;
	}
#endif

	/* bring the interface up for IPv6 */
#if defined(SOL2)
	if (!sif6up(f->unit)) {
	    if (debug)
		warn("sifup failed (IPV6)");
	    ipv6cp_close(f->unit, "Interface configuration failed");
	    return;
	}
#else
	if (!sifup(f->unit)) {
	    if (debug)
		warn("sifup failed (IPV6)");
	    ipv6cp_close(f->unit, "Interface configuration failed");
	    return;
	}
#endif /* defined(SOL2) */

#if defined(__linux__) || defined(SOL2) || (defined(SVR4) && (defined(SNI) || defined(__USLC__)))
	if (!sif6addr(f->unit, go->ourid, ho->hisid)) {
	    if (debug)
		warn("sif6addr failed");
	    ipv6cp_close(f->unit, "Interface configuration failed");
	    return;
	}
#endif
	sifnpmode(f->unit, PPP_IPV6, NPMODE_PASS);

	notice("local  LL address %s", llv6_ntoa(go->ourid));
	notice("remote LL address %s", llv6_ntoa(ho->hisid));
    }

    np_up(f->unit, PPP_IPV6);
    ipv6cp_is_up = 1;
    /*ipv6拨号时，也把tcpmss记下*/
    set_tcp_mss();

    /*记录ipv6cp的localid 和 remoteid*/
    snprintf(acFilePath, sizeof(acFilePath), "/var/wan/%s/ipv6id_local", wanif_name);
    WriteToFile(acFilePath, eui64_ntoa(go->ourid));
    snprintf(acFilePath, sizeof(acFilePath), "/var/wan/%s/ipv6id_remote", wanif_name);
    WriteToFile(acFilePath, eui64_ntoa(ho->hisid));
    
    /*为解决DTS2010061200425新建IPv4的WAN时也发IPv6的报文,每个wan接口proc下面的ipv6 enable 被置成了0, 
    这里必须置成1, 否则ppp的WAN口不能收发IPv6报文*/
    /*Start of ATP 2011-5-4 8:47 for  by h00163136*/
#if 0
    sprintf(acFilePath, "/proc/sys/net/ipv6/conf/%s/enable", wanif_name);
#else
    snprintf(acFilePath, sizeof(acFilePath), "/proc/sys/net/ipv6/conf/%s/enable", wanif_name);
#endif
    /*End of ATP 2011-5-4 8:47 by h00163136*/
    lfd = open(acFilePath, O_CREAT | O_WRONLY | O_TRUNC, 0755);
    write(lfd, "1", 1);
    close(lfd);

    /* 如果配置为静态IP的话，那么就UP了 */
    if (ATP_PPPC_IPV6_ADDR_TYPE_STATIC == g_iIpv6Mode)
    {
        //SetIPv6Addr(1);
        pppoe_setstatus6(Connected);   //设置状态为已连接
    }
    else    /* 否则，根据DHCP或者RA来决定 */
    {
        //start_dhcp6c();
        /*用短暂超时代替直接调用，保证ipv6cp中sendandrecv 流程不阻塞ipcp流程，
        从而使ipv4 up事件首先上报*/
        TIMEOUT_USEC(start_dhcp6c, NULL, IPV6_START_DHCP6C_TIMEOUT);
    }


    /*
     * Execute the ipv6-up script, like this:
     *	/etc/ppp/ipv6-up interface tty speed local-LL remote-LL
     */
    if (ipv6cp_script_state == s_down && ipv6cp_script_pid == 0) {
	ipv6cp_script_state = s_up;
	ipv6cp_script(_PATH_IPV6UP);
    }
#ifdef SUPPORT_ATP_PPP_DYN_DIAL
    //如果v4没up, 加默认路由，使能触发拨号
    /*Start of Protocol 2014-3-18 20:1 for DTS2014030403587 路由机制修改 by t00189036*/
    if (demand && (0 == get_ipcp_up()))
    /*End of Protocol 2014-3-18 20:1 for DTS2014030403587 路由机制修改 by t00189036*/
    {
        snprintf(acFilePath, sizeof(acFilePath), "route del default");//避免有多余的默认路由
        ATP_UTIL_ForkProcess(acFilePath, -1, NULL, NULL);
        snprintf(acFilePath, sizeof(acFilePath), "route add default dev %s", W724V_PPP_NAME);
        ATP_UTIL_ForkProcess(acFilePath, -1, NULL, NULL);
    }
#endif
}


/*
 * ipv6cp_down - IPV6CP has gone DOWN.
 *
 * Take the IPv6 network interface down, clear its addresses
 * and delete routes through it.
 */
static void
ipv6cp_down(f)
    fsm *f;
{
    IPV6CPDEBUG(("ipv6cp: down"));
    update_link_stats(f->unit);
    if (ipv6cp_is_up) {
    	ipv6cp_is_up = 0;

        if (ATP_PPPC_IPV6_ADDR_TYPE_STATIC == g_iIpv6Mode)
        {
            //SetIPv6Addr(0);            
            pppoe_setstatus6(Disconnected);
        }
        else
        {
            stop_dhcp6c();
            ipv6cp_dhcp_pid = 0;            
        }
        
        
    	np_down(f->unit, PPP_IPV6);
    }
#ifdef IPV6CP_COMP
    sif6comp(f->unit, 0);
#endif

    /*
     * If we are doing dial-on-demand, set the interface
     * to queue up outgoing packets (for now).
     */
    if (demand) {
	sifnpmode(f->unit, PPP_IPV6, NPMODE_QUEUE);
    } else {
	sifnpmode(f->unit, PPP_IPV6, NPMODE_DROP);
#if !defined(__linux__) && !(defined(SVR4) && (defined(SNI) || defined(__USLC)))
#if defined(SOL2)
	sif6down(f->unit);
#else
	sifdown(f->unit);
#endif /* defined(SOL2) */
#endif
	ipv6cp_clear_addrs(f->unit, 
			   ipv6cp_gotoptions[f->unit].ourid,
			   ipv6cp_hisoptions[f->unit].hisid);
#if defined(__linux__) || (defined(SVR4) && (defined(SNI) || defined(__USLC)))
	sifdown(f->unit);
#endif
    }

    /* Execute the ipv6-down script */
    if (ipv6cp_script_state == s_up && ipv6cp_script_pid == 0) {
	ipv6cp_script_state = s_down;
	ipv6cp_script(_PATH_IPV6DOWN);
    }
}


/*
 * ipv6cp_clear_addrs() - clear the interface addresses, routes,
 * proxy neighbour discovery entries, etc.
 */
static void
ipv6cp_clear_addrs(unit, ourid, hisid)
    int unit;
    eui64_t ourid;
    eui64_t hisid;
{
    cif6addr(unit, ourid, hisid);
}


/*
 * ipv6cp_finished - possibly shut down the lower layers.
 */
static void
ipv6cp_finished(f)
    fsm *f;
{
    np_finished(f->unit, PPP_IPV6);
}


/*
 * ipv6cp_script_done - called when the ipv6-up or ipv6-down script
 * has finished.
 */
static void
ipv6cp_script_done(arg)
    void *arg;
{
    ipv6cp_script_pid = 0;
    switch (ipv6cp_script_state) {
    case s_up:
	if (ipv6cp_fsm[0].state != OPENED) {
	    ipv6cp_script_state = s_down;
	    ipv6cp_script(_PATH_IPV6DOWN);
	}
	break;
    case s_down:
	if (ipv6cp_fsm[0].state == OPENED) {
	    ipv6cp_script_state = s_up;
	    ipv6cp_script(_PATH_IPV6UP);
	}
	break;
    }
}


/*
 * ipv6cp_script - Execute a script with arguments
 * interface-name tty-name speed local-LL remote-LL.
 */
static void
ipv6cp_script(script)
    char *script;
{
    char strspeed[32], strlocal[32], strremote[32];
    char *argv[8];

    /*Start of ATP 2011-5-3 12:10 for  by h00163136*/
#if 0
    sprintf(strspeed, "%d", baud_rate);
    strcpy(strlocal, llv6_ntoa(ipv6cp_gotoptions[0].ourid));
    strcpy(strremote, llv6_ntoa(ipv6cp_hisoptions[0].hisid));
#else
    snprintf(strspeed, sizeof(strspeed), "%d", baud_rate);
    snprintf(strlocal, sizeof(strlocal),"%s", llv6_ntoa(ipv6cp_gotoptions[0].ourid));
    snprintf(strremote, sizeof(strremote), "%s", llv6_ntoa(ipv6cp_hisoptions[0].hisid));
#endif
    /*End of ATP 2011-5-3 12:10 by h00163136*/

    argv[0] = script;
    argv[1] = ifname;
    argv[2] = devnam;
    argv[3] = strspeed;
    argv[4] = strlocal;
    argv[5] = strremote;
    argv[6] = ipparam;
    argv[7] = NULL;

    ipv6cp_script_pid = run_program(script, argv, 0, ipv6cp_script_done, NULL);
}

/*
 * ipv6cp_printpkt - print the contents of an IPV6CP packet.
 */
static char *ipv6cp_codenames[] = {
    "ConfReq", "ConfAck", "ConfNak", "ConfRej",
    "TermReq", "TermAck", "CodeRej"
};

static int
ipv6cp_printpkt(p, plen, printer, arg)
    u_char *p;
    int plen;
    void (*printer) __P((void *, char *, ...));
    void *arg;
{
    int code, id, len, olen;
    u_char *pstart, *optend;
    u_short cishort;
    eui64_t ifaceid;

    if (plen < HEADERLEN)
	return 0;
    pstart = p;
    GETCHAR(code, p);
    GETCHAR(id, p);
    GETSHORT(len, p);
    if (len < HEADERLEN || len > plen)
	return 0;

    if (code >= 1 && code <= sizeof(ipv6cp_codenames) / sizeof(char *))
	printer(arg, " %s", ipv6cp_codenames[code-1]);
    else
	printer(arg, " code=0x%x", code);
    printer(arg, " id=0x%x", id);
    len -= HEADERLEN;
    switch (code) {
    case CONFREQ:
    case CONFACK:
    case CONFNAK:
    case CONFREJ:
	/* print option list */
	while (len >= 2) {
	    GETCHAR(code, p);
	    GETCHAR(olen, p);
	    p -= 2;
	    if (olen < 2 || olen > len) {
		break;
	    }
	    printer(arg, " <");
	    len -= olen;
	    optend = p + olen;
	    switch (code) {
	    case CI_COMPRESSTYPE:
		if (olen >= CILEN_COMPRESS) {
		    p += 2;
		    GETSHORT(cishort, p);
		    printer(arg, "compress ");
		    printer(arg, "0x%x", cishort);
		}
		break;
	    case CI_IFACEID:
		if (olen == CILEN_IFACEID) {
		    p += 2;
		    eui64_get(ifaceid, p);
		    printer(arg, "addr %s", llv6_ntoa(ifaceid));
		}
		break;
	    }
	    while (p < optend) {
		GETCHAR(code, p);
		printer(arg, " %.2x", code);
	    }
	    printer(arg, ">");
	}
	break;

    case TERMACK:
    case TERMREQ:
	if (len > 0 && *p >= ' ' && *p < 0x7f) {
	    printer(arg, " ");
	    print_string((char *)p, len, printer, arg);
	    p += len;
	    len = 0;
	}
	break;
    }

    /* print the rest of the bytes in the packet */
    for (; len > 0; --len) {
	GETCHAR(code, p);
	printer(arg, " %.2x", code);
    }

    return p - pstart;
}

/*
 * ipv6_active_pkt - see if this IP packet is worth bringing the link up for.
 * We don't bring the link up for IP fragments or for TCP FIN packets
 * with no data.
 */
#define IP6_HDRLEN	40	/* bytes */
#define IP6_NHDR_FRAG	44	/* fragment IPv6 header */
//#define IPPROTO_TCP	6
#define TCP_HDRLEN	20
#define TH_FIN		0x01

/*
 * We use these macros because the IP header may be at an odd address,
 * and some compilers might use word loads to get th_off or ip_hl.
 */

#define get_ip6nh(x)	(((unsigned char *)(x))[6])
#define get_tcpoff(x)	(((unsigned char *)(x))[12] >> 4)
#define get_tcpflags(x)	(((unsigned char *)(x))[13])

static int
ipv6_active_pkt(pkt, len)
    u_char *pkt;
    int len;
{
    u_char *tcp;

    len -= PPP_HDRLEN;
    pkt += PPP_HDRLEN;
    if (len < IP6_HDRLEN)
	return 0;
    if (get_ip6nh(pkt) == IP6_NHDR_FRAG)
	return 0;
    if (get_ip6nh(pkt) != IPPROTO_TCP)
	return 1;
    if (len < IP6_HDRLEN + TCP_HDRLEN)
	return 0;
    tcp = pkt + IP6_HDRLEN;
    if ((get_tcpflags(tcp) & TH_FIN) != 0 && len == IP6_HDRLEN + get_tcpoff(tcp) * 4)
	return 0;
    return 1;
}
