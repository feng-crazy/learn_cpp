/******************************************************************************
项目名称：	Mx
版本号：		1.0
文件名：		KeyboardFbdump.cpp
生成日期：	2015-9-25
作者：		何登锋
功能说明：
	该功能用于在键盘截图，在所有的平台上作为一个独立的进程，接上USB键盘和U盘，
	此程序通过获取键值，判断截屏按下，使用
	第三方程序fbdump截图。

******************************************************************************/


#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <termios.h>
#include "sys/statfs.h"
#include "linux/input.h"

#include <string.h>
#include <string>

//时间结构体
typedef struct
{
	char second;
	char minute;
	char hour;
	char day;
	char month;
	short year;
}DATE_TIME;


/******************************************************************************
AUTHOR: 何登锋	, 2015.09.25
DESCRIPTION:	获取当前系统的时间用来命名图片
PARAMETERS:
RETURN:
******************************************************************************/
void get_time(DATE_TIME *datetime)
{
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	datetime->year = tm->tm_year + 1900;
	datetime->month = tm->tm_mon + 1;
	datetime->day = tm->tm_mday;
	datetime->hour = tm->tm_hour;
	datetime->minute = tm->tm_min;
	datetime->second = tm->tm_sec;


}


/******************************************************************************
AUTHOR: 何登锋	, 2015.09.25
DESCRIPTION:	创建文件夹
PARAMETERS:
RETURN:
******************************************************************************/
int create_folder(char *directory)
{
	char dire[50];
	memset(dire,0x00,sizeof(dire));
	strcpy(dire,directory);
	if(mkdir(dire,0777)== -1)
	{
		return -1;
	}
	return 0;
}

/******************************************************************************
AUTHOR: 何登锋	, 2015.09.25
DESCRIPTION:	重新封装system函数
PARAMETERS:
RETURN:
******************************************************************************/
int mw_system(const char *cmd)
{
	int status = 0;
	pid_t pid;

	if((pid = vfork()) < 0)
	{
		status = -1;
	}
	else if(pid == 0)
	{
		const char *new_argv[4];
		struct sigaction sa_cld;
		sa_cld.sa_handler = SIG_DFL;
		sa_cld.sa_flags = 0;

		/*在子进程中放开SIGINT信号*/
		sigemptyset(&sa_cld.sa_mask);
		sigaction(SIGINT,&sa_cld,NULL);
		sigaction(SIGQUIT,&sa_cld,NULL);
		sigaction(SIGCHLD,&sa_cld,NULL);

		new_argv[0] = "sh";
		new_argv[1] = "-c";
		new_argv[2] = cmd;
		new_argv[3] = NULL;



		if(execve("/bin/sh",(char *const *)new_argv,NULL) < 0)
		{
			//printf("fail to execve %s! errno \r\n",cmd);
			exit(1);
		}
		else
		{
			exit(0);
		}


	}
	else
	{
		waitpid(pid,&status,0);
	}
	return status;
}

/******************************************************************************
AUTHOR: 何登锋	, 2015.09.25
DESCRIPTION:	获取USB键盘的设备
PARAMETERS:
RETURN:
******************************************************************************/
bool get_keyboard_dev(char *keyboard_dev_name)
{
	const char *dir_name = "/dev/input";

	DIR *pdir = opendir(dir_name);
	if(NULL == pdir)
	{
		return false;
	}

	while(true)
	{

		struct dirent *ptr = readdir(pdir);
		if(NULL == ptr)
		{
			break;
		}

		if((0 == strcmp(ptr->d_name, ".")) || (0 == strcmp(ptr->d_name, "..")))
		{
			continue;
		}

		char dev_name[30];
		memset(dev_name,0,sizeof(dev_name));
		sprintf(dev_name,"%s/%s",dir_name,ptr->d_name);

		int fd = open(dev_name,O_RDONLY);

		if(fd < 0)
		{
			//printf("open dev  error dev_name is :%s\n",dev_name);
			continue;
		}
		else
		{
			//printf("%s:open, fd = %d\n",dev_name,fd);

			char get_dev_name[80] = "nokown";

			if(0<=(ioctl(fd,EVIOCGNAME(sizeof(get_dev_name)-1),get_dev_name)))
			{
				//printf("get_dev_name is  %s\n",get_dev_name);
				//如果设备为USB键盘
				if((strstr(get_dev_name,"Keyboard")!=NULL) && (strstr(get_dev_name,"USB")!=NULL))
				{
					int rep[2]={0,0};

					int flag = ioctl(fd,EVIOCGREP,rep);
					//printf("flag:%d,rep[0]:%d rep[1]:%d\n",flag,rep[0],rep[1]);

					if((flag<0)&&(rep[0]!=0)&&(rep[1]!=0))//说明此设备存在副本，中断查找不再继续
					{
						strcpy(keyboard_dev_name,dev_name);
						close(fd);
						break;
					}
					else//可能此设备是副本，继续查找看是否存在同样的设备
					{
						strcpy(keyboard_dev_name,dev_name);
						close(fd);
					}

				}

			}
			else
			{

			}

			close(fd);

		}
	}

	closedir(pdir);


}

