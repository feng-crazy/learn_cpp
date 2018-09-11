/*****************************************************************************
	  COPYRIGHT: 2017 LingTrack Instruments,	Inc. All rights	reserved
	PROJECTNAME: LT
		VERSION: 1.0
	   FILENAME: Misc.h
		CREATED: 2017-04-09
		 作者: 何登锋
	功能描述:整个Mx共用的一些函数方法，可以在其他类中直接使用这些方法
	相当于定义了一组全局函数.
	MODIFICATION:何登锋.2017.1.28
	将misc.h拆分为misc.h与misc.cpp两个文件，这样做的目的是为了防止与其他目录的交叉
	include，在misc.h中只存放函数的声明(模板函数除外),在misc.cpp中对声明的函索实现
******************************************************************************/
#include "Utility/Misc.h"
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<dirent.h>
#include<mntent.h>
#include<sys/vfs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <iconv.h>
#include <cstring>
#include <errno.h>

using namespace std;



/******************************************************************************
作者: 何登锋, 2017.01.28
功能描述:
	得到浮点数小数精度的位数.
参数说明:
	value:这个类型的值
返回值:
	精度的位数
******************************************************************************/
unsigned int calc_float_bits(float value)
{
	char array[16]; // 最长为数是16位。

	snprintf(array, sizeof(array),"%.4f", value); // 最多支持4位小数。

	string str(array);

	// 找最后一个非0的位置
	int last = str.find_last_not_of('0', str.length()-1);

	// 找小数点出现的位置
	int dot = str.find_first_of('.', 0);
	return (last - dot);
}


/******************************************************************************
作者: 何登锋, 2017.06.09
功能描述:
	将一个字串中的某个字符替换成其他字符。例如:
	src = RESP|PLETH|CO2, old_char = '|', new_char = ' '
	则, src = RESP PLETH CO2
参数说明:
	src: 原始字串。
	old_char: 被替换的字符。
	new_char: 使用该字符替换old_char。
返回值:
******************************************************************************/
void replace_char(string &src, char old_char, const char new_char)
{
	// 找到第一个被替换字符的位置
	size_t pos = src.find(old_char);

	// 下面依次替换字符。
	while(pos != string::npos)
	{
		// string的标准方法，请查阅string的文档。
		src.replace(pos, 1, 1, new_char);

		// 找下一个被替换字符。
		pos = src.find(old_char);
	}
}


/******************************************************************************
作者: 何登锋, 2017.06.03
功能描述:
	从一个字符串中按照一个分隔符提取出指定位置的字串。例如:
	src = "RESP|PLETH|CO2", separator = "|" index = "1", so, dest = PLETH
参数说明:
	src: 原始字串。
	dest: 处理完毕后的字串。
	separator: 指定的分隔符。
	index: 字串的所处的位置。
返回值:
******************************************************************************/
bool distill_string(const string &src, string &dest,
		const string &separator, unsigned index)
{
	size_t start_pos = 0;

	for(size_t i=0; i<index; i++)
	{
		// 从start_pos开始寻找separator，并返回位置。
		start_pos = src.find(separator, start_pos);

		// 没有找到，则返回false。
		if(start_pos == string::npos)
		{
			return false;
		}

		// 移动start_pos。
		start_pos += separator.size();

		// start_pos移出整个原始字串，返回false。这时表明index指定太大。
		if(start_pos >= src.size())
		{
			return false;
		}
	}

	// end_pos定位在start_pos后的第一个位置，也即start_pos与end_pos之间的位置
	// 就是我们需要的字串。
	size_t end_pos = src.find(separator, start_pos);

	// 如果start_pos就是最后一个separator后的第一个字符位置。也即index是最后
	// 字串，如该方法头注释中index = 2的情况。
	if(end_pos == string::npos)
	{
		dest = src.substr(start_pos);
		return true;
	}
	else // 取出需要的字串。
	{
		dest = src.substr(start_pos, end_pos - start_pos);
		return true;
	}
}

/******************************************************************************
作者:     何登锋, 2017.05.20
功能描述: 去掉一个字符串的首尾空字符
参数说明:
返回值:
		  返回一个去掉首尾空字符的字符串
******************************************************************************/
string trim(string &s)
{
	const string &space = " \f\n\t\r\v";
	string r = s.erase(s.find_last_not_of(space) + 1);
	return r.erase(0, r.find_first_not_of(space));
}

/******************************************************************************
作者:     何登锋, 2017.05.20
功能描述: 去掉一个字符串的首空字符
参数说明:
返回值:
		  返回一个去掉首空字符的字符串
******************************************************************************/
string ltrim(string &s)
{
	const string &space = " \f\n\t\r\v";
	return s.erase(0, s.find_first_not_of(space));
}

