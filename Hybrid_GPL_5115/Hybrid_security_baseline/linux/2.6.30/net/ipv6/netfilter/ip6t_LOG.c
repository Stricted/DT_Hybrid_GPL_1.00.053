/*
 * This is a module which is used for logging packets.
 */

/* (C) 2001 Jan Rekorajski <baggins@pld.org.pl>
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/spinlock.h>
#include <linux/icmpv6.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/ipv6.h>
#include <linux/netfilter.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <net/netfilter/nf_log.h>

#ifdef CONFIG_FIREWALL_LOG
#include "atplogdef.h"
#include "msg/kcmsmonitormsgtypes.h"
#endif

MODULE_AUTHOR("Jan Rekorajski <baggins@pld.org.pl>");
MODULE_DESCRIPTION("Xtables: IPv6 packet logging to syslog");
MODULE_LICENSE("GPL");

struct in_device;
#include <net/route.h>
#include <linux/netfilter_ipv6/ip6t_LOG.h>

/* Use lock to serialize, so printks don't overlap */
static DEFINE_SPINLOCK(log_lock);

/* One level of recursion won't kill us */
static void dump_packet(const struct nf_loginfo *info,
			const struct sk_buff *skb, unsigned int ip6hoff,
			int recurse)
{
	u_int8_t currenthdr;
	int fragment;
	struct ipv6hdr _ip6h;
	const struct ipv6hdr *ih;
	unsigned int ptr;
	unsigned int hdrlen = 0;
	unsigned int logflags;
#ifdef CONFIG_FIREWALL_LOG
	u_int8_t acLogBuf[1500];
	u_int8_t acTmp[256];
#endif
	
	if (info->type == NF_LOG_TYPE_LOG)
		logflags = info->u.log.logflags;
	else
		logflags = NF_LOG_MASK;

	ih = skb_header_pointer(skb, ip6hoff, sizeof(_ip6h), &_ip6h);
	if (ih == NULL) {
		printk("TRUNCATED");
		return;
	}

#ifdef CONFIG_FIREWALL_LOG
        memset(acLogBuf, 0,sizeof(acLogBuf));
#endif
        
	/* Max length: 88 "SRC=0000.0000.0000.0000.0000.0000.0000.0000 DST=0000.0000.0000.0000.0000.0000.0000.0000 " */
	printk("SRC=%pI6 DST=%pI6 ", &ih->saddr, &ih->daddr);
	
#ifdef CONFIG_FIREWALL_LOG
	snprintf(acTmp, sizeof(acTmp), "SRC=%pI6 DST=%pI6 ", &ih->saddr, &ih->daddr);
	strcat(acLogBuf, acTmp);
#endif
	
	/* Max length: 44 "LEN=65535 TC=255 HOPLIMIT=255 FLOWLBL=FFFFF " */
	printk("LEN=%Zu TC=%u HOPLIMIT=%u FLOWLBL=%u ",
	       ntohs(ih->payload_len) + sizeof(struct ipv6hdr),
	       (ntohl(*(__be32 *)ih) & 0x0ff00000) >> 20,
	       ih->hop_limit,
	       (ntohl(*(__be32 *)ih) & 0x000fffff));
#ifdef CONFIG_FIREWALL_LOG
	snprintf(acTmp, sizeof(acTmp), "LEN=%Zu TC=%u HOPLIMIT=%u FLOWLBL=%u ",
	       ntohs(ih->payload_len) + sizeof(struct ipv6hdr),
	       (ntohl(*(__be32 *)ih) & 0x0ff00000) >> 20,
	       ih->hop_limit,
	       (ntohl(*(__be32 *)ih) & 0x000fffff));
	strcat(acLogBuf, acTmp);
#endif

	fragment = 0;
	ptr = ip6hoff + sizeof(struct ipv6hdr);
	currenthdr = ih->nexthdr;
	while (currenthdr != NEXTHDR_NONE && ip6t_ext_hdr(currenthdr)) {
		struct ipv6_opt_hdr _hdr;
		const struct ipv6_opt_hdr *hp;

		hp = skb_header_pointer(skb, ptr, sizeof(_hdr), &_hdr);
		if (hp == NULL) {
			printk("TRUNCATED");
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "TRUNCATED");
			strcat(acLogBuf, acTmp);
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		}

		/* Max length: 48 "OPT (...) " */
		if (logflags & IP6T_LOG_IPOPT)
		{
			printk("OPT ( ");
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "OPT ( ");
			strcat(acLogBuf, acTmp);
#endif
		}

		switch (currenthdr) {
		case IPPROTO_FRAGMENT: {
			struct frag_hdr _fhdr;
			const struct frag_hdr *fh;

			printk("FRAG:");
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "FRAG:");
			strcat(acLogBuf, acTmp);
#endif
			
			fh = skb_header_pointer(skb, ptr, sizeof(_fhdr),
						&_fhdr);
			if (fh == NULL) {
				printk("TRUNCATED ");
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp, sizeof(acTmp), "TRUNCATED");
				strcat(acLogBuf, acTmp);
				syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
				return;
			}

			/* Max length: 6 "65535 " */
			printk("%u ", ntohs(fh->frag_off) & 0xFFF8);
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "%u ", ntohs(fh->frag_off) & 0xFFF8);
			strcat(acLogBuf, acTmp);
