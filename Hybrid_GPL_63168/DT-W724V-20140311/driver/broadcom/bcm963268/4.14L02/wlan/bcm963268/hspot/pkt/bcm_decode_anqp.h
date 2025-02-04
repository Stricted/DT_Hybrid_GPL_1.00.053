/*
 * Decode functions which provides decoding of ANQP packets as defined in 802.11u.
 *
 * Copyright (C) 2013, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id:$
 */

#ifndef _BCM_DECODE_ANQP_H_
#define _BCM_DECODE_ANQP_H_

#include "typedefs.h"
#include "bcm_decode.h"
#include "bcm_decode_hspot_anqp.h"

typedef struct {
#ifndef BCM_DECODE_NO_ANQP
	int anqpQueryListLength;
	uint8 *anqpQueryListBuffer;
	int anqpCapabilityListLength;
	uint8 *anqpCapabilityListBuffer;
	int venueNameInfoLength;
	uint8 *venueNameInfoBuffer;
	int emergencyCallNumberInfoLength;
	uint8 *emergencyCallNumberInfoBuffer;
	int networkAuthenticationTypeInfoLength;
	uint8 *networkAuthenticationTypeInfoBuffer;
	int roamingConsortiumListLength;
	uint8 *roamingConsortiumListBuffer;
	int ipAddressTypeAvailabilityInfoLength;
	uint8 *ipAddressTypeAvailabilityInfoBuffer;
	int naiRealmListLength;
	uint8 *naiRealmListBuffer;
	int g3ppCellularNetworkInfoLength;
	uint8 *g3ppCellularNetworkInfoBuffer;
	int apGeospatialLocationLength;
	uint8 *apGeospatialLocationBuffer;
	int apCivicLocationLength;
	uint8 *apCivicLocationBuffer;
	int apLocationPublicIdUriLength;
	uint8 *apLocationPublicIdUriBuffer;
	int domainNameListLength;
	uint8 *domainNameListBuffer;
	int emergencyAlertIdUriLength;
	uint8 *emergencyAlertIdUriBuffer;
	int emergencyNaiLength;
	uint8 *emergencyNaiBuffer;
#endif	/* BCM_DECODE_NO_ANQP */
	int anqpVendorSpecificListLength;
	uint8 *anqpVendorSpecificListBuffer;

#ifndef BCM_DECODE_NO_HOTSPOT_ANQP
	/* hotspot specific */
	bcm_decode_hspot_anqp_t hspot;
#endif	/* BCM_DECODE_NO_HOTSPOT_ANQP */
} bcm_decode_anqp_t;

/* decode ANQP */
int bcm_decode_anqp(bcm_decode_t *pkt, bcm_decode_anqp_t *anqp);

#define BCM_DECODE_ANQP_MAX_LIST_SIZE	16
typedef struct
{
	uint16 queryLen;
	uint16 queryId[BCM_DECODE_ANQP_MAX_LIST_SIZE];
} bcm_decode_anqp_query_list_t;

/* decode ANQP query list */
int bcm_decode_anqp_query_list(bcm_decode_t *pkt, bcm_decode_anqp_query_list_t *queryList);

/* print decoded ANQP query list */
void bcm_decode_anqp_query_list_print(bcm_decode_anqp_query_list_t *queryList);

typedef struct
{
	uint16 vendorLen;
	uint8 vendorData[BCM_DECODE_ANQP_MAX_LIST_SIZE];
} bcm_decode_anqp_vendor_list_t;

/* decode ANQP vendor specific list */
int bcm_decode_anqp_vendor_specific_list(bcm_decode_t *pkt,
	bcm_decode_anqp_vendor_list_t *vendorList);

typedef struct
{
	uint16 capLen;
	uint16 capId[BCM_DECODE_ANQP_MAX_LIST_SIZE];
	bcm_decode_hspot_anqp_capability_list_t hspotCapList;
} bcm_decode_anqp_capability_list_t;

/* decode ANQP capability list */
int bcm_decode_anqp_capability_list(bcm_decode_t *pkt,
	bcm_decode_anqp_capability_list_t *capList);

/* print decoded ANQP capability list */
void bcm_decode_anqp_capability_list_print(bcm_decode_anqp_capability_list_t *capList);

typedef struct
{
	uint8 langLen;
	char lang[VENUE_LANGUAGE_CODE_SIZE + 1];	/* null terminated */
	uint8 nameLen;
	char name[VENUE_NAME_SIZE + 1];		/* null terminated */
} bcm_decode_anqp_venue_name_duple_t;

