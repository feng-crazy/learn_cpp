/******************************************************************************
版权所有：深圳市理邦精密仪器有限公司
项目名称：LT
版本号：	1.0
文件名：	DebugCtrl.h
生成日期：2017.07.30
作者：	何登锋
功能说明：
	定义一些调试使用的宏，它们都受到NDEBUG编译开关控制。
******************************************************************************/
#ifndef _DEBUG_CTRL_H
#define _DEBUG_CTRL_H
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifndef NDEBUG

#define dbg \
	printf("%s(line %d): %s()\t$$$$$$$$$$$$$$$$$$$$\n", basename(__FILE__), __LINE__, __FUNCTION__);

#define lt_debug(args...) \
do { \
	flockfile(stdout); \
	printf("%s(line %d): %s():\t", basename(__FILE__), __LINE__, __FUNCTION__); \
	printf(args); \
	funlockfile(stdout); \
} while(false)

#define IF(arg) \
	if(arg) \
		printf(">>>!! ASSERT was hitted in %s(line %d) !!\n", \
				basename(__FILE__), __LINE__); \
	if(arg)

#define out_str(type, buff, len) \
for(type i = 0; i < len; i++) \
{\
	printf("0x%x ", buff[i]); \
} \
printf("\t%s:%d\n", basename(__FILE__), __LINE__)
#else // NDEBUG

#define dbg
#define lt_debug(args...)
#define IF(arg) if(arg)
#define out_str(type, buff, len)
#endif // NDEBUG

#define warning(args...) \
do { \
	printf("%s(line %d): %s(): ", basename(__FILE__), __LINE__, __FUNCTION__); \
	printf(args); \
} while(false)

// 打印当前线程的函数栈
void stack_trace(const char *ptr = NULL);
void log_err_to_file(const char *log, FILE *log_file);
void log_dbg_info(const char *log);

#endif // _DEBUG_CTRL_H
