#ifndef __ATP_WEB_MSGTYPES_H__
#define __ATP_WEB_MSGTYPES_H__

#include "atpconfig.h" /* DTS2013072002019 z81004143 2013/9/6 */


/*all four ports should change together*/
#define ATP_SYS_WEB_HTTP_PORT_D (80)
/* <DTS2013072002019 z81004143 2013/9/6 begin */
/* <DTS2013091607269 z81004143 2013/9/16 begin */
#ifdef SUPPORT_ATP_B880_CUBE
#define ATP_SYS_WEB_HTTPS_PORT_D (33443)
#define ATP_SYS_ACL_HTTPS_PORT_D "33443"
#else
#define ATP_SYS_WEB_HTTPS_PORT_D (443)
#define ATP_SYS_ACL_HTTPS_PORT_D "443"
#endif
/* DTS2013091607269 z81004143 2013/9/16 end> */
/* DTS2013072002019 z81004143 2013/9/6 end> */
#define ATP_SYS_WEB_HTTP_PORTMAPPING_REDIRECT_PORT_D (8080)
#define ATP_SYS_WEB_HTTPS_PORTMAPPING_REDIRECT_PORT_D (8443)

#define ATP_CBBID_WEB_NAME       "web"

enum ATP_WEB_MSGTYPES_EN
{
    ATP_MSG_WEB_START = ATP_MSG_CATEGORY_DEF(ATP_MODULE_WEB),
    ATP_MSG_GET_WAN_INTF_NAME,
    ATP_MSG_WEB_CFG_UPDATE,
    ATP_MSG_GET_WAN_INTF_STATE,
    ATP_MSG_CHECK_HTTPS_PORTMAPPING_EXIST,
} ;
#endif // End of __ATP_WEB_MSGTYPES_H__

