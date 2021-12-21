/******************************************************************************

            版权所有 (C), 2016-2017, 西安交大捷普网络科技有限公司

 ******************************************************************************
    文 件 名 : DebugLog.cpp
    版 本 号 : V1.0
    作    者 : zhangcan
    生成日期 : 2017年12月6日
    功能描述 : 主要用于打印日志和记录日志作用，同时支持Windows、Linux、Unix系统
    修改历史 :
******************************************************************************/
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
# define SYS_WIN32_DEBUG
#else
# define SYS_LINUX_DEBUG
#endif

#if defined(SYS_WIN32_DEBUG)
# include <windows.h>
#elif defined(SYS_LINUX_DEBUG)
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>
# include <unistd.h>
# include <time.h>
# include <sys/stat.h>
# include <sys/types.h>
#endif

#include <vector>
#include <string>
#include "DebugLog.h"

/* 日志文件大小和路径根据自己情况而定 */
#define DEBUG_LOG_FILE_NAME     260   //日志文件路径长度
#define DEBUG_LOG_FILE_LEN_MIN  10    //日志文件最小值(10Mb)

/* 日志锁 */
typedef struct {
#if defined(SYS_WIN32_DEBUG)
    volatile LONG locked; /**< lock status 0 = unlocked, 1 = locked */
#elif defined(SYS_LINUX_DEBUG)
    volatile int locked;  /**< lock status 0 = unlocked, 1 = locked */
#endif
} debug_log_spinlock_t;

/* 日志模块 */
typedef struct debug_log_data {
    int log_num;
    int log_mode;
    int log_file_len;
    std::string log_file_name;
} DEBUG_LOG_DATA_S;

/* 日志模块个数 */
static int g_debug_log_mod_max = 100;

/* 初始化日志锁 */
static debug_log_spinlock_t g_debug_log_lock = {
    .locked = 0
};

/* 保存日志模块列表 */
static std::vector<DEBUG_LOG_DATA_S> g_vDebugLog;

/*****************************************************************************
    函 数 名 : debug_log_spinlock_lock
    功能描述 : 日志加锁
    输入参数 : debug_log_spinlock_t *sl  
    输出参数 : 无
    返 回 值 : 无
    作    者 : zhangcan
    日    期 : 2017年12月16日
*****************************************************************************/
static inline void debug_log_spinlock_lock(debug_log_spinlock_t *sl)
{
#if defined(SYS_WIN32_DEBUG)
    InterlockedIncrement((LPLONG)&(sl->locked));
#elif defined(SYS_LINUX_DEBUG)
	int lock_val = 1;
    asm volatile (
            "1:\n"
            "xchg %[locked], %[lv]\n"
            "test %[lv], %[lv]\n"
            "jz 3f\n"
            "2:\n"
            "pause\n"
            "cmpl $0, %[locked]\n"
            "jnz 2b\n"
            "jmp 1b\n"
            "3:\n"
            : [locked] "=m" (sl->locked), [lv] "=q" (lock_val)
            : "[lv]" (lock_val)
            : "memory");
#endif
}

/*****************************************************************************
    函 数 名 : debug_log_spinlock_unlock
    功能描述 : 日志解锁
    输入参数 : debug_log_spinlock_t *sl  
    输出参数 : 无
    返 回 值 : 无
    作    者 : zhangcan
    日    期 : 2017年12月16日
*****************************************************************************/
static inline void debug_log_spinlock_unlock(debug_log_spinlock_t *sl)
{
#if defined(SYS_WIN32_DEBUG)
    InterlockedDecrement((LPLONG)&(sl->locked));
#elif defined(SYS_LINUX_DEBUG)
    int unlock_val = 0;
    asm volatile (
            "xchg %[locked], %[ulv]\n"
            : [locked] "=m" (sl->locked), [ulv] "=q" (unlock_val)
            : "[ulv]" (unlock_val)
            : "memory");
#endif
}

/*****************************************************************************
    函 数 名 : debug_log_init
    功能描述 : 日志初始化
    输入参数 : const char *name   
               const char *file  
    输出参数 : 无
    返 回 值 : int
    作    者 : zhangcan
    日    期 : 2017年12月6日
*****************************************************************************/
int debug_log_init(const char *name, const char *file)
{
    int num = -1;

    debug_log_spinlock_lock(&g_debug_log_lock);

    if (name && file) {
        if (g_vDebugLog.size() <= g_debug_log_mod_max) {
            int index = -1;
            std::string s_file_name;
            std::string s_file = file;

            index = s_file.rfind('/');
            if (index != std::string::npos) {
                std::string s1 = s_file.substr(0, index+1);
                std::string s2 = s_file.substr(index+1);
                s_file_name  = s1;
                s_file_name += name;
                s_file_name += "_";
                s_file_name += s2;
            } else {
                if (s_file.empty()) {
                    s_file_name  = name;
                    s_file_name += ".log";
                } else {
                    s_file_name  = name;
                    s_file_name += "_";
                    s_file_name += s_file;
                }
            }

            /* 判断是否存在日志模块 */
            for (auto &l : g_vDebugLog) {
                if (!(l.log_file_name.compare(s_file_name))) {
                    num = l.log_num;
                    goto UNLOCK;
                }
            }

            /* 添加日志模块 */
            DEBUG_LOG_DATA_S log;
            log.log_num       = g_vDebugLog.size() + 1;
            log.log_mode	  = DEBUG_LOG_MODE_FILE;
            log.log_file_len  = DEBUG_LOG_FILE_LEN_MIN * 1048576;
            log.log_file_name = s_file_name;
            g_vDebugLog.push_back(log);
            num = log.log_num;
            goto UNLOCK;
        }
    }

UNLOCK:
    debug_log_spinlock_unlock(&g_debug_log_lock);
    return num;
}