/******************************************************************************
作者:     何登锋, 2017.05.20
功能描述: 去掉一个字符串的尾空字符
参数说明:
返回值:
		  返回一个去掉尾空字符的字符串
******************************************************************************/
string rtrim(string &s)
{
	const string &space = " \f\n\t\r\v";
	return s.erase(0, s.find_last_not_of(space) + 1);
}


/******************************************************************************
作者: 何登锋, 20170817
功能描述:
	获取当前最新日期 年、月、日的函数接口，方便与时间显示相关的操作

参数说明:
	year：获取当前的年份
	month：获取当前的月份
	day:获取当前的日期
返回值:
	无
******************************************************************************/
void get_current_localtime(int &year, int &month, int &day)
{
	time_t timep = time(NULL);
	struct tm *local_time = localtime(&timep);

	if(local_time != NULL)
	{
		year = 1900+local_time->tm_year;
		month = 1+local_time->tm_mon;
		day = local_time->tm_mday;
	}
}


/******************************************************************************
作者: 何登锋, 20170910
功能描述:
	上一个函数的重载版本，可以获取到年、月、日、时、分、秒的精度.

参数说明:
	year：获取当前的年份
	month：获取当前的月份
	day:获取当前的日期
	hour:获取当前的时
	min：获取当前的分
	sec：获取当前的秒
返回值:
	无
******************************************************************************/
void get_current_localtime(int &year, int &month, int &day,
		int &hour, int &min, int &sec)
{
	time_t timep = time(NULL);
	struct tm *local_time = localtime(&timep);

	if(local_time != NULL)
	{
		year = 1900+local_time->tm_year;
		month = 1+local_time->tm_mon;//+1是因为月份下标从0－11。
		day = local_time->tm_mday;
		hour = local_time->tm_hour;
		min = local_time->tm_min;
		sec = local_time->tm_sec;
	}
}


