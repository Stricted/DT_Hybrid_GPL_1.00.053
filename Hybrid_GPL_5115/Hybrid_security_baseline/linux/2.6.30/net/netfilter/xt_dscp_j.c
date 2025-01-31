/* x_tables module for setting the IPv4/IPv6 DSCP field, Version 1.8
 *
 * (C) 2002 by Harald Welte <laforge@netfilter.org>
 * based on ipt_FTOS.c (C) 2000 by Matthew G. Marsh <mgm@paktronix.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * See RFC2474 for a description of the DSCP field within the IP Header.
*/

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/dsfield.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_dscp_j.h>
#include <linux/netfilter_ipv4/ipt_tos_j.h>

MODULE_AUTHOR("Harald Welte <laforge@netfilter.org>");
MODULE_DESCRIPTION("Xtables: DSCP/TOS field modification");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_DSCP");
MODULE_ALIAS("ip6t_DSCP");
MODULE_ALIAS("ipt_TOS");
MODULE_ALIAS("ip6t_TOS");

#define QOS_TOS_IPP_MARK_ZERO                     0x1 
#define QOS_TOS_MARK_ZERO_IPT                     0xFF

static unsigned int
dscp_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_DSCP_info *dinfo = par->targinfo;
	u_int8_t dscp = ipv4_get_dsfield(ip_hdr(skb)) >> XT_DSCP_SHIFT;

	if (dscp != dinfo->dscp) {
		if (!skb_make_writable(skb, sizeof(struct iphdr)))
			return NF_DROP;

		ipv4_change_dsfield(ip_hdr(skb), (__u8)(~XT_DSCP_MASK),
				    dinfo->dscp << XT_DSCP_SHIFT);

	}
	return XT_CONTINUE;
}

static unsigned int
dscp_tg6(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_DSCP_info *dinfo = par->targinfo;
	u_int8_t dscp = ipv6_get_dsfield(ipv6_hdr(skb)) >> XT_DSCP_SHIFT;

	if (dscp != dinfo->dscp) {
		if (!skb_make_writable(skb, sizeof(struct ipv6hdr)))
			return NF_DROP;

		ipv6_change_dsfield(ipv6_hdr(skb), (__u8)(~XT_DSCP_MASK),
				    dinfo->dscp << XT_DSCP_SHIFT);
	}
	return XT_CONTINUE;
}

static bool dscp_tg_check(const struct xt_tgchk_param *par)
{
	const struct xt_DSCP_info *info = par->targinfo;

	if (info->dscp > XT_DSCP_MAX) {
		printk(KERN_WARNING "DSCP: dscp %x out of range\n", info->dscp);
		return false;
	}
	return true;
}

/* start IP precedence and TOS remark by f00120964 */
/*
static unsigned int
tos_tg_v0(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ipt_tos_target_info *info = par->targinfo;
	struct iphdr *iph = ip_hdr(skb);
	u_int8_t oldtos;

	if ((iph->tos & IPTOS_TOS_MASK) != info->tos) {
		if (!skb_make_writable(skb, sizeof(struct iphdr)))
			return NF_DROP;

		iph      = ip_hdr(skb);
		oldtos   = iph->tos;
		iph->tos = (iph->tos & IPTOS_PREC_MASK) | info->tos;
		csum_replace2(&iph->check, htons(oldtos), htons(iph->tos));
	}

	return XT_CONTINUE;
}
*/
static unsigned int
tos_tg_v0(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ipt_tos_target_info *tosinfo = par->targinfo;
	struct iphdr *iph = ip_hdr(skb);

	if ((iph->tos != tosinfo->tos) || (QOS_TOS_MARK_ZERO_IPT == tosinfo->tos)) {

		__u8 oldtos;
		if (!skb_make_writable(skb, sizeof(struct iphdr)))
			return NF_DROP;
		iph = ip_hdr(skb);
		oldtos = iph->tos;

        if (tosinfo->tos & QOS_TOS_IPP_MARK_ZERO)
        {
            if (QOS_TOS_MARK_ZERO_IPT == tosinfo->tos)
            {
                iph->tos = (iph->tos & (~IPTOS_TOS_MASK)) | (iph->tos & 0x1);
            }
            else if (IPTOS_TOS(tosinfo->tos))
            {
                iph->tos = IPTOS_TOS(tosinfo->tos) | (iph->tos & 0x1);
            }
            else if (IPTOS_PREC(tosinfo->tos))
            {
                iph->tos = IPTOS_PREC(tosinfo->tos) | (iph->tos & 0x1);
            } 
            else
            {
                iph->tos = (iph->tos & (~IPTOS_PREC_MASK)) | (iph->tos & 0x1);
            }
        }
        else
        {
            if (IPTOS_TOS(tosinfo->tos) 
                && IPTOS_PREC(tosinfo->tos))
            {
                iph->tos = (iph->tos & 0x1) | tosinfo->tos;
            }
            else if (IPTOS_TOS(tosinfo->tos))
            {
                iph->tos = ((iph->tos & (~IPTOS_TOS_MASK)) | IPTOS_TOS(tosinfo->tos)) | (iph->tos & 0x1);
            }
            else if (IPTOS_PREC(tosinfo->tos))
            {
                iph->tos = ((iph->tos & (~IPTOS_PREC_MASK)) | IPTOS_PREC(tosinfo->tos)) | (iph->tos & 0x1);
            }
            else
            {
                iph->tos = iph->tos & 0x1;
            }
        }
  
        
		csum_replace2(&iph->check, htons(oldtos), htons(iph->tos));
	}

	return XT_CONTINUE;
}
/* end IP precedence and TOS remark by f00110348 */
static bool tos_tg_check_v0(const struct xt_tgchk_param *par)
{
	const struct ipt_tos_target_info *info = par->targinfo;
	const uint8_t tos = info->tos;
	/*
	if (tos != IPTOS_LOWDELAY && tos != IPTOS_THROUGHPUT &&
	    tos != IPTOS_RELIABILITY && tos != IPTOS_MINCOST &&
	    tos != IPTOS_NORMALSVC) {
		printk(KERN_WARNING "TOS: bad tos value %#x\n", tos);
		return false;
	}*/

	if ((tos < 0) || (tos > 255)) {
		printk(KERN_WARNING "TOS: bad tos value %#x\n", tos);
		return false;
	}

	return true;
}