#define BCM_DECODE_ANQP_MAX_VENUE_NAME	4
typedef struct
{
	uint8 group;
	uint8 type;
	int numVenueName;
	bcm_decode_anqp_venue_name_duple_t venueName[BCM_DECODE_ANQP_MAX_VENUE_NAME];
} bcm_decode_anqp_venue_name_t;

/* decode venue name */
int bcm_decode_anqp_venue_name(bcm_decode_t *pkt, bcm_decode_anqp_venue_name_t *venueName);

/* print decoded venue name */
void bcm_decode_anqp_venue_name_print(bcm_decode_anqp_venue_name_t *venueName);

#define BCM_DECODE_ANQP_MAX_URL_LENGTH	128
typedef struct
{
	uint8 type;
	uint16 urlLen;
	uint8 url[BCM_DECODE_ANQP_MAX_URL_LENGTH + 1];		/* null terminated */
} bcm_decode_anqp_network_authentication_unit_t;

#define BCM_DECODE_ANQP_MAX_AUTHENTICATION_UNIT	8
typedef struct
{
	int numAuthenticationType;
	bcm_decode_anqp_network_authentication_unit_t unit[BCM_DECODE_ANQP_MAX_AUTHENTICATION_UNIT];
} bcm_decode_anqp_network_authentication_type_t;

/* decode network authentication type */
int bcm_decode_anqp_network_authentication_type(bcm_decode_t *pkt,
	bcm_decode_anqp_network_authentication_type_t *auth);

/* print decoded network authentication type */
void bcm_decode_anqp_network_authentication_type_print(
	bcm_decode_anqp_network_authentication_type_t *auth);

/* search decoded network authentication type for online enrollment support */
int bcm_decode_anqp_is_online_enrollment_support(
	bcm_decode_anqp_network_authentication_type_t *auth);

#define BCM_DECODE_ANQP_MAX_OI_LENGTH	8
typedef struct
{
	uint8 oiLen;
	uint8 oi[BCM_DECODE_ANQP_MAX_OI_LENGTH];
} bcm_decode_anqp_oi_duple_t;

#define BCM_DECODE_ANQP_MAX_OI	16
typedef struct
{
	int numOi;
	bcm_decode_anqp_oi_duple_t oi[BCM_DECODE_ANQP_MAX_OI];
} bcm_decode_anqp_roaming_consortium_t;

/* decode roaming consortium */
int bcm_decode_anqp_roaming_consortium(bcm_decode_t *pkt,
	bcm_decode_anqp_roaming_consortium_t *roam);

/* print decoded roaming consortium */
void bcm_decode_anqp_roaming_consortium_print(bcm_decode_anqp_roaming_consortium_t *roam);

/* search decoded roaming consortium for a match */
int bcm_decode_anqp_is_roaming_consortium(bcm_decode_anqp_roaming_consortium_t *roam,
	bcm_decode_anqp_oi_duple_t *oi);

typedef struct
{
	uint8 ipv6;
	uint8 ipv4;
} bcm_decode_anqp_ip_type_t;

/* decode IP address type availability */
int bcm_decode_anqp_ip_type_availability(bcm_decode_t *pkt, bcm_decode_anqp_ip_type_t *ip);

/* print decoded IP address type availability */
void bcm_decode_anqp_ip_type_availability_print(bcm_decode_anqp_ip_type_t *ip);

#define BCM_DECODE_ANQP_MAX_AUTH_PARAM		16
typedef struct
{
	uint8 id;
	uint8 len;
	uint8 value[BCM_DECODE_ANQP_MAX_AUTH_PARAM];
} bcm_decode_anqp_auth_t;

#define BCM_DECODE_ANQP_MAX_AUTH			4
typedef struct
{
	uint8 eapMethod;
	uint8 authCount;
	bcm_decode_anqp_auth_t auth[BCM_DECODE_ANQP_MAX_AUTH];
} bcm_decode_anqp_eap_method_t;

#define BCM_DECODE_ANQP_MAX_REALM_LENGTH	255
#define BCM_DECODE_ANQP_MAX_EAP_METHOD		4
typedef struct
{
	uint8 encoding;
	uint8 realmLen;
	uint8 realm[BCM_DECODE_ANQP_MAX_REALM_LENGTH + 1];	/* null terminated */
	uint8 eapCount;
	bcm_decode_anqp_eap_method_t eap[BCM_DECODE_ANQP_MAX_EAP_METHOD];
} bcm_decode_anqp_nai_realm_data_t;

