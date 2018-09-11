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
	void *req_socket = zmq_socket(zmqContext_, ZMQ_REQ);
	assert(req_socket);

	int rc = zmq_connect(req_socket, ("tcp://localhost:" + port).c_str());
	assert(rc == 0);

	char *buffer;
	int length;
	unsigned char *send_buffer;

	ifstream is;
	string filename = "test.txt";
	is.open(filename, ios::binary);

	// get length of file:
	is.seekg(0, ios::end);
	length = is.tellg();
	is.seekg(0, ios::beg);

	// allocate memory:
	send_buffer = new unsigned char[length];

	// read data as a block:
	is.read(send_buffer, length);


	int send_len  = zmq_lt_send(req_socket,send_buffer, length);
	printf("send_len = %d\n",send_len);

	vector<unsigned char>  content;
	int recv_len = zmq_lt_recv(req_socket,content);
	printf("recv_len = %d\n",recv_len);

	for (unsigned char b : content)
	{
		cout << b;
	}
	cout<<endl;

	char* buf = &content[0];
	printf("recv data:%s\n",buf);


	is.close();
	zmq_close(req_socket);

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
	void *req_socket = zmq_socket(zmqContext_, ZMQ_REQ);
	assert(req_socket);

	int rc = zmq_connect(req_socket, ("tcp://localhost:" + port).c_str());
	assert(rc == 0);

	char buffer[256] = { 0 };
	char* begin_str = "Start the config file transfer";
	char* end_str = "Configuration file transfer complete";

	int length = 256;

	char send_buffer[256] = {0};
	ifstream is;
	string filename = "test.txt";
	is.open(filename, ios::binary);

	bool begin_flag = true;

	while(!is.eof())
	{
		if(begin_flag)
		{
			zmq_send(req_socket, begin_str, strlen(begin_str), 0);
			begin_flag=false;
		}

		memset(buffer, 0, 256);

		is.getline(send_buffer, length);

		zmq_send(req_socket, send_buffer, length, 0);

		zmq_recv(req_socket, buffer, 256, 0);

		printf("%s\n", buffer);

	}

	zmq_send(req_socket, end_str, strlen(end_str), 0);
	begin_flag=false;

	zmq_recv(req_socket, buffer, 256, 0);

	printf(" over the config tranfer %s\n", buffer);

	is.close();
	zmq_close(req_socket);

	zmq_ctx_destroy(zmqContext_);
	return 0;
}
