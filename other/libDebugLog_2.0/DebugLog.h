/******************************************************************************

            版权所有 (C), 2016-2017, 西安交大捷普网络科技有限公司

 ******************************************************************************
    文 件 名 : DebugLog.h
    版 本 号 : V1.0
    作    者 : zhangcan
    生成日期 : 2017年12月6日
    功能描述 : DebugLog头文件，主要用于打印日志和记录日志作用，
               同时支持Windows、Linux、Unix系统
			   以下是对外公开接口：
                1、DEBUG_INIT      --- 日志初始化
                2、DEBUG_SET_MODE  --- 设置日志模式
                3、DEBUG_SET_LEN   --- 设置日志文件大小
                4、DEBUG_LOG       --- 记录打印日志
                5、DEBUG_ASSERT    --- 记录打印断言
    修改历史 :
******************************************************************************/
#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 日志打印模式，默认为 2 */
enum {
    DEBUG_LOG_MODE_STDOUT   = 1, //标准输出
    DEBUG_LOG_MODE_FILE     = 2, //记录文件
    DEBUG_LOG_MODE_DISABLE  = 3, //关闭日志
    DEBUG_LOG_MODE_MAX,
};

/********************************************************************/
// 以下为内部使用接口
int debug_log_init(const char *name, const char *file);
void debug_log_set_mode(int num, int mode);
int debug_log_get_mode(int num);
void debug_log_set_file(int num, int length);
void debug_log_print(int num, const char *function,
    unsigned int line, const char *format, ...);
/********************************************************************/


/* 将s转换成字符串宏 */
#define DEBUG_TOSTRING(s)		#s


/********************************************************************/
// 以下对外提供DEBUG相关宏
// eg:
//	    ......
//      int log_num;
//      log_num = DEBUG_INIT(test_log_mod, "./test.log");
//      ......
//      DEBUG_LOG(log_num, "Test debug_log_mod!\n");
//      DEBUG_LOG(log_num, "Test debug_log_mod id = %d\n", log_num);
//      ......

/* DEBUG初始化 */
#define DEBUG_INIT(_log_mod, _log_file) ({ \
    int _log_num = debug_log_init(DEBUG_TOSTRING(_log_mod), (_log_file)); \
    (_log_num); \
})

/* 设置日志模式，见上面枚举值，默认为: DEBUG_LOG_MODE_FILE -- 记录文件 */
#define DEBUG_SET_MODE(_log_num, _log_mode) do { \
    debug_log_set_mode((_log_num), (_log_mode)); \
} while (0)

/* 设置日志文件大小，默认10Mb(单位:Mb) */
#define DEBUG_SET_LENG(_log_num, _log_length) do { \
    debug_log_set_file((_log_num), (_log_length)); \
} while (0)

/* 获取日志模式 */
#define DEBUG_GET_MODE(_log_num) ({ \
    int _mode = debug_log_get_mode((_log_num)); \
    (_mode); \
})

/* 记录打印日志 */
#define DEBUG_LOG(_log_num, ...) do { \
    if (debug_log_get_mode((_log_num)) != DEBUG_LOG_MODE_DISABLE) \
        debug_log_print((_log_num), __FUNCTION__, __LINE__,  __VA_ARGS__); \
} while (0)

/* 记录打印断言 */
#define DEBUG_ASSERT(_log_num, _s) do { \
    if ((!(_s)) && (debug_log_get_mode((_log_num)) != DEBUG_LOG_MODE_DISABLE)) { \
        debug_log_print((_log_num), __FUNCTION__, __LINE__, \
            "assert '%s'!\n", DEBUG_TOSTRING(_s)); \
    } \
} while (0)
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif

