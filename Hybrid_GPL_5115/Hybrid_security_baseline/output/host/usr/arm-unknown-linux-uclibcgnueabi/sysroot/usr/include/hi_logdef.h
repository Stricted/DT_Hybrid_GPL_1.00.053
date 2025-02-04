/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : hi_logdef.h
  版 本 号   : 初稿
  作    者   : l00183967
  生成日期   : D2011_07_28
  最近修改   :

******************************************************************************/
#ifndef __HI_LOGDEF_H__
#define __HI_LOGDEF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HI_LOG_OUTPUTMAXLEN    2048

typedef enum  {
    HI_LOGTYPE_DEBUG_E = 0,
    HI_LOGTYPE_SYSTRACE_E,
    HI_LOGTYPE_PRINT_E,
    HI_LOGTYPE_PRINTMEM_E,
}hi_logtype_e;

/*最多八个类型,因为DEBUG为UCHAR8类型*/
typedef enum  {
    HI_LOG_LEVEL_DBG_E = 0, /* 调试 */
    HI_LOG_LEVEL_ALM_E,     /* 告警 */
    HI_LOG_LEVEL_WRN_E,     /* 提示 */
    HI_LOG_LEVEL_INFO_E,    /* 信息 */
    HI_LOG_LEVEL_MSG_E,     /* 消息 */
    HI_LOG_LEVEL_ERR_E,     /* 错误 */
    HI_LOG_LEVEL_EVT_E,     /* 事件 */
    HI_LOG_LEVEL_RESV_E,    /* 内存打印 */
}hi_dbglevel_e; 

typedef struct  {
    hi_uint32 ui_srcmodule;
    hi_uint32 ui_maskdbgflag;   /*bit0:31,每个比特对应hi_dbglevel_e枚举*/
    hi_uchar8 uc_systraceflag;  /* 0x0: 关闭不输出; 0x1: 输出所有信息; 0x2: 只输出失败信息; 
                                   0x3: 只输出成功信息;0x4: 按照配置返回值精确匹配*/
    hi_uchar8 uc_printflag;
    hi_uchar8 uc_flag;          /*bit0:1远程打印 bit0:0本地打印*/
    hi_uchar8 uc_resv;
    hi_uint32 ui_retcode;
}hi_log_cfg_s;

typedef struct  {
    hi_uint32 ui_srcmodule;
    hi_uint32 ui_logtype;
    hi_uint32 ui_level; 
    hi_uint32 ui_ret;
    hi_uint32 ui_arg1;
    hi_uint32 ui_arg2;
    hi_uint32 ui_arg3;
    hi_uint32 ui_arg4;
    hi_uint32 ui_datalen; 
    hi_uchar8 uc_data[HI_LOG_OUTPUTMAXLEN];
}hi_log_data_s;

#define HI_BUG()              { printk("%s %d\r\n",__func__,__LINE__); (*(hi_int32*)0 = 0);}
#define HI_ASSERT(expt)       if (expt) HI_BUG();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_LOGDEF_H__ */