#endif

			/* Max length: 11 "INCOMPLETE " */
			if (fh->frag_off & htons(0x0001))
			{
				printk("INCOMPLETE ");
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp, sizeof(acTmp), "INCOMPLETE ");
				strcat(acLogBuf, acTmp);
#endif
			}
			printk("ID:%08x ", ntohl(fh->identification));
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "ID:%08x ", ntohl(fh->identification));
			strcat(acLogBuf, acTmp);
#endif

			if (ntohs(fh->frag_off) & 0xFFF8)
				fragment = 1;

			hdrlen = 8;

			break;
		}
		case IPPROTO_DSTOPTS:
		case IPPROTO_ROUTING:
		case IPPROTO_HOPOPTS:
			if (fragment) {
				if (logflags & IP6T_LOG_IPOPT)
				{
					printk(")");
#ifdef CONFIG_FIREWALL_LOG
					strcpy(acTmp, ")");
					strcat(acLogBuf, acTmp);
#endif
				}

#ifdef CONFIG_FIREWALL_LOG
				syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
				return;
			}
			hdrlen = ipv6_optlen(hp);
			break;
		/* Max Length */
		case IPPROTO_AH:
			if (logflags & IP6T_LOG_IPOPT) {
				struct ip_auth_hdr _ahdr;
				const struct ip_auth_hdr *ah;

				/* Max length: 3 "AH " */
				printk("AH ");
#ifdef CONFIG_FIREWALL_LOG
				strcpy(acTmp, "AH");
				strcat(acLogBuf, acTmp);
#endif

				if (fragment) {
					printk(")");
#ifdef CONFIG_FIREWALL_LOG
					strcpy(acTmp, ")");
					strcat(acLogBuf, acTmp);
					syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
					return;
				}

				ah = skb_header_pointer(skb, ptr, sizeof(_ahdr),
							&_ahdr);
				if (ah == NULL) {
					/*
					 * Max length: 26 "INCOMPLETE [65535
					 *  bytes] )"
					 */
					printk("INCOMPLETE [%u bytes] )",
					       skb->len - ptr);
#ifdef CONFIG_FIREWALL_LOG
					snprintf(acTmp, sizeof(acTmp), "INCOMPLETE [%u bytes] )",
					       skb->len - ptr);
					strcat(acLogBuf, acTmp);
					syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
					return;
				}

				/* Length: 15 "SPI=0xF1234567 */
				printk("SPI=0x%x ", ntohl(ah->spi));
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp, sizeof(acTmp), "SPI=0x%x ", ntohl(ah->spi));
				strcat(acLogBuf, acTmp);
#endif

			}

			hdrlen = (hp->hdrlen+2)<<2;
			break;
		case IPPROTO_ESP:
			if (logflags & IP6T_LOG_IPOPT) {
				struct ip_esp_hdr _esph;
				const struct ip_esp_hdr *eh;

				/* Max length: 4 "ESP " */
				printk("ESP ");
#ifdef CONFIG_FIREWALL_LOG
				strcpy(acTmp, "ESP");
				strcat(acLogBuf, acTmp);
#endif

				if (fragment) {
					printk(")");
#ifdef CONFIG_FIREWALL_LOG
					strcpy(acTmp, ")");
					strcat(acLogBuf, acTmp);
					syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
					return;
				}

				/*
				 * Max length: 26 "INCOMPLETE [65535 bytes] )"
				 */
				eh = skb_header_pointer(skb, ptr, sizeof(_esph),
							&_esph);
				if (eh == NULL) {
					printk("INCOMPLETE [%u bytes] )",   skb->len - ptr);
#ifdef CONFIG_FIREWALL_LOG
					snprintf(acTmp, sizeof(acTmp), "INCOMPLETE [%u bytes] )",   skb->len - ptr);
					strcat(acLogBuf, acTmp);
					syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
					return;
				}

				/* Length: 16 "SPI=0xF1234567 )" */
				printk("SPI=0x%x )", ntohl(eh->spi) );
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp, sizeof(acTmp), "SPI=0x%x )", ntohl(eh->spi));
				strcat(acLogBuf, acTmp);
#endif

			}
#ifdef CONFIG_FIREWALL_LOG
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		default:
			/* Max length: 20 "Unknown Ext Hdr 255" */
			printk("Unknown Ext Hdr %u", currenthdr);
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "Unknown Ext Hdr %u", currenthdr);
			strcat(acLogBuf, acTmp);
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		}
		if (logflags & IP6T_LOG_IPOPT)
		{
			printk(") ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, ")");
			strcat(acLogBuf, acTmp);
#endif
		}
		currenthdr = hp->nexthdr;
		ptr += hdrlen;
	}

	switch (currenthdr) {
	case IPPROTO_TCP: {
		struct tcphdr _tcph;
		const struct tcphdr *th;

		/* Max length: 10 "PROTO=TCP " */
		printk("PROTO=TCP ");
#ifdef CONFIG_FIREWALL_LOG
		strcpy(acTmp, "PROTO=TCP ");
		strcat(acLogBuf, acTmp);
#endif

		if (fragment)
			break;

		/* Max length: 25 "INCOMPLETE [65535 bytes] " */
		th = skb_header_pointer(skb, ptr, sizeof(_tcph), &_tcph);
		if (th == NULL) {
			printk("INCOMPLETE [%u bytes] ", skb->len - ptr);
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "INCOMPLETE [%u bytes] ", skb->len - ptr);
			strcat(acLogBuf, acTmp);
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		}

		/* Max length: 20 "SPT=65535 DPT=65535 " */
		printk("SPT=%u DPT=%u ",   ntohs(th->source), ntohs(th->dest));
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "SPT=%u DPT=%u ",    ntohs(th->source), ntohs(th->dest));
		strcat(acLogBuf, acTmp);
#endif
		/* Max length: 30 "SEQ=4294967295 ACK=4294967295 " */
		if (logflags & IP6T_LOG_TCPSEQ)
		{
			printk("SEQ=%u ACK=%u ", ntohl(th->seq), ntohl(th->ack_seq));
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "SEQ=%u ACK=%u ", ntohl(th->seq), ntohl(th->ack_seq));
			strcat(acLogBuf, acTmp);
#endif
		}
		/* Max length: 13 "WINDOW=65535 " */
		printk("WINDOW=%u ", ntohs(th->window));
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "WINDOW=%u ", ntohs(th->window));
		strcat(acLogBuf, acTmp);
#endif
		/* Max length: 9 "RES=0x3C " */
		printk("RES=0x%02x ", (u_int8_t)(ntohl(tcp_flag_word(th) & TCP_RESERVED_BITS) >> 22));
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "RES=0x%02x ", (u_int8_t)(ntohl(tcp_flag_word(th) & TCP_RESERVED_BITS) >> 22));
		strcat(acLogBuf, acTmp);
