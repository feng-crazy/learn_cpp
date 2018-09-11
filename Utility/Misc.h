/*****************************************************************************
	  COPYRIGHT: 2017 LingTrack Instruments,	Inc. All rights	reserved
	PROJECTNAME: LT
		VERSION: 1.0
	   FILENAME: Misc.h
		CREATED: 2017-04-09
		 作者: 何登锋
	功能描述:整个LT共用的一些函数方法，可以在其他类中直接使用这些方法
	相当于定义了一组全局函数.
	MODIFICATION:何登锋.2017.1.28
	将misc.h拆分为misc.h与misc.cpp两个文件，这样做的目的是为了防止与其他目录的交叉
	include，在misc.h中只存放函数的声明(模板函数除外),在misc.cpp中对声明的函索实现
******************************************************************************/
#ifndef _MISC_H
#define _MISC_H

extern "C"
{
#include <sys/time.h>
#include <unistd.h>
}
#include <typeinfo>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// 得到单位的调节步长。
unsigned int get_float_bits(const std::string &unit);

// 得到浮点数小数精度的位数.
unsigned int calc_float_bits(float value);

// 获取当前最新日期 年、月、日的函数接口，方便与时间显示相关的操作
void get_current_localtime(int &year, int &month, int &day);

// 上一个函数的重载版本，可以获取到年、月、日、时、分、秒的精度.
void get_current_localtime(int &year, int &month, int &day,
		int &hour, int &min, int &sec);

// 根据配置文件设置的年月日显示时间格式，将时间转化为相应格式的字符串.
string get_formated_time_str(const time_t &src_time, bool add_h_m_s);

// 将一个字串中的某个字符替换成其他字符
void replace_char(string &src, char old_char, const char new_char);

// 从一个字符串中按照一个分隔符提取出指定位置的字串。
bool distill_string(const string &src, string &dest,
		const string &separator, unsigned index);

// 去掉字一个符串的首尾空字符
string trim(string &s);

// 去掉一个字符串的首空字符
string ltrim(string &s);

// 去掉一个字符串的尾空字符
string rtrim(string &s);


// 获取一个文件的体积，单位为Byte
int file_size(const string &file_full_name);

// 获取时间的微秒差
int udifftime(timeval time_1, timeval time_2);

// 判断是否是闰年
bool is_leap_year(int year);

// 判断U判是否挂载上去
bool usb_disk_is_mount();

//将UTF-8编码转换为UTF-16编码
bool utf8_to_utf16(char *sourcebuf, size_t sourcelen,
		char *destbuf, size_t destlen);

// 更改硬件时间和内核时间
bool update_hardwave_kernel_time(long diff_time);

// 获取较小的值。
int min_for_int(int i1, int i2);

/******************************************************************************
作者: 何登锋, 2017.04.09
功能描述:
	将某类型值转换成C类型的字符串,可控制小数点后的显示小数精度.
参数说明:
	value:这个类型的值
	str:输出的字符串结果
	float_bit:小数点后输出的位数
返回值:
	当前正在处理的字符串。
	注意:str指向的数组空间需足够大防止溢出.
******************************************************************************/
template<typename T>
void to_char(T value, char str[], int float_bit = -1)
{
	stringstream ss;

	if((typeid(value) == typeid(char)) ||
			(typeid(value) == typeid(unsigned char)))
	{
		if(float_bit != -1)
		{
			ss << setiosflags(ios::fixed) <<
					setprecision(float_bit) << (int)value;
		}
		else
		{
			ss << (int)value;
		}
	}
	else
	{
		if(float_bit != -1)
		{
			ss << setiosflags(ios::fixed) << setprecision(float_bit) << value;
		}
		else
		{
			ss << value;
		}
	}

	strcpy(str, ss.str().c_str());
}

/******************************************************************************
作者: 何登锋
功能描述:将值转为string,可指定小数点精度.默认为0表示按照值的原样输出最多达6位有效
	数字.

参数说明:
	float_bit,小数点之后显示的位数
返回值:
Modified:何登锋,增加控制小数点精度功能.
******************************************************************************/
template<typename T>
void to_string(T value, string &str, int float_bit = -1)
{
	char str_format[20];
	char str_temp[32];

	if((typeid(value) == typeid(float) || typeid(value) == typeid(double)))
	{
		if(float_bit != -1)
		{
			snprintf(str_format,sizeof(str_format), "%%.%df", float_bit);
			snprintf(str_temp, sizeof(str_temp),str_format, value);
			str = str_temp;
		}
		// 如果不设定小数位数，则按原来的位数输出
		else
		{
			int float_bits;
			float_bits = calc_float_bits(value);
			snprintf(str_format,sizeof(str_format), "%%.%df", float_bits);
			snprintf(str_temp, sizeof(str_temp),str_format, value);
			str = str_temp;
		}
	}
	else
	{
		snprintf(str_temp, sizeof(str_temp),"%d", (int)value);
		str = str_temp;
	}
}

/******************************************************************************
作者: 何登锋
功能描述:将值转为string,可指定小数点精度.默认为0表示按照值的原样输出最多达6位有效
	数字.
参数说明:
	float_bit,小数点之后显示的位数
返回值:
Modified:何登锋,增加控制小数点精度功能.
******************************************************************************/
template<typename T>
string to_string(T value, int float_bit = -1, const char *fomat = "%d")
{
	string str;
	char str_format[20];
	char str_temp[32];

	if((typeid(value) == typeid(float) || typeid(value) == typeid(double)))
	{
		if(float_bit != -1)
		{
			snprintf(str_format,sizeof(str_format), "%%.%df", float_bit);
			snprintf(str_temp,sizeof(str_temp), str_format, value);
			str = str_temp;
		}
		// 如果不设定小数位数，则按原来的位数输出
		else
		{
			int float_bits;
			float_bits = calc_float_bits(value);
			snprintf(str_format, sizeof(str_format),"%%.%df", float_bits);
			snprintf(str_temp, sizeof(str_temp),str_format, value);
			str = str_temp;
		}
	}
	else
	{
		snprintf(str_temp, sizeof(str_temp),fomat, (int)value);
		str = str_temp;
	}
	return str;
}


// 精度控制， 精度外四舍五入，此外浮点数内部存储不是标准的0.5时，辅助进位
float enforce_round_float(float value, int precision);

// 精度控制, 精度外四舍五入
float round_float(float value, int precision);

// 单精度浮点数零判断
bool is_zero(float value, int precision = 5);

// 单精度浮点数逻辑运算, 详见Misc.cpp
bool is_greater(float a, float b, int precision);
bool is_greater_equal(float a, float b, int precision);
bool is_less(float a, float b, int precision);
bool is_less_equal(float a, float b, int precision);
bool is_equal(float a, float b, int precision);
bool is_not_equal(float a, float b, int precision);

#endif
