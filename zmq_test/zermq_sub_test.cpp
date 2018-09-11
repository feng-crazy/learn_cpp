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
#include <thread>

#include "MyZmq.h"

using namespace std;
using namespace  LTUtilities;

string MESSAGE_ENDPOINT = "inproc://message";

int main1(void *zmqContext_,void *pub_socket)
{
	//创建zmq的套接字，设置套接字为请求应答模式
	void *sub_socket = zmq_socket(zmqContext_, ZMQ_SUB);
	assert(sub_socket);

	int rc = zmq_connect(sub_socket, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "boy", 3);
	assert(rc == 0);
	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "girl", 4);
	assert(rc == 0);


	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
//		printf("main1...\n");
		zmq_pollitem_t item = { sub_socket, 0, ZMQ_POLLIN, 0};

		if (zmq_poll(&item, 1, 20) < 0) {
			zmq_ctx_destroy(zmqContext_);
			zmq_close(sub_socket);
			return -1;
		}

//		printf("events=%d,revents=%d\n",item.events,item.revents);
		if (item.revents & ZMQ_POLLIN)
		{
			string title;
			vector<unsigned char>  content;
			Recv2Parts(sub_socket,title,content);
			printf("main1 title:%s\n",title.c_str());
			for(size_t i = 0;i<content.size();i++)
			{
				printf("%02X ",content[i]);
			}
			printf("\n");
		}

	}

	zmq_close(sub_socket);


	return 0;
}


int main3(void *zmqContext_,void *pub_socket)
{

	//使用zeromq发送文件流，创建zmq的上下文
	void *zmqContext_3 = zmq_ctx_new();
	assert(zmqContext_3);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *pub_socket_3 = zmq_socket(zmqContext_3, ZMQ_PUB);
	assert(pub_socket);

	int rc = zmq_bind(zmqContext_3, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *sub_socket = zmq_socket(zmqContext_3, ZMQ_SUB);
	assert(sub_socket);

	rc = zmq_connect(sub_socket, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "boy", 3);
	assert(rc == 0);
	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "girl", 4);
	assert(rc == 0);


	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
//		printf("main3...\n");
		zmq_pollitem_t item = { sub_socket, 0, ZMQ_POLLIN, 0};

		if (zmq_poll(&item, 1, 20) < 0) {
			zmq_ctx_destroy(zmqContext_);
			zmq_close(sub_socket);
			return -1;
		}

//		printf("events=%d,revents=%d\n",item.events,item.revents);
		if (item.revents & ZMQ_POLLIN)
		{
			string title;
			vector<unsigned char>  content;
			Recv2Parts(sub_socket,title,content);
			printf("main3 title:%s\n",title.c_str());
			for(size_t i=0;i<content.size();i++)
			{
				printf("%02X ",content[i]);
			}
			printf("\n");
		}

		string title = "boy";
		unsigned char boy_data[] = {6,7,8,9,10};
		int send_ret = Send2Parts(pub_socket,title,boy_data,5);
//		printf("boy_data send ret = %d\n",send_ret);

		title = "girl";
		unsigned char girl_data[] = {10,9,8,7,6};
		send_ret = Send2Parts(pub_socket,title,girl_data,5);
//		printf("girl_data send ret = %d\n",send_ret);

	}

	zmq_close(sub_socket);


	return 0;
}

int main2(void *zmqContext_,void *pub_socket)
{
	//使用zeromq发送文件流，创建zmq的上下文
	void *zmqContext_2 = zmq_ctx_new();
	assert(zmqContext_2);


	//创建zmq的套接字，设置套接字为请求应答模式
	void *pub_socket_2 = zmq_socket(zmqContext_2, ZMQ_PUB);
	assert(pub_socket);

	int rc = zmq_bind(zmqContext_2, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *sub_socket = zmq_socket(zmqContext_2, ZMQ_SUB);
	assert(sub_socket);

	rc = zmq_connect(sub_socket, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "boy", 3);
	assert(rc == 0);
	rc = zmq_setsockopt(sub_socket, ZMQ_SUBSCRIBE, "girl", 4);
	assert(rc == 0);

	while(1)
	{
//		printf("main2...\n");
		string title = "boy";
		unsigned char boy_data[] = {1,2,3,4,5};
		int send_ret = Send2Parts(pub_socket,title,boy_data,5);
//		printf("boy_data send ret = %d\n",send_ret);

		title = "girl";
		unsigned char girl_data[] = {5,4,3,2,1};
		send_ret = Send2Parts(pub_socket,title,girl_data,5);
//		printf("girl_data send ret = %d\n",send_ret);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		zmq_pollitem_t item = { sub_socket, 0, ZMQ_POLLIN, 0};

		if (zmq_poll(&item, 1, 20) < 0) {
			zmq_ctx_destroy(zmqContext_);
			zmq_close(sub_socket);
			return -1;
		}

//		printf("events=%d,revents=%d\n",item.events,item.revents);
		if (item.revents & ZMQ_POLLIN)
		{
			string title;
			vector<unsigned char>  content;
			Recv2Parts(sub_socket,title,content);
			printf("main2 title:%s\n",title.c_str());
			for(size_t i=0;i<content.size();i++)
			{
				printf("%02X ",content[i]);
			}
			printf("\n");
		}

	}

	zmq_close(pub_socket);
	return 0;
}

int main()
{
	//使用zeromq发送文件流，创建zmq的上下文
	void *zmqContext_ = zmq_ctx_new();
	assert(zmqContext_);

	//创建zmq的套接字，设置套接字为请求应答模式
	void *pub_socket = zmq_socket(zmqContext_, ZMQ_PUB);
	assert(pub_socket);

	int rc = zmq_bind(pub_socket, MESSAGE_ENDPOINT.c_str());
	assert(rc == 0);

	thread sub(main1,zmqContext_,pub_socket);
	thread sub2(main3,zmqContext_,pub_socket);
	thread pub(main2,zmqContext_,pub_socket);
	sub.join();
	sub2.join();
	pub.join();




	printf("zmq_ctx_destroy....\n");
	zmq_ctx_destroy(zmqContext_);
	return 0;
}