/*****************************************************************************
    函 数 名 : debug_log_set_mode
    功能描述 : 设置日志记录模式
    输入参数 : int num  
               int mode   
    输出参数 : 无
    返 回 值 : 无
    作    者 : zhangcan
    日    期 : 2017年12月7日
*****************************************************************************/
void debug_log_set_mode(int num, int mode)
{
    debug_log_spinlock_lock(&g_debug_log_lock);

    if ((num > 0) && (num <= g_vDebugLog.size()) &&
        (mode > 0) && (mode < DEBUG_LOG_MODE_MAX)) {
        auto &it = g_vDebugLog[num - 1];
        it.log_mode = mode;
    }

    debug_log_spinlock_unlock(&g_debug_log_lock);
}

/*****************************************************************************
    函 数 名 : debug_log_get_mode
    功能描述 : 获取日志记录模式
    输入参数 : int num  
    输出参数 : 无
    返 回 值 : int
    作    者 : zhangcan
    日    期 : 2018年2月3日
*****************************************************************************/
int debug_log_get_mode(int num)
{
    int mode = DEBUG_LOG_MODE_DISABLE;

    if ((num > 0) && (num <= g_vDebugLog.size())) {
        auto &it = g_vDebugLog[num - 1];
        mode = it.log_mode;
    }

    return mode;
}

/*****************************************************************************
    函 数 名 : debug_log_set_file
    功能描述 : 设置日志文件大小
    输入参数 : int num   
               int length  
    输出参数 : 无
    返 回 值 : 无
    作    者 : zhangcan
    日    期 : 2017年12月7日
*****************************************************************************/
void debug_log_set_file(int num, int length)
{
    debug_log_spinlock_lock(&g_debug_log_lock);

    if ((num > 0) && (num <= g_vDebugLog.size())) {
        auto &it = g_vDebugLog[num - 1];
        it.log_file_len = (length <= DEBUG_LOG_FILE_LEN_MIN) ?
            (DEBUG_LOG_FILE_LEN_MIN * 1048576) : (length * 1048576);
    }

    debug_log_spinlock_unlock(&g_debug_log_lock);
}

/*****************************************************************************
    函 数 名 : debug_log_print
    功能描述 : 记录日志文件，此接口提供 Linux 和 Windows 两种
    输入参数 : int num
               const char *function
               unsigned int line
               const char *format
               ...
    输出参数 : 无
    返 回 值 : 无
    作    者 : zhangcan
    日    期 : 2016年2月24日
*****************************************************************************/
void debug_log_print(int num, const char *function,
    unsigned int line, const char *format, ...)
{
    debug_log_spinlock_lock(&g_debug_log_lock);

    if ((num > 0) && (num <= g_vDebugLog.size())) {
        long lFileLen = 0;
        FILE *pFd = stdout;
        auto &it = g_vDebugLog[num - 1];

        switch (it.log_mode) {
            case DEBUG_LOG_MODE_FILE:
            {
                /* 获取文件大小 */
#if defined(SYS_WIN32_DEBUG)				
                int iFd = -1;
                if ((iFd = _open(it.log_file_name.c_str(), O_RDONLY)) >= 0) {
                    lFileLen = _filelength(iFd);
                    _close(iFd);
                }
#elif defined(SYS_LINUX_DEBUG)
                struct stat stStat;
                if (!stat(it.log_file_name.c_str(), &stStat)) {
                    lFileLen = (long)stStat.st_size;
                }
#endif
                /* 判断文件大小 */
                if (lFileLen > it.log_file_len) {
                    if ((pFd = fopen(it.log_file_name.c_str(), "w")) == NULL) {
                        goto UNLOCK;
                    }
                } else {
                    if ((pFd = fopen(it.log_file_name.c_str(), "a")) == NULL) {
                        goto UNLOCK;
                    }
                }
                break;
            }
            case DEBUG_LOG_MODE_STDOUT: break;
            case DEBUG_LOG_MODE_DISABLE: goto UNLOCK;
            default: goto UNLOCK;
		}

        time_t timer_t;
        char szTimeBuf[64] = {0};

        timer_t = time(NULL);
        strftime(szTimeBuf, sizeof(szTimeBuf), "%Y/%m/%d %X", localtime(&timer_t));
        fprintf(pFd, "[%s][%s:%d] ", szTimeBuf, function, line);
        fflush(pFd);

        va_list ap;
        char szLogBuf[4096] = {0};

        va_start(ap, format);
        int size = vsnprintf(szLogBuf, sizeof(szLogBuf), format, ap);
        va_end(ap);
        if (size < (sizeof(szLogBuf) - 1)) {
            fprintf(pFd, szLogBuf);
        } else {
            fprintf(pFd, "%s\n", szLogBuf);
        }
        fflush(pFd);

        if (pFd != stdout) fclose(pFd);
    }

UNLOCK:
    debug_log_spinlock_unlock(&g_debug_log_lock);
}

