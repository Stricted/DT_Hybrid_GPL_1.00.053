/******************************************************************************
  版权所有  : 2009，华为技术有限公司
  文 件 名  : sysinfoapi.h
  作    者  : c47036
  版    本  : v1.0
  创建日期  : 2009-2-20
  描    述  : DNS API

  历史记录      :
   1.日    期   : 2009-2-20
     作    者   : c47036
     修改内容   : 完成初稿

******************************************************************************/

#ifndef __SYS_CMSAPI_API_H__
#define __SYS_CMSAPI_API_H__
#include "msgapi.h"
/*
 *  Internet Gateway Device cfm procs
 */
#ifndef SUPPORT_DESKTOP
#include "atpflashapi.h"
#endif

#ifdef SUPPORT_ATP_LAN
#define USB_LAN_PROFILE         ", USBLAN:1"                // Require IGD1.1
#else
#define USB_LAN_PROFILE
#endif

#ifdef SUPPORT_ATP_WLAN
#define WIFI_LAN_PROFILE        ", WiFiLAN:1"               // Require IGD1.1
#else
#define WIFI_LAN_PROFILE
#endif

#if defined(SUPPORT_ATP_XDSL) || defined(SUPPORT_ATP_ADSL)
#define ADSL_WAN_PROFILE        ", ADSLWAN:1"               // Require IGD1.1
#else
#define ADSL_WAN_PROFILE
#endif

#ifdef SUPPORT_ATP_WANETH
#define ETH_WAN_PROFILE         ", EthernetWAN:1"           // Require IGD1.1
#else
#define ETH_WAN_PROFILE
#endif

#ifdef SUPPORT_ATP_POTS
#define POTS_WAN_PROFILE        ", POTSWAN:1"               // Require IGD1.1
#else
#define POTS_WAN_PROFILE
#endif

#ifdef SUPPORT_ATP_QUEUE
#define QOS_PROFILE             ", QoS:1"                   // Require IGD1.1
#else
#define QOS_PROFILE
#endif

#ifdef SUPPORT_QOS_DYN_FLOW
#define QOS_DYN_FLOW_PROFILE    ", QoSDynamicFlow:1"        // Require IGD1.1
#else
#define QOS_DYN_FLOW_PROFILE
#endif

#define BRIDGE_PROFILE          ", Bridging:1"              // Require IGD1.1

#ifdef SUPPORT_ATP_SNTP
#define TIME_PROFILE            ", Time:1"                  // Require IGD1.1
#else
#define TIME_PROFILE
#endif

#define IPPING_PROFILE          ", IPPing:1"                // Require IGD1.1

/*start modify of DTS2010112601709 by p00134244*/
#if 0
#if defined(SUPPORT_ATP_XTM) || defined(SUPPORT_ATP_ATM)
#define ATM_LOOPBACK_PROFILE    ", ATMLoopback:1"           // Require IGD1.1
#else
#define ATM_LOOPBACK_PROFILE
#endif
#else
#if (defined(SUPPORT_ATP_XTM) || defined(SUPPORT_ATP_ATM)) && defined(SUPPORT_ATP_XDSL_DIAG)
#define ATM_LOOPBACK_PROFILE    ", ATMLoopback:1"           // Require IGD1.1
#else
#define ATM_LOOPBACK_PROFILE
#endif
#endif
/*end modify of DTS2010112601709 by p00134244*/

#ifdef SUPPORT_ATP_XDSL_DIAG
#define ADSLDIAG_PROFILE        ", DSLDiagnostics:1"        // Require IGD1.1
#else
#define ADSLDIAG_PROFILE
#endif

#if defined(SUPPORT_ATP_CWMP) && defined(SUPPORT_ATP_TR111) && defined(SUPPORT_ATP_STUNC)
#define DEVASSO_PROFILE         ", DeviceAssociation:1"     // Require IGD1.1
#define UDPCONREQ_PROFILE       ", UDPConnReq:1"
#else
#define UDPCONREQ_PROFILE
#define DEVASSO_PROFILE
#endif

