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
	void *pub_socket = zmq_socket(zmqContext_, ZMQ_PUB);
	assert(pub_socket);

	int rc = zmq_bind(pub_socket, ("tcp://*:" + port).c_str());
	assert(rc == 0);



	while(1)
	{
		string title = "boy";
		unsigned char boy_data[] = {1,2,3,4,5};
		Send2Parts(pub_socket,title,boy_data,5);

		title = "girl";
		unsigned char girl_data[] = {5,4,3,2,1};
		Send2Parts(pub_socket,title,girl_data,5);

	}

	zmq_close(pub_socket);

	zmq_ctx_destroy(zmqContext_);
	return 0;
}