#endif
		/* Max length: 32 "CWR ECE URG ACK PSH RST SYN FIN " */
		if (th->cwr){
			printk("CWR ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "CWR ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->ece){
			printk("ECE ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "ECE ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->urg){
			printk("URG ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "URG ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->ack){
			printk("ACK ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "ACK ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->psh){
			printk("PSH ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "PSH ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->rst){
			printk("RST ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "RST ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->syn){
			printk("SYN ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "SYN ");
			strcat(acLogBuf,acTmp);
#endif
		}
		if (th->fin){
			printk("FIN ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "FIN  ");
			strcat(acLogBuf,acTmp);
#endif
		}
		/* Max length: 11 "URGP=65535 " */
		printk("URGP=%u ", ntohs(th->urg_ptr));
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "URGP=%u ", ntohs(th->urg_ptr));
		strcat(acLogBuf, acTmp);
#endif

		if ((logflags & IP6T_LOG_TCPOPT)
		    && th->doff * 4 > sizeof(struct tcphdr)) {
			u_int8_t _opt[60 - sizeof(struct tcphdr)];
			const u_int8_t *op;
			unsigned int i;
			unsigned int optsize = th->doff * 4
					       - sizeof(struct tcphdr);

			op = skb_header_pointer(skb,
						ptr + sizeof(struct tcphdr),
						optsize, _opt);
			if (op == NULL) {
				printk("OPT (TRUNCATED)");
#ifdef CONFIG_FIREWALL_LOG
				strcpy(acTmp, "OPT (TRUNCATED)");
				strcat(acLogBuf, acTmp);
				syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
				return;
			}

			/* Max length: 127 "OPT (" 15*4*2chars ") " */
			printk("OPT (");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "OPT (");
			strcat(acLogBuf, acTmp);
#endif
			for (i =0; i < optsize; i++)
			{
				printk("%02X", op[i]);
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp+i*2, sizeof(acTmp), "%02X", op[i]);
#endif
			}
#ifdef CONFIG_FIREWALL_LOG
			strcat(acLogBuf, acTmp);
#endif
			printk(") ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, ") ");
			strcat(acLogBuf, acTmp);
#endif
		}
		break;
	}
	case IPPROTO_UDP:
	case IPPROTO_UDPLITE: {
		struct udphdr _udph;
		const struct udphdr *uh;

		if (currenthdr == IPPROTO_UDP)
		{
			/* Max length: 10 "PROTO=UDP "     */
			printk("PROTO=UDP " );
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "PROTO=UDP ");
			strcat(acLogBuf, acTmp);
#endif
		}
		else	/* Max length: 14 "PROTO=UDPLITE " */
		{
			printk("PROTO=UDPLITE ");
#ifdef CONFIG_FIREWALL_LOG
			strcpy(acTmp, "PROTO=UDPLITE ");
			strcat(acLogBuf, acTmp);
#endif
		}

		if (fragment)
			break;

		/* Max length: 25 "INCOMPLETE [65535 bytes] " */
		uh = skb_header_pointer(skb, ptr, sizeof(_udph), &_udph);
		if (uh == NULL) {
			printk("INCOMPLETE [%u bytes] ", skb->len - ptr);
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "INCOMPLETE [%u bytes] ", skb->len - ptr);
			strcat(acLogBuf, acTmp);
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		}

		/* Max length: 20 "SPT=65535 DPT=65535 " */
		printk("SPT=%u DPT=%u LEN=%u ",  ntohs(uh->source), ntohs(uh->dest), ntohs(uh->len));
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "SPT=%u DPT=%u LEN=%u ",  ntohs(uh->source), ntohs(uh->dest), ntohs(uh->len));
		strcat(acLogBuf, acTmp);
#endif
		break;
	}
	case IPPROTO_ICMPV6: {
		struct icmp6hdr _icmp6h;
		const struct icmp6hdr *ic;

		/* Max length: 13 "PROTO=ICMPv6 " */
		printk("PROTO=ICMPv6 ");
#ifdef CONFIG_FIREWALL_LOG
		strcpy(acTmp, "PROTO=ICMPv6 ");
		strcat(acLogBuf,acTmp);
#endif

		if (fragment)
			break;

		/* Max length: 25 "INCOMPLETE [65535 bytes] " */
		ic = skb_header_pointer(skb, ptr, sizeof(_icmp6h), &_icmp6h);
		if (ic == NULL) {
			printk("INCOMPLETE [%u bytes] ", skb->len - ptr);
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "INCOMPLETE [%u bytes] ", skb->len - ptr);
			strcat(acLogBuf,acTmp);
			syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
			return;
		}

		/* Max length: 18 "TYPE=255 CODE=255 " */
		printk("TYPE=%u CODE=%u ", ic->icmp6_type, ic->icmp6_code);
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "TYPE=%u CODE=%u ", ic->icmp6_type, ic->icmp6_code);
		strcat(acLogBuf,acTmp);
