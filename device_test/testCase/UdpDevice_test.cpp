/*
 * UdpDevice_test.cpp
 *
 *  Created on: Sep 4, 2018
 *      Author: hdf_123
 */

#include "UdpDevice.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>


#include "LTCommon.h"
#include "LTConfig.h"
#include "LTUtilities.h"
#include "LTFrame.h"
#include "LTCircularBuffer.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

int main()
{
	std::shared_ptr<spdlog::logger> logger_ = spd::rotating_logger_mt("logger_", "udp_dev.log", 1048576 * 5, 3);
	spd::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l][thread %t] %v ");
	spd::set_level(spd::level::level_enum(5));
	logger_->flush_on(spd::level::err);

	UdpDevice *udp_dev = new UdpDevice(logger_);

	if(!udp_dev->begin())
	{
		printf("udp_dev init failed\n");
		delete udp_dev;
		return 0;
	}

	struct timeval ts, td;
	gettimeofday (&ts , NULL);

	CircularBuffer<unsigned char> queue_buff(4096);     // 缓存从端口读取的数据。

	while(true)
	{
//		std::this_thread::sleep_for(std::chrono::microseconds(100));
		unsigned char buff[2048] = {0x00};
		int read_len = -1;

//		gettimeofday (&td , NULL);
//		int diff_time = (td.tv_sec - ts.tv_sec) * 1000 + (td.tv_usec - ts.tv_usec)/1000;
//		printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>diff_time = %d ms \n",diff_time);
//		ts = td;

		read_len = udp_dev->read(buff, sizeof(buff));
		printf("udp read-------len = %d------------\n",read_len);
		for(int i=0;i<read_len;i++)
		{
			printf("%02x ",buff[i]);
		}
		printf("udp read ------------------end\n");

		if(read_len > 0)
		{
//			printf("queue_buff.size = %d  write spi_read_len=%d\n",queue_buff.size(),read_len);
			queue_buff.push_back_block(buff, read_len);
		}

		//循环直到队列等于包头标示到包长位置的字节数，防止包头到长度部分被截断，queue_buff始终会保留该字节数
		while(queue_buff.size() > Frame::FRAME_HEAD_PACKET_LEN_SIZE)
		{
			Frame::FrameType ftype = Frame::Invalid;
			int lastSeq = -1;

			clock_t lastOpenTime = 0;
			ftype = Frame::GetType(queue_buff, queue_buff.size());
			//说明找到了包头
			if (ftype == Frame::Invalid)
			{
				//弹出一个字节继续寻找包头
				queue_buff.pop_front();
				continue;
			}

			unsigned int packet_len = Frame::GetLength(queue_buff, queue_buff.size());
//			printf("packet_len = %d\n",packet_len);
			//如果帧长大于最大帧的，说明长度出错
			if(packet_len > Frame::maxSize||packet_len<Frame::FRAME_HEAD_MIN_LEN)
			{
				printf("packet_len > Frame::maxSize is error :%d\n",packet_len);
				queue_buff.pop_front();
				continue;
			}

//			printf("packet_len = %d ,queue_buff.size():%d\n",packet_len,queue_buff.size());
			//如果包长大于队列的长度，说明该帧数据不完整，需要下一次读取拼包,所以此处break
			if(packet_len>queue_buff.size())
			{
//				printf("packet_len > size need agin read :%d\n",packet_len);
				break;
			}

			//判断校验和
			if(!Frame::judge_check_sum(queue_buff,packet_len))
			{
				printf("\njudge_check_sum is failed packet_len=%d\n",packet_len);
				for(int i=0;i<queue_buff.size();i++)
				{
					printf("%02x ",queue_buff[i]);
				}
				printf("\n judge_check_sum is failed word end\n");
				queue_buff.pop_front();
				continue;
			}
			else
			{

				//数据包正确，拷贝出该数据包并进行解析
				unsigned char data_buff[Frame::maxSize] = {0x00};
				queue_buff.pop_front_block(packet_len, data_buff);
				printf("judge_check_sum is succes\n");
//				for(int i=0;i<packet_len;i++)
//				{
//					printf("%02x ",data_buff[i]);
//				}
//				printf("\n");

				if(ftype == Frame::Optic)
				{
					int sensorId = OpticFrame::CheckSensorId(data_buff, packet_len);
					int seq = OpticFrame::CheckSeq(data_buff, packet_len);
					static int old_optic_seq[8] = {0x00};
					int diff_seq = seq - old_optic_seq[sensorId];
					if(diff_seq != 1)
					{
						printf("Frame::Optic seq = %d old_optic_seq[%d] = %d diff_seq = %d\n"
								,seq,sensorId,old_optic_seq[sensorId],diff_seq);
					}
					old_optic_seq[sensorId] = seq;

				}
				else if(ftype == Frame::Groove)
				{
					int sensorId = GrooveFrame::CheckSensorId(data_buff, packet_len);
					int seq = GrooveFrame::CheckSeq(data_buff, packet_len);
					static int old_optic_seq[8] = {0x00};
					int diff_seq = seq - old_optic_seq[sensorId];
					if(diff_seq != 1)
					{
						printf("Frame::Optic seq = %d old_optic_seq[%d] = %d diff_seq = %d\n"
								,seq,sensorId,old_optic_seq[sensorId],diff_seq);
					}
					old_optic_seq[sensorId] = seq;

				}
				else if (ftype == Frame::IMU)
				{
					int seq = Frame::CheckSeq(data_buff, packet_len);
					static int old_imu_seq = 0;
					int diff_seq = seq - old_imu_seq;
					if(diff_seq != 1)
					{
						printf("Frame::IMU seq = %d old_imu_seq = %d diff_seq = %d\n",seq,old_imu_seq,diff_seq);
					}
					old_imu_seq = seq;
				}
//				((data_buff[packet_len-10]<<40) | (data_buff[packet_len-9]<<32) |
//				printf("data_buff[packet_len - 6] = %02x ,data_buff[packet_len - 5] = %02x\n",data_buff[packet_len - 6],data_buff[packet_len - 5]);
//				long long ts_ = (long long) ((data_buff[packet_len-10]<<40) | (data_buff[packet_len-9]<<32) |(data_buff[packet_len-8]<<24) | (data_buff[packet_len-7]<<16) | (data_buff[packet_len-6]<<8) | data_buff[packet_len-5]);
//				printf(">>>>>ts_ = %lld  %x\n",ts_,ts_);
//				printf("(data_buff[packet_len-6]<<8) = %x\n",(data_buff[packet_len-6]<<8));
			}
		}


//		usleep(200);

	}
}
