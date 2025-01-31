#ifndef __ATP_ROUTECMS_MSGTYPES_H__
#define __ATP_ROUTECMS_MSGTYPES_H__

/*policy rule pref*/
#define HYBRID_POLICY_ROUTE_PREF_START   30000
/*Start of Protocol 2014-5-23 15:54 for HOMEGW-17282:添加blacklist路由 by t00189036*/
#define HYBRID_BLACKLIST_PREF     (1 + HYBRID_POLICY_ROUTE_PREF_START)
/*End of Protocol 2014-5-23 15:54 for by t00189036*/
#define HYBRID_DHCP_OPT121_PREF     (2 + HYBRID_POLICY_ROUTE_PREF_START)
/*Start of Protocol 2014-4-15 10:21 for DTS2014032711111:DSCP标记的telekomlist有较高的优先级没有实现 by t00189036*/
#define HYBRID_TELEKOM_DSCP_LIST_PREF    (3 + HYBRID_POLICY_ROUTE_PREF_START)
/*End of Protocol 2014-4-15 10:21 for by t00189036*/
#define HYBRID_TELEKOM_LIST_PREF    (4 + HYBRID_POLICY_ROUTE_PREF_START)
#define HYBRID_USER_LIST_PREF       (5 + HYBRID_POLICY_ROUTE_PREF_START)
#define ATP_BYPASS_ROUTE_PREF (6 + HYBRID_POLICY_ROUTE_PREF_START)
#define ATP_TUNNEL_ROUTE_PREF (7 + HYBRID_POLICY_ROUTE_PREF_START)

/*policy route table*/
/*Telekom and user list use the same table*/
#define INTERFACE_POLICE_ROUTING_TABLE		201
/*Start of Protocol 2014-5-23 15:55 for HOMEGW-17282:添加blacklist路由 by t00189036*/
#define HYBRID_BLACKLIST_TABLE    202
/*End of Protocol 2014-5-23 15:55 for by t00189036*/
#define HYBRID_DHCP_OPT121_TABLE    203
#define HYBRID_FILTER_LIST_TABLE    204
#define ATP_BYPASS_ROUTE_TABLE 205
#define ATP_TUNNEL_ROUTE_TALBE 206

/*Start of Protocol 2014-5-23 15:55 for HOMEGW-17282:添加blacklist路由 by t00189036*/
#define BLACK_LIST_CHAIN "BLACK_LIST"/*用于区分blacklist*/
/*End of Protocol 2014-5-23 15:55 for by t00189036*/
#define ROUTE_CONTROL_CHAIN "ROUTE_CTL_LIST"/*用于区分blacklist*/
#define ROUTE_OPTION121_CHAIN "ROUTE_OPTION121_LIST"/*用于区分option121*/


enum ATP_ROUTECMS_MSGTYPES_EN
{
    ATP_MSG_ROUTECMS_START = ATP_MSG_CATEGORY_DEF(ATP_MODULE_ROUTECMS),
} ;

#endif // End of __ATP_ROUTECMS_MSGTYPES_H__

