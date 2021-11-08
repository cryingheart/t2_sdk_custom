#pragma once
#include <kernel/os.h>

#define novirtstr(s) ({static const char __c[] __irqro = (s); &__c[0];})

#define pr_log_simple(_fmt, ...) ({                     \
            if(0) os_printf(_fmt, ##__VA_ARGS__);       \
            os_printf(novirtstr(_fmt), ##__VA_ARGS__);  \
        })

/* print helper macros */
#define P_TIME   "[%mT] "
#define P_HEAP   "% 6d "
#define P_THREAD "%s(%d)"
#define P_SYM    "@%x@"

#define log_os_thread_name() (in_interrupt() ? "IRQ" : os_thread_name(os_self()))

#define pr_log_time(_fmt, ...) pr_log_simple(P_TIME _fmt, ##__VA_ARGS__)
#define pr_log_time_thread_lr_func(_fmt, ...) pr_log_simple(P_TIME P_HEAP P_THREAD " LR" P_SYM " F:%s " _fmt, \
            os_avail_heap(), \
            log_os_thread_name(), \
            cpu_get_ipsr(), \
            (uintptr_t)__builtin_return_address(0), \
            __func__,  ##__VA_ARGS__)

#define pr_log_func(_fmt, ...) pr_log_simple("%s: " _fmt, __func__, ##__VA_ARGS__)

#define PR_LEVEL_NONE  0
#define PR_LEVEL_ERR   1
#define PR_LEVEL_WARN  2
#define PR_LEVEL_INFO  3
#define PR_LEVEL_DEBUG 4

#if !defined(OS_DEBUG)
#define OS_DEBUG PR_LEVEL_WARN
#endif

#if 0
#define __pr_log pr_log_time_thread_lr_func
#else
#define __pr_log pr_log_time
#endif

#define __pr_log_level(_lvl, _fmt, ...)                         \
    ({if (OS_DEBUG >= _lvl) __pr_log(_fmt, ##__VA_ARGS__);})

#define pr_debug(_fmt, ...)  __pr_log_level(PR_LEVEL_DEBUG, _fmt, ## __VA_ARGS__)
#define pr_info(_fmt, ...)   __pr_log_level(PR_LEVEL_INFO, _fmt, ## __VA_ARGS__)
#define pr_warn(_fmt, ...)   __pr_log_level(PR_LEVEL_WARN, _fmt, ## __VA_ARGS__)
#define pr_err(_fmt, ...)    __pr_log_level(PR_LEVEL_ERR, _fmt, ## __VA_ARGS__)
#define pr_always(_fmt, ...) __pr_log(_fmt, ## __VA_ARGS__)

/* print helper macros for SSID */
/* use like: os_printf("..." P_SSID "...", A_SSID(ssid)) */
#define P_SSID "%.*s"
#define A_SSID(_ssid)                                                   \
    _Generic((_ssid),                                                   \
             const struct wifi_ssid* : A_SSID_SSID_LENGTH(_ssid),       \
             struct wifi_ssid* : A_SSID_SSID_LENGTH(_ssid),             \
             const uint8_t* : A_SSID_IE_LENGTH(_ssid),                  \
             uint8_t* : A_SSID_IE_LENGTH(_ssid)),                       \
    _Generic((_ssid),                                                   \
             const struct wifi_ssid* : A_SSID_SSID_VALUE(_ssid),        \
             struct wifi_ssid* : A_SSID_SSID_VALUE(_ssid),              \
             const uint8_t* : A_SSID_IE_VALUE(_ssid),                   \
             uint8_t* : A_SSID_IE_VALUE(_ssid))                         \

/* xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx */
#define P_UUID "%4pm-%2pm-%2pm-%2pm-%6pm"
#define A_UUID(u_) &(u_)->uuid[0], &(u_)->uuid[4], &(u_)->uuid[6], &(u_)->uuid[8], &(u_)->uuid[10]

/* helper macros for above, apparently we need to force the type here
 * since both GCC and CLANG seem to agree that it's ok to semantically
 * parse the expressions even if they are not selected */
#define A_SSID_SSID_LENGTH(_ssid) ((struct wifi_ssid*)(_ssid))->ws_len
#define A_SSID_SSID_VALUE(_ssid)  ((struct wifi_ssid*)(_ssid))->ws_ssid
#define A_SSID_IE_LENGTH(_ie)     ((uint8_t*)(_ie))[ELEMENT_LENGTH]
#define A_SSID_IE_VALUE(_ie)      &((uint8_t*)(_ie))[ELEMENT_INFORMATION]

#define INNOTRACE_CPU_EXEC_MASK (SUPPORT_ENABLE_CPU_MASK&1)