#define PIC_DIR "pics"//图片保存路径

/******************************************************************************
AUTHOR: 何登锋	, 2015.09.25
DESCRIPTION:	主函数
PARAMETERS:
RETURN:
******************************************************************************/
int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("input param option is not enough,please input --help\n");
		return -1;
	}

	if(!strcmp(argv[1],"--help"))
	{
		printf("option is: keytest [version] [u disk path]\n");
		return 0;
	}

	char versions[10];
	memset(versions,0x00,sizeof(versions));
	strcpy(versions,argv[1]);

	char u_disk_path[100];
	memset(u_disk_path,0x00,sizeof(u_disk_path));
	strcpy(u_disk_path,argv[2]);

	if(0 > access(u_disk_path,F_OK))
	{
		//printf("U disk dir not enter! U disk is ready?\n");
		return -1;
	}

	char dir_name[30];

	sprintf(dir_name,"%s/%s",u_disk_path,PIC_DIR);

	////printf("dir_name:%s\n",dir_name);

	if(0 > access(dir_name,F_OK))
	{
		create_folder(dir_name);
		if(0 > access(dir_name,W_OK))
		{
			chmod(dir_name,0777);
		}
	}


	struct input_event key_ev;

	char dev_name[80] = "nokown";

	int get_flag = get_keyboard_dev(dev_name);

	if((get_flag==false) &&(strcmp(dev_name,"nokown")==0))
	{
		printf("not have keyboard dev!\n");
		return -1;
	}

	int fd = open(dev_name,O_RDWR|O_NONBLOCK);
	if(fd < 0)
	{
		//printf("open dev  error dev_name is :%s\n",dev_name);
		return -1;
	}
	else
	{
		//printf("open dev  success dev_name is :%s\n",dev_name);
		struct termios tio;
		tcgetattr(fd,&tio);
	}



	int press_key_flag = false;

	int cut_pic_flag = false;

	while(true)
	{
		int len;

		static fd_set fs_read;
		static struct timeval tv_timeout;

		FD_ZERO(&fs_read);
		FD_SET(fd,&fs_read);
		tv_timeout.tv_sec = 0;
		tv_timeout.tv_usec = 10000;

		int select_read_flag = select(fd+1,&fs_read,NULL,NULL,&tv_timeout);

		if(select_read_flag >0 )
		{
			len = read(fd,&key_ev,sizeof(struct input_event));
		}
		else
		{
			continue;
		}

		//判断键值，是否截屏键按下
		if(len>0)
		{
			printf("type:%d,code:%d.value:%d\n",key_ev.type,key_ev.code,key_ev.value);


			switch(key_ev.type)
			{
				case EV_KEY:
					if(key_ev.code == KEY_SYSRQ)//按下截图键
					{
						if(key_ev.value == 1)//按下
						{
							press_key_flag = true;
						}
						else if(key_ev.value == 0)//松开
						{
							if(press_key_flag == true)
							{
								cut_pic_flag = true;
							}
						}
					}
					break;

				default:
					break;
			}
		}
		else
		{
			//printf("read keyborad len is 0:\n");
			continue;
		}

		//如果截屏键按下，执行截屏
		if(cut_pic_flag == true)
		{
			//struct tm *tm;
			//tick = time((time_t*)NULL);

			char cmd[200];

			char *exe_name = "home/edan/bin/fbdump";//第三方截图程序fbdump在/home/edan/bin
			char *fb_dev = "/dev/fb0";
			char pic_name[100];

			//此处存在歧义，测试发现只有老主板的IMX53用的是fb2，V5,V6即有老主板有新主板，
			//而im系列用的是fb0。所以淘汰老主板统一为fb0
//			if(!strcmp(versions,"V5") ||!strcmp(versions,"V6") )
//			{
//				fb_dev = "/dev/fb2";
//			}
//			else
//			{
//				fb_dev = "/dev/fb0";
//			}

			DATE_TIME time;
			get_time(&time);

			sprintf(pic_name,"%d%02d%02d%02d%02d%02d%s.ppm",
					time.year,time.month,time.day,
					time.hour,time.minute,time.second,versions);


			sprintf(cmd,"%s -fb %s > %s/%s",exe_name,fb_dev,dir_name,pic_name);

			printf("cmd:  %s\n",cmd);

			int stutas = mw_system(cmd);//执行截图
			if(stutas<0)
			{
				printf("fail mw_system error is :%s\n",strerror(errno));

			}

			press_key_flag = false;

			cut_pic_flag = false;
		}


	}

	close(fd);
	return 0;

}