// For TR140 usb storage
#if defined(SUPPORT_ATP_USB_STORAGE) && defined(SUPPORT_ATP_TR140)
#define FTPSERVER_PROFILE       ", FTPServer:1"
#define USB_STORAGE_PROFILE     ", StorageService:1.0[](Baseline:1, UserAccess:1, VolumeConfig:1" FTPSERVER_PROFILE ")"
#else
#define FTPSERVER_PROFILE   ""
#define USB_STORAGE_PROFILE ""
#endif

// For TR104, VoIP CPE
#ifdef SUPPORT_ATP_VOICE
#define VOICE_PROFILE ", VoiceService:1.0[](Endpoint:1, SIPEndpoint:1, TAEndpoint:1)"
#else
#define VOICE_PROFILE ""
#endif

#ifdef SUPPORT_ATP_TR143_DOWNLOAD
#define DOWNLOAD_PROFILE ", Download:1"
#else
#define DOWNLOAD_PROFILE ""
#endif
#ifdef SUPPORT_ATP_TR143_DOWNLOAD_TCP
#define DOWNLOAD_TCP_PROFILE ", DownloadTCP:1"
#else
#define DOWNLOAD_TCP_PROFILE ""
#endif


#ifdef SUPPORT_ATP_TR143_UPLOAD
#define UPLOAD_PROFILE ", Upload:1"
#else
#define UPLOAD_PROFILE ""
#endif

#ifdef SUPPORT_ATP_TR143_UPLOAD_TCP
#define UPLOAD_TCP_PROFILE ", UploadTCP:1"
#else
#define UPLOAD_TCP_PROFILE ""
#endif

#ifdef SUPPORT_ATP_TR143_UDPECHO
#define UDPECHO_PROFILE ", UDPEcho:1"
#else
#define UDPECHO_PROFILE ""
#endif

#ifdef SUPPORT_ATP_TR143_UDPECHOPLUS
#define UDPECHOPLUS_PROFILE ", UDPEchoPlus:1"
#else
#define UDPECHOPLUS_PROFILE ""
#endif

#ifdef SUPPORT_ATP_TELEFONICA_SYSTEMSTATES
#define SYSTEMSTATE_PROFILE ", X_TELEFONICA-ES_SystemState:1"
#else
#define SYSTEMSTATE_PROFILE
#endif

#ifdef SUPPORT_ATP_IGMP_EXT_PARAMETERS_SET
#define IGMP_PROFILE ", X_TELEFONICA-ES_IGMP:1"
#else
#define IGMP_PROFILE
#endif

#ifdef SUPPORT_ATP_WAN_NAT_TYPE_SEPARATE_FROM_ENABLE
#define NATTYPE_PROFILE ", X_TELEFONICA-ES_NATType:1"
#else
#define NATTYPE_PROFILE
#endif

/********************** Global APIs Definitions *****************************/

#if defined(__cplusplus)
extern "C"
{
#endif                      

/* DeviceSummary, defined by TR-106, section 3.7 */
static const VOS_CHAR  *g_szDevSum = "InternetGatewayDevice:1.1[](Baseline:1, EthernetLAN:1" \
                                     USB_LAN_PROFILE WIFI_LAN_PROFILE ADSL_WAN_PROFILE \
                                     ETH_WAN_PROFILE POTS_WAN_PROFILE QOS_PROFILE \
                                     QOS_DYN_FLOW_PROFILE BRIDGE_PROFILE TIME_PROFILE \
                                     IPPING_PROFILE ATM_LOOPBACK_PROFILE ADSLDIAG_PROFILE \
                                     DEVASSO_PROFILE UDPCONREQ_PROFILE \
                                     DOWNLOAD_PROFILE DOWNLOAD_TCP_PROFILE UPLOAD_PROFILE UPLOAD_TCP_PROFILE UDPECHO_PROFILE UDPECHOPLUS_PROFILE")" \
                                     USB_STORAGE_PROFILE VOICE_PROFILE SYSTEMSTATE_PROFILE IGMP_PROFILE NATTYPE_PROFILE ;

#ifdef SUPPORT_ATP_UPNP_DM_SERVER
VOS_UINT32 ATP_UPNPDM_Init(VOS_VOID);
#endif

#ifdef SUPPORT_ATP_PERIODIC_STATISTIC
VOS_UINT32 ATP_PS_Init(VOS_VOID);
#endif

VOS_UINT32 ATP_SYS_DeviceInfoInit(VOS_VOID);

#if defined(__cplusplus)
}
#endif

#endif