#define BCM_DECODE_ANQP_MAX_REALM			16
typedef struct
{
	uint16 realmCount;
	bcm_decode_anqp_nai_realm_data_t realm[BCM_DECODE_ANQP_MAX_REALM];
} bcm_decode_anqp_nai_realm_list_t;

/* decode NAI realm */
int bcm_decode_anqp_nai_realm(bcm_decode_t *pkt, bcm_decode_anqp_nai_realm_list_t *realm);

/* print decoded NAI realm */
void bcm_decode_anqp_nai_realm_print(bcm_decode_anqp_nai_realm_list_t *realm);

/* search the decoded NAI realm for a match */
int bcm_decode_anqp_is_realm(bcm_decode_anqp_nai_realm_list_t *realmList,
	char *realmName, uint8 eapMethod, uint8 credential);

#define BCM_DECODE_ANQP_MCC_LENGTH	3
#define BCM_DECODE_ANQP_MNC_LENGTH	3
typedef struct
{
	char mcc[BCM_DECODE_ANQP_MCC_LENGTH + 1];
	char mnc[BCM_DECODE_ANQP_MNC_LENGTH + 1];
} bcm_decode_anqp_plmn_t;

#define BCM_DECODE_ANQP_MAX_PLMN	16
typedef struct
{
	uint8 plmnCount;
	bcm_decode_anqp_plmn_t plmn[BCM_DECODE_ANQP_MAX_PLMN];
} bcm_decode_anqp_3gpp_cellular_network_t;

/* decode 3GPP cellular network */
int bcm_decode_anqp_3gpp_cellular_network(bcm_decode_t *pkt,
	bcm_decode_anqp_3gpp_cellular_network_t *g3pp);

/* print decoded 3GPP cellular network */
void bcm_decode_anqp_3gpp_cellular_network_print(bcm_decode_anqp_3gpp_cellular_network_t *g3pp);

/* search the decoded 3GPP cellular network for a match */
int bcm_decode_anqp_is_3gpp(bcm_decode_anqp_3gpp_cellular_network_t *g3pp,
	bcm_decode_anqp_plmn_t *plmn);

#define BCM_DECODE_ANQP_MAX_DOMAIN_NAME_SIZE 128
typedef struct
{
	uint8 len;
	char name[BCM_DECODE_ANQP_MAX_DOMAIN_NAME_SIZE + 1];	/* null terminated */
} bcm_decode_anqp_domain_name_t;

#define BCM_DECODE_ANQP_MAX_DOMAIN 	16
typedef struct
{
	int numDomain;
	bcm_decode_anqp_domain_name_t domain[BCM_DECODE_ANQP_MAX_DOMAIN];
} bcm_decode_anqp_domain_name_list_t;

/* decode domain name list */
int bcm_decode_anqp_domain_name_list(bcm_decode_t *pkt, bcm_decode_anqp_domain_name_list_t *list);

/* print decoded domain name list */
void bcm_decode_anqp_domain_name_list_print(bcm_decode_anqp_domain_name_list_t *list);

/* search the decoded domain name list for a match */
int bcm_decode_anqp_is_domain_name(bcm_decode_anqp_domain_name_list_t *list,
	char *domain, int isSubdomain);

/* decode query vendor specific */
int bcm_decode_anqp_query_vendor_specific(bcm_decode_t *pkt, uint16 *serviceUpdateIndicator);

#define BCM_DECODE_ANQP_QUERY_REQUEST_TLV_MIN_LENGTH	4

typedef struct
{
	uint8 serviceProtocolType;
	uint8 serviceTransactionId;
	uint16 queryLen;
	uint8 *queryData;
} bcm_decode_anqp_query_request_vendor_specific_tlv_t;

/* decode query request vendor specific TLV */
int bcm_decode_anqp_query_request_vendor_specific_tlv(bcm_decode_t *pkt,
	bcm_decode_anqp_query_request_vendor_specific_tlv_t *request);

#define BCM_DECODE_ANQP_QUERY_RESPONSE_TLV_MIN_LENGTH	5

typedef struct
{
	uint8 serviceProtocolType;
	uint8 serviceTransactionId;
	uint8 statusCode;
	uint16 queryLen;
	uint8 *queryData;
} bcm_decode_anqp_query_response_vendor_specific_tlv_t;

/* decode query response vendor specific TLV */
int bcm_decode_anqp_query_response_vendor_specific_tlv(bcm_decode_t *pkt,
	bcm_decode_anqp_query_response_vendor_specific_tlv_t *response);

#endif /* _BCM_DECODE_ANQP_H_ */