#endif

		switch (ic->icmp6_type) {
		case ICMPV6_ECHO_REQUEST:
		case ICMPV6_ECHO_REPLY:
			/* Max length: 19 "ID=65535 SEQ=65535 " */
			printk("ID=%u SEQ=%u ", ntohs(ic->icmp6_identifier),	ntohs(ic->icmp6_sequence));
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "ID=%u SEQ=%u ", ntohs(ic->icmp6_identifier),	ntohs(ic->icmp6_sequence));
			strcat(acLogBuf,acTmp);
#endif
			break;
		case ICMPV6_MGM_QUERY:
		case ICMPV6_MGM_REPORT:
		case ICMPV6_MGM_REDUCTION:
			break;

		case ICMPV6_PARAMPROB:
			/* Max length: 17 "POINTER=ffffffff " */
			printk("POINTER=%08x ", ntohl(ic->icmp6_pointer));
#ifdef CONFIG_FIREWALL_LOG
			snprintf(acTmp, sizeof(acTmp), "POINTER=%08x ", ntohl(ic->icmp6_pointer));
			strcat(acLogBuf,acTmp);
#endif
			/* Fall through */
		case ICMPV6_DEST_UNREACH:
		case ICMPV6_PKT_TOOBIG:
		case ICMPV6_TIME_EXCEED:
			/* Max length: 3+maxlen */
			if (recurse) {
				printk("[");
				dump_packet(info, skb, ptr + sizeof(_icmp6h),
					    0);
				printk("] ");
			}

			/* Max length: 10 "MTU=65535 " */
			if (ic->icmp6_type == ICMPV6_PKT_TOOBIG)
			{
				printk("MTU=%u ", ntohl(ic->icmp6_mtu));
#ifdef CONFIG_FIREWALL_LOG
				snprintf(acTmp, sizeof(acTmp), "MTU=%u ", ntohl(ic->icmp6_mtu));
				strcat(acLogBuf,acTmp);
#endif
			}
		}
		break;
	}
	/* Max length: 10 "PROTO=255 " */
	default:
	{
		printk("PROTO=%u ", currenthdr);
#ifdef CONFIG_FIREWALL_LOG
		snprintf(acTmp, sizeof(acTmp), "PROTO=%u ", currenthdr);
		strcat(acLogBuf,acTmp);
#endif
	}
	}

	/* Max length: 15 "UID=4294967295 " */
	if ((logflags & IP6T_LOG_UID) && recurse && skb->sk) {
		read_lock_bh(&skb->sk->sk_callback_lock);
		if (skb->sk->sk_socket && skb->sk->sk_socket->file)
			printk("UID=%u GID=%u ",
				skb->sk->sk_socket->file->f_cred->fsuid,
				skb->sk->sk_socket->file->f_cred->fsgid);
		read_unlock_bh(&skb->sk->sk_callback_lock);
	}

