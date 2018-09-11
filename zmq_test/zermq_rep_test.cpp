/*
 * zermq_rep_test.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: hdf_123
 */
#include <zmq.h>
#include <assert.h>
#include <string>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "MyZmq.h"

using namespace std;
using namespace  LTUtilities;

int main()
{
	string port = "6113";

	//使用zeromq发送文件流，创建zmq的上下文
	void *zmqContext_ = zmq_ctx_new();
	assert(zmqContext_);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *rep_socket = zmq_socket(zmqContext_, ZMQ_REP);
	assert(rep_socket);

	int rc = zmq_bind(rep_socket, ("tcp://*:" + port).c_str());
	assert(rc == 0);

	char *buffer;

	ofstream out("recv.txt",ios_base::out|ios_base::binary);
	while(1)
	{

		zmq_pollitem_t item = { rep_socket, 0, ZMQ_POLLIN, 0};

		if (zmq_poll(&item, 1, 20) < 0) {
			zmq_ctx_destroy(zmqContext_);
			zmq_close(rep_socket);
			return -1;
		}

//		printf("events=%d,revents=%d\n",item.events,item.revents);
		if (item.revents & ZMQ_POLLIN)
		{

			string title;
			vector<unsigned char>  content;
			int recv_len = zmq_lt_recv(rep_socket,content);
			printf("recv_len = %d\n",recv_len);

			for (unsigned char b : content)
			{
				cout << b;
			}

			title = "recv";
			unsigned char *send_buffer = "recv ok";
			int send_len  = zmq_lt_send(rep_socket,send_buffer, 7);
			printf("send_len = %d\n",send_len);

		}

	}

	zmq_close(rep_socket);

	zmq_ctx_destroy(zmqContext_);


	return 0;
}

int main1()
{
	string port = "6113";

	//使用zeromq发送文件流，创建zmq的上下文
	void *zmqContext_ = zmq_ctx_new();
	assert(zmqContext_);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *rep_socket = zmq_socket(zmqContext_, ZMQ_REP);
	assert(rep_socket);

	int rc = zmq_bind(rep_socket, ("tcp://*:" + port).c_str());
	assert(rc == 0);

	char buffer[256] = { 0 };
	char* begin_str = "Start the config file transfer";
	char* end_str = "Configuration file transfer complete";
	bool finish_flag = false;
	ofstream out("recv.txt",ios_base::out|ios_base::binary);
	while(!finish_flag)
	{

		memset(buffer, 0, 256);

		zmq_pollitem_t item = { rep_socket, 0, ZMQ_POLLIN, 0};

		if (zmq_poll(&item, 1, 20) < 0) {
			zmq_ctx_destroy(zmqContext_);
			zmq_close(rep_socket);
			return -1;
		}

//		printf("events=%d,revents=%d\n",item.events,item.revents);
		if (item.revents & ZMQ_POLLIN)
		{

			int recv_len = zmq_recv(rep_socket, buffer, 256, 0);

			if(!strcmp(buffer,begin_str))
			{
				cout <<buffer<<endl;
				if(out.is_open())
				{
					out.close();
				}
				out.open("recv.txt",ios_base::out|ios_base::binary);

			}
			else if(!strcmp(buffer,end_str))
			{
				finish_flag =  true;
				out.flush();
				out.close();
			}
			else
			{
				out<<buffer<<endl;
			}
			//返回结果
		    char *str = new char [256];
		    sprintf(str,"recv ok recv_len :%d",recv_len);
			zmq_send(rep_socket, str, strlen(str), 0);

		}

	}

	zmq_close(rep_socket);

	zmq_ctx_destroy(zmqContext_);


	return 0;
}