static unsigned int
tos_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_tos_target_info *info = par->targinfo;
	struct iphdr *iph = ip_hdr(skb);
	u_int8_t orig, nv;

	orig = ipv4_get_dsfield(iph);
	nv   = (orig & ~info->tos_mask) ^ info->tos_value;

	if (orig != nv) {
		if (!skb_make_writable(skb, sizeof(struct iphdr)))
			return NF_DROP;
		iph = ip_hdr(skb);
		ipv4_change_dsfield(iph, 0, nv);
	}

	return XT_CONTINUE;
}

static unsigned int
tos_tg6(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_tos_target_info *info = par->targinfo;
	struct ipv6hdr *iph = ipv6_hdr(skb);
	u_int8_t orig, nv;

	orig = ipv6_get_dsfield(iph);
	nv   = (orig & info->tos_mask) ^ info->tos_value;

	if (orig != nv) {
		if (!skb_make_writable(skb, sizeof(struct iphdr)))
			return NF_DROP;
		iph = ipv6_hdr(skb);
		ipv6_change_dsfield(iph, 0, nv);
	}

	return XT_CONTINUE;
}

static struct xt_target dscp_tg_reg[] __read_mostly = {
	{
		.name		= "DSCP",
		.family		= NFPROTO_IPV4,
		.checkentry	= dscp_tg_check,
		.target		= dscp_tg,
		.targetsize	= sizeof(struct xt_DSCP_info),
		.table		= "mangle",
		.me		= THIS_MODULE,
	},
	{
		.name		= "DSCP",
		.family		= NFPROTO_IPV6,
		.checkentry	= dscp_tg_check,
		.target		= dscp_tg6,
		.targetsize	= sizeof(struct xt_DSCP_info),
		.table		= "mangle",
		.me		= THIS_MODULE,
	},
	{
		.name		= "TOS",
		.revision	= 0,
		.family		= NFPROTO_IPV4,
		.table		= "mangle",
		.target		= tos_tg_v0,
		.targetsize	= sizeof(struct ipt_tos_target_info),
		.checkentry	= tos_tg_check_v0,
		.me		= THIS_MODULE,
	},
	{
		.name		= "TOS",
		.revision	= 1,
		.family		= NFPROTO_IPV4,
		.table		= "mangle",
		.target		= tos_tg,
		.targetsize	= sizeof(struct xt_tos_target_info),
		.me		= THIS_MODULE,
	},
	{
		.name		= "TOS",
		.revision	= 1,
		.family		= NFPROTO_IPV6,
		.table		= "mangle",
		.target		= tos_tg6,
		.targetsize	= sizeof(struct xt_tos_target_info),
		.me		= THIS_MODULE,
	},
};

static int __init dscp_tg_init(void)
{
	return xt_register_targets(dscp_tg_reg, ARRAY_SIZE(dscp_tg_reg));
}

static void __exit dscp_tg_exit(void)
{
	xt_unregister_targets(dscp_tg_reg, ARRAY_SIZE(dscp_tg_reg));
}

module_init(dscp_tg_init);
module_exit(dscp_tg_exit);
