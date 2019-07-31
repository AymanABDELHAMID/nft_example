/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
#ifndef HELPERMACROS_H_
#define HELPERMACROS_H_

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Macros to check invalid reference and debug trace to logger.
 */
#define ERR_CHK_NULL_RET(ptr) {if(!ptr){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
                return E_FALSE;}\
              }
#define ERR_CHK_NULL_RET_NONE(ptr) {if(!ptr){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
                return ;}\
              }
#define ERR_CHK_NULL_RET2_NONE(ptr, dbgstr) {if(!ptr){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference, %s", __FILE__, __FUNCTION__, __LINE__, dbgstr);\
                return ;}\
              }
#define ERR_CHK_NULL_RET_VAL(ptr, val) {if(!ptr){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference", __FILE__, __FUNCTION__, __LINE__);\
                return val;}\
              }
#define ERR_CHK_NULL_RET2(ptr, dbgStr) {if(!ptr){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] Invalid reference, %s", __FILE__, __FUNCTION__, __LINE__, dbgStr);\
                return E_FALSE;}\
              }

/**
 * @brief Macros to check conditions and debug trace to logger.
 */
#define ASSERT_COND_RET(ret, dbgStr) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s",dbgStr);\
                return E_FALSE;}\
              }
#define ASSERT_COND_RET2(ret, dbgStr1, dbgStr2) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s <%s>;",dbgStr1, dbgStr2);\
                return E_FALSE;}\
              }
#define ASSERT_COND_RET_VAL(ret, dbgStr, val) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s",dbgStr);\
                return val;}\
              }
#define ASSERT_COND_RET_NONE(ret, dbgStr) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s",dbgStr);\
                return;}\
              }
#define ASSERT_COND_LOG(ret, dbgStr) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s",dbgStr);}\
              }
#define ASSERT_COND_LOG2(ret, dbgStr1, dbgStr2) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s <%s>;",dbgStr1, dbgStr2);}\
              }
#define ASSERT_COND_RET_CLEANUP(ret, dbgStr, clean_up_code) {if(!ret){\
                RETAILMSG(ZONE_ERROR, "%s",dbgStr);\
                {clean_up_code};\
                return E_FALSE;}\
              }

/**
 * @brief Macros to check value out of range and debug trace to logger.
 */
#define ERR_CHK_VAL_IN_RANGE(val, min, max, dbgStr) {if(!((val>min)&&(val<max))){\
                RETAILMSG(ZONE_ERROR, "[%s][%s()][%d] %s value<%d> out of range[%d,%d]", __FILE__, __FUNCTION__, __LINE__, dbgStr, val, min, max);\
                return E_FALSE;}\
              }
#define ERR_CHK_IS_VAL_IN_RANGE(val, min, max) ((val>min)&&(val<max))


/**
 * @brief Macros for color output.
 */
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR_BOLD_BLACK         "\033[1m\033[30m"
#define ANSI_COLOR_BOLD_RED           "\033[1m\033[31m"
#define ANSI_COLOR_BOLD_GREEN         "\033[1m\033[32m"
#define ANSI_COLOR_BOLD_YELLOW        "\033[1m\033[33m"
#define ANSI_COLOR_BOLD_BLUE          "\033[1m\033[34m"
#define ANSI_COLOR_BOLD_MAGENTA       "\033[1m\033[35m"
#define ANSI_COLOR_BOLD_CYAN          "\033[1m\033[36m"
#define ANSI_COLOR_BOLD_WHITE         "\033[1m\033[37m"

#endif /* HELPERMACROS_H_ */
