/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/

#ifndef CONSOLE_LOG_H_
#define CONSOLE_LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "helper_functions.h" // To find and print level

void log_handler ( log_level_t level, const char* format, ... );

/**
 * @brief Macros to check invalid reference and debug trace to logger.
 */
#define CONSOLE_ERR_CHK_NULL_RET(ptr) {if(!ptr){\
    CONSOLE_MSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
    return E_FALSE;}\
}
#define CONSOLE_ERR_CHK_NULL_RET_NONE(ptr) {if(!ptr){\
    CONSOLE_MSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
    return ;}\
}
#define CONSOLE_ERR_CHK_NULL_RET_VAL(ptr, val) {if(!ptr){\
    CONSOLE_MSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
    return val;}\
}

/**
 * @brief Macros to check conditions and debug trace to logger.
 */
#define CONSOLE_ASSERT_COND_RET(ret, dbgStr) {if(!ret){\
    CONSOLE_MSG(ZONE_ERROR, "%s",dbgStr);\
    return E_FALSE;}\
}

/**
 * @brief Macros to print on console
 */
#define CONSOLE_MSG(level, ...) do {\
    if ((level)<= MAX_CONSOLE_LEVEL)\
    {\
      log_handler((level),__VA_ARGS__);\
    }\
} while(0)

#endif /* CONSOLE_LOG_H_ */