#ifdef CONFIG_FIREWALL_LOG
	syswatch_sendLog(ATP_LOG_TYPE_FIREWALL, ATP_LOG_LEVEL_WARNING, 0, acLogBuf);
#endif
	
	/* Max length: 16 "MARK=0xFFFFFFFF " */
	if (!recurse && skb->mark)
		printk("MARK=0x%x ", skb->mark);
}

static struct nf_loginfo default_loginfo = {
	.type	= NF_LOG_TYPE_LOG,
	.u = {
		.log = {
			.level	  = 0,
			.logflags = NF_LOG_MASK,
		},
	},
};

static void
ip6t_log_packet(u_int8_t pf,
		unsigned int hooknum,
		const struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		const struct nf_loginfo *loginfo,
		const char *prefix)
{
	if (!loginfo)
		loginfo = &default_loginfo;

	spin_lock_bh(&log_lock);
	printk("<%d>%sIN=%s OUT=%s ", loginfo->u.log.level,
		prefix,
		in ? in->name : "",
		out ? out->name : "");
	if (in && !out) {
		unsigned int len;
		/* MAC logging for input chain only. */
		printk("MAC=");
		if (skb->dev && (len = skb->dev->hard_header_len) &&
		    skb->mac_header != skb->network_header) {
			const unsigned char *p = skb_mac_header(skb);
			int i;

			if (skb->dev->type == ARPHRD_SIT &&
			    (p -= ETH_HLEN) < skb->head)
				p = NULL;

			if (p != NULL) {
				for (i = 0; i < len; i++)
					printk("%02x%s", p[i],
					       i == len - 1 ? "" : ":");
			}
			printk(" ");

			if (skb->dev->type == ARPHRD_SIT) {
				const struct iphdr *iph =
					(struct iphdr *)skb_mac_header(skb);
				printk("TUNNEL=%pI4->%pI4 ",
				       &iph->saddr, &iph->daddr);
			}
		} else
			printk(" ");
	}

	dump_packet(loginfo, skb, skb_network_offset(skb), 1);
	printk("\n");
	spin_unlock_bh(&log_lock);
}

static unsigned int
log_tg6(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ip6t_log_info *loginfo = par->targinfo;
	struct nf_loginfo li;

	li.type = NF_LOG_TYPE_LOG;
	li.u.log.level = loginfo->level;
	li.u.log.logflags = loginfo->logflags;

	ip6t_log_packet(NFPROTO_IPV6, par->hooknum, skb, par->in, par->out,
			&li, loginfo->prefix);
	return XT_CONTINUE;
}


static bool log_tg6_check(const struct xt_tgchk_param *par)
{
	const struct ip6t_log_info *loginfo = par->targinfo;

	if (loginfo->level >= 8) {
		pr_debug("LOG: level %u >= 8\n", loginfo->level);
		return false;
	}
	if (loginfo->prefix[sizeof(loginfo->prefix)-1] != '\0') {
		pr_debug("LOG: prefix term %i\n",
			 loginfo->prefix[sizeof(loginfo->prefix)-1]);
		return false;
	}
	return true;
}

static struct xt_target log_tg6_reg __read_mostly = {
	.name 		= "LOG",
	.family		= NFPROTO_IPV6,
	.target 	= log_tg6,
	.targetsize	= sizeof(struct ip6t_log_info),
	.checkentry	= log_tg6_check,
	.me 		= THIS_MODULE,
};

static struct nf_logger ip6t_logger __read_mostly = {
	.name		= "ip6t_LOG",
	.logfn		= &ip6t_log_packet,
	.me		= THIS_MODULE,
};

static int __init log_tg6_init(void)
{
	int ret;

	ret = xt_register_target(&log_tg6_reg);
	if (ret < 0)
		return ret;
	nf_log_register(NFPROTO_IPV6, &ip6t_logger);
	return 0;
}

static void __exit log_tg6_exit(void)
{
	nf_log_unregister(&ip6t_logger);
	xt_unregister_target(&log_tg6_reg);
}

module_init(log_tg6_init);
module_exit(log_tg6_exit);
