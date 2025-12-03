#ifndef _MOCAR_LOG_H_
#define _MOCAR_LOG_H_

#include "zlog.h"

#define MOCAR_LOG_CONF   "/usr/local/config/mocar_log.conf"
#define MOCAR_LOG_RELOAD_TIME  60

#define MOCAR_LOG_INIT(_CONF_PATH_, _CNAME_)    dzlog_init(_CONF_PATH_, _CNAME_)   

#define MOCAR_LOG_RELOAD(_CONF_PATH_)           zlog_reload(_CONF_PATH_)                                  

#define MOCAR_LOG_FINI()        \
    do                                       \
    {                                        \
        zlog_fini();        \
    } while (0)   

#define MOCAR_LOG_FATAL(_MSG_)                  \
    do                                       \
    {                                        \
        dzlog_fatal(_MSG_); \
    } while (0)
#define MOCAR_LOG_FATAL_FMT(_FMT_, ...)                            \
    do                                                          \
    {                                                           \
        dzlog_fatal(_FMT_, ##__VA_ARGS__); \
    } while (0)

#define MOCAR_LOG_ERROR(_MSG_)                  \
    do                                       \
    {                                        \
        dzlog_error(_MSG_); \
    } while (0)
#define MOCAR_LOG_ERROR_FMT(_FMT_, ...)                            \
    do                                                          \
    {                                                           \
        dzlog_error(_FMT_, ##__VA_ARGS__); \
    } while (0)

#define MOCAR_LOG_WARN(_MSG_)                  \
    do                                      \
    {                                       \
        dzlog_warn(_MSG_); \
    } while (0)
#define MOCAR_LOG_WARN_FMT(_FMT_, ...)                            \
    do                                                         \
    {                                                          \
        dzlog_warn(_FMT_, ##__VA_ARGS__); \
    } while (0)

#define MOCAR_LOG_NOTICE(_MSG_)                  \
    do                                      \
    {                                       \
        dzlog_notice(_MSG_); \
    } while (0)
#define MOCAR_LOG_NOTICE_FMT(_FMT_, ...)                            \
    do                                                         \
    {                                                          \
        dzlog_notice(_FMT_, ##__VA_ARGS__); \
    } while (0)

#define MOCAR_LOG_INFO(_MSG_)                  \
    do                                       \
    {                                        \
        dzlog_info(_MSG_); \
    } while (0)
#define MOCAR_LOG_INFO_FMT(_FMT_, ...)                            \
    do                                                          \
    {                                                           \
        dzlog_info(_FMT_, ##__VA_ARGS__); \
    } while (0)

#define MOCAR_LOG_DEBUG(_MSG_)                  \
    do                                       \
    {                                        \
        dzlog_debug(_MSG_); \
    } while (0)
#define MOCAR_LOG_DEBUG_FMT(_FMT_, ...)                            \
    do                                                          \
    {                                                           \
        dzlog_debug(_FMT_, ##__VA_ARGS__); \
    } while (0)
#endif