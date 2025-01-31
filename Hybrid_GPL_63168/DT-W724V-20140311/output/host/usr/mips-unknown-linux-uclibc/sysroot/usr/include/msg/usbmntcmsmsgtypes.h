/**\file usbmntcmsmsgtypes.h
  *USBMOUNT需要发送的消息
  *\copyright 2007-2020，华为技术有限公司
  *\author w00190448
  *\date  2012-1-31
*/
#ifndef __ATP_USBMOUNTCMS_MSGTYPES_H__
#define __ATP_USBMOUNTCMS_MSGTYPES_H__

#define ATP_CBBID_USBMOUNTCMS_NAME "usbmountcms"

enum ATP_USBMOUNTCMS_MSGTYPES_EN
{
    ATP_MSG_USBMOUNTCMS_START = ATP_MSG_CATEGORY_DEF(ATP_MODULE_USBMOUNTCMS),
    ATP_MSG_MONITOR_EVT_USBSTORAGE_MOUNT_DISKSUBAREA_REFRESH,
    ATP_MSG_MONITOR_EVT_USBSTORAGE_ALL_MOUNT_FINISH_REFRESH,
    ATP_MSG_USBMOUNTCMS_MOUNT_SUBAREA_RESULT,
} ;
#endif
