#ifndef __ATP_SYSINFOCMS_MSGTYPES_H__
#define __ATP_SYSINFOCMS_MSGTYPES_H__

#ifdef SUPPORT_ATP_DT_W724V_UPG_LED
enum ATP_SYSINFOCMS_MSGTYPES_EN
{
    ATP_MSG_SYSINFOCMS_START = ATP_MSG_CATEGORY_DEF(ATP_MODULE_SYSINFOCMS),
} ;

enum ATP_SYSINFOCMS_SERVICE_TYPE_EN
{
    ATP_SYSINFOCMS_SERVICE = ATP_MSG_CATEGORY_DEF(ATP_MODULE_SYSINFOCMS) + 100,
};

enum ATP_SYSINFOCMS_STATUS_EN
{
    ATP_SYSINFO_UPG_LED_BLINK_START,
    ATP_SYSINFOCMS_UPG_LED_BLINK_STOP,
};
#endif

#endif // End of __ATP_SYSINFOCMS_MSGTYPES_H__