/*****************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	按指定的精度保留浮点数，精度以外的小数四舍五入，如：
		round_float(3.141592, 0) = 3
		round_float(3.141592, 1) = 3.1
		round_float(3.141592, 2) = 3.14
		round_float(3.141592, 3) = 3.142
		round_float(3.141592, 4) = 3.1416
		round_float(3.141592, 5) = 3.14159
		round_float(3.141592, 6) = 3.141592
		round_float(-3.141592, 3) = -3.142
		round_float(2.5, 0) = 3
		round_float(3.5, 0) = 4
参数说明:
	value: 单精度浮点数，注意其最大精度为小数点后6位
	precision: 小数位数
返回值:
	按指定精度转换后的浮点数
******************************************************************************/
float round_float(float value, int precision)
{
	float n = powf(10, precision);
	return (roundf(value * n) / n);
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	按指定的精度对浮点数进行转换，使得本应进位，但因内部存储而无法正常进位的浮点数正常进位。
	此外，其他数据正常进位。如：
	round_float(2.5, 0) = 3
	round_float(3.5, 0) = 4
	round_float(3.141592, 2) = 3.14
	enforce_round_float(79.0499, 1) = 79.1
	enforce_round_float(79.0501, 1) = 79.1
参数说明:
	value: 单精度浮点数，注意其最大精度为小数点后6位
	precision: 小数位数
返回值:
	按指定精度初步转换后的浮点数
******************************************************************************/
float enforce_round_float(float value, int precision)
{
	float n = powf(10, precision);
	if((value * n - int(value * n) - 0.5 <= 0) && (value * n - int(value * n)
			- 0.5 >= -0.01))
	{
		return (roundf(int(value * n) + 0.5) / n);
	}
	else
	{
		return (roundf(value * n) / n);
	}
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数是否为0
参数说明:
	value: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示零, false表示非零
******************************************************************************/
bool is_zero(float value, int precision)
{
	float n = powf(10, precision);
	return (lroundf(value * n) == 0);
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否大于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a大于b, false表示a小于或等于b
******************************************************************************/
bool is_greater(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) > lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否大于或等于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a大于或等于b, false表示a小于b
******************************************************************************/
bool is_greater_equal(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) >= lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否小于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a小于b, false表示a大于或等于b
******************************************************************************/
bool is_less(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) < lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否小于于或等于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a小于或等于b, false表示a大于b
******************************************************************************/
bool is_less_equal(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) <= lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否等于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a等于b, false表示a不等于b
******************************************************************************/
bool is_equal(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) == lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.05.24
功能描述:
	在指定的精度下，判断浮点数a是否不等于b
参数说明:
	a: 待判断的浮点数
	b: 待判断的浮点数
	precision: 小数位数
返回值:
	true表示a不等于b, false表示a等于b
******************************************************************************/
bool is_not_equal(float a, float b, int precision)
{
	float n = powf(10, precision);
	return (lroundf(a * n) != lroundf(b * n));
}

/******************************************************************************
作者: 何登锋, 2017.07.15
功能描述:
	获取一个文件的体积，单位为Byte
参数说明:
	file_full_name: 带路径的文件名
返回值:
	>0: 返回文件的体积，单位为Byte.
	-1: 文件读取出错
******************************************************************************/
int file_size(const string &file_full_name)
{
	struct stat file_stat;
	if(stat(file_full_name.c_str(), &file_stat) < 0)
	{
		return -1;
	}

	return file_stat.st_size;
}

/******************************************************************************
作者: 何登锋, 2017.12.27
功能描述:
	获取两个时间的微秒差
参数说明:
	time_1:被减数
	time_2:减数
返回值:
	返回time_1与time_2的微秒差,如果相差超过30秒，则返回30秒的微秒数
******************************************************************************/
int udifftime(timeval time_1, timeval time_2)
{
	int udif_time;

	if(time_1.tv_sec - time_2.tv_sec >= 30)
	{
		return 30000000;
	}

	if(time_1.tv_sec - time_2.tv_sec <= 30)
	{
		return -30000000;
	}

	udif_time = (time_1.tv_sec - time_2.tv_sec) * 1000000;
	udif_time += time_1.tv_usec - time_2.tv_usec;

	return udif_time;
}

/******************************************************************************
作者:何登锋, 2017.02.22
功能描述:
	判断是否是闰年
参数说明:
	year:年
	time_2:减数
返回值:
	是返回TRUE 否则返回FALSE
******************************************************************************/
bool is_leap_year(int year)
{
	if(year <0)
	{
		return false;
	}

	return ((year%4 == 0 && year%100 != 0) || (year%400 == 0));
}



/******************************************************************************
作者: 何登锋, 2017.06.04
功能描述:
	将UTF-8编码转换为UTF-16编码
参数说明:
	sourcebuf:待转换的UTF-8编码缓存区
	sourcelen:用以记录还未转换的字符数
	destbuf:转换为UTF-16编码的输出缓存区
	destlen:用以记录输出缓存的剩余空间
返回值:
	>0: 转换成功
	-1: 转换出错
******************************************************************************/
bool utf8_to_utf16(char *sourcebuf, size_t sourcelen,
		char *destbuf, size_t destlen)
{
	iconv_t cd;

	errno = 0;
	if((cd = iconv_open("UTF-16BE", "UTF-8")) < 0)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	memset(destbuf, 0, destlen);
	char **source = &sourcebuf;
	char **dest = &destbuf;

	if(0 > iconv(cd, source, &sourcelen, dest, &destlen))
	{
		return -1;
	}
	iconv_close(cd);
	return 0;
}


/******************************************************************************
作者: 何登锋 2017-08-09
功能描述:
	更改硬件时间和内核时间
参数说明:
	要更新的时间差
返回值:
	更改时间是否成功
******************************************************************************/
bool update_hardwave_kernel_time(long diff_time)
{
	int fd, ret;
	struct rtc_time rtc_tm;
	fd = open("/dev/rtc", O_RDWR);

	if(-1 == fd)
	{
		fd = open("/dev/rtc0", O_RDWR);
		if(-1 == fd)
		{
			printf("open hardware time device failed\n");
			return false;
		}
	}

	time_t time_new = time(NULL) + diff_time;
	struct tm tm_new = *(gmtime(&time_new));
	rtc_tm.tm_year = tm_new.tm_year;
	rtc_tm.tm_mon = tm_new.tm_mon;
	rtc_tm.tm_mday = tm_new.tm_mday;
	rtc_tm.tm_hour = tm_new.tm_hour;
	rtc_tm.tm_min = tm_new.tm_min;
	rtc_tm.tm_sec = tm_new.tm_sec;

	// 设置硬件时间
	ret = ioctl(fd, RTC_SET_TIME, &rtc_tm);
	if(-1 == ret)
	{
printf("set hardware time failed\n");
		close(fd);
		return false;
	}
	close(fd);

	// 设置内核时间
	stime(&time_new);

	return true;
}


int min_for_int(int i1, int i2)
{
	return i1 <= i2 ? i1 : i2;
}
