#ifndef LTZMQ_H
#define LTZMQ_H

#include <vector>
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>
#include <memory>

#include <string.h>
#include <stdio.h>
#include <zmq.h>


typedef unsigned short DataUnit;
typedef unsigned char ByteUnit;
typedef long long ClockTick;  //ms

namespace LTUtilities {
	
	static int Send2Parts(void* s, const std::string & title, unsigned char *data, int dataLen)
	{
		zmq_msg_t msg;
		int rc = -1;
		assert(s);

		//first frame: title
		zmq_msg_init_size(&msg, title.size());
		memcpy(zmq_msg_data(&msg), title.c_str(), title.size());
		rc = zmq_msg_send(&msg, s, ZMQ_SNDMORE);
		if (rc < 0) {
			zmq_msg_close(&msg);
			return rc;
		}

		//2rd frame: real data
		if (data) {
			zmq_msg_init_size(&msg, dataLen);
			memcpy(zmq_msg_data(&msg), data, dataLen);
			rc = zmq_msg_send(&msg, s, 0);
			if (rc < 0) {
				zmq_msg_close(&msg);
				return rc;
			}
		}
		else {
			zmq_msg_init_size(&msg, 0);
			rc = zmq_msg_send(&msg, s, 0);
			if (rc < 0) {
				zmq_msg_close(&msg);
				return rc;
			}
		}
		zmq_msg_close(&msg);
		return 0;
	}

	static int Send2Strings(void* s, const std::string & title, std::string content)
	{
		zmq_msg_t msg;
		int rc = -1;

		//first frame: title
		zmq_msg_init_size(&msg, title.size());
		memcpy(zmq_msg_data(&msg), title.c_str(), title.size());
		rc = zmq_msg_send(&msg, s, ZMQ_SNDMORE);
		if (rc < 0) {
			zmq_msg_close(&msg);
			return rc;
		}

		//2rd frame: real data
		if (!content.empty()) {
			zmq_msg_init_size(&msg, content.size());
			memcpy(zmq_msg_data(&msg), content.c_str(), content.size());
			rc = zmq_msg_send(&msg, s, 0);
			if (rc < 0) {
				zmq_msg_close(&msg);
				return rc;
			}
		}
		else {
			char val = 0;
			zmq_msg_init_size(&msg, 1);
			memcpy(zmq_msg_data(&msg), &val, 1);
			rc = zmq_msg_send(&msg, s, 0);
			if (rc < 0) {
				zmq_msg_close(&msg);
				return rc;
			}
		}

		zmq_msg_close(&msg);
		return 0;
	}
	
	static int zmq_lt_send(void* s, unsigned char *data, int dataLen)
	{
		zmq_msg_t msg;
		int rc = -1;

		zmq_msg_init_size(&msg, dataLen);
		memcpy(zmq_msg_data(&msg), data, dataLen);
		rc = zmq_msg_send(&msg, s, 0);

		zmq_msg_close(&msg);
		return rc;

	}

	static int zmq_lt_recv(void* s,std::vector<unsigned char> & content)
	{
		zmq_msg_t msg;
		int size = 0;

		content.clear();

		zmq_msg_init(&msg);
		size = zmq_msg_recv(&msg, s, 0);
		if (size < 0)
		{
			zmq_msg_close(&msg);
			return size;
		}
		content.insert(content.begin(), (unsigned char*)zmq_msg_data(&msg), (unsigned char*)zmq_msg_data(&msg) + size);

		zmq_msg_close(&msg);
		return size;
	}

	static int Recv2Parts(void* s, std::string & title, std::vector<unsigned char> & content)
	{
		zmq_msg_t msg;
		int more = 0, size = 0;
		size_t moreSize = sizeof(more);
		title.clear();
		content.clear();

		//first frame: title
		zmq_msg_init(&msg);
		size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);

		if (size < 0) {
			int errNum = zmq_errno();
			if (errNum == EAGAIN) {
				zmq_msg_close(&msg);
				return 0;
			}
			else {
				zmq_msg_close(&msg);
				return -1;
			}
		}

		title.insert(0, (const char*)zmq_msg_data(&msg), size);

		zmq_getsockopt(s, ZMQ_RCVMORE, &more, &moreSize);
		if (!more) {
			zmq_msg_close(&msg);
			return -1;
		}
		zmq_msg_close(&msg);

		//2rd frame: real data
		zmq_msg_init(&msg);
		size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);
		if (size < 0) {
			zmq_msg_close(&msg);
			return size;
		}
		content.insert(content.begin(), (unsigned char*)zmq_msg_data(&msg), (unsigned char*)zmq_msg_data(&msg) + size);

		zmq_msg_close(&msg);
		return 0;
	}

	static bool HasMoreMsg(void* s)
	{
		assert(s);
		int more = 0;
		size_t moreSize = sizeof(more);
		int rc = zmq_getsockopt(s, ZMQ_RCVMORE, &more, &moreSize);
		if (rc != 0)
			return false;
		return more != 0;
	}

    //Receive all available messages from a socket. This is useful when the messages arrive too
    //fast for the application to process. The Zeromq by default drops latest messages when the
    //internal buffer is overflowed; however the application may want to keep the newest messages
    //and discard the old one. We thus take all messages off at once and leave it to the application
    //to decide what to do.
	//0 on success
	//< 0 on failure
	static int RecvAllMessages(void* s,
		std::vector<std::shared_ptr<std::string> > & titles,
		std::vector<std::shared_ptr<std::vector<ByteUnit>> > & contents,
		int & errNum)
	{
		zmq_msg_t msg;
		int more = 0, size = 0;
		size_t moreSize = sizeof(more);

		while (1) {

			//first frame: title
			zmq_msg_init(&msg);
			size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);

			if (size < 0) {
				errNum = zmq_errno();
				if (errNum == EAGAIN) {
					break; //finished picking up all messages
				}
				else {
					zmq_msg_close(&msg);
					return -1;
				}
			}

			std::shared_ptr<std::string> ptitle(new std::string());
			ptitle->insert(0, (const char*)zmq_msg_data(&msg), size);

			zmq_getsockopt(s, ZMQ_RCVMORE, &more, &moreSize);
			if (!more) {
				errNum = zmq_errno();
				zmq_msg_close(&msg);
				return -1;
			}

			zmq_msg_close(&msg);

			//2rd frame: real data
			zmq_msg_init(&msg);
			size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);
			if (size < 0) {
				errNum = zmq_errno();
				zmq_msg_close(&msg);
				return -1;
			}

			std::shared_ptr<std::vector<ByteUnit>> pcontent(new std::vector<ByteUnit>());
			pcontent->insert(pcontent->begin(), (unsigned char*)zmq_msg_data(&msg), (unsigned char*)zmq_msg_data(&msg) + size);

			titles.push_back(ptitle);
			contents.push_back(pcontent);

			zmq_msg_close(&msg);
		}

		zmq_msg_close(&msg);
		return 0;
	}

#if 0
	static int Recv2Strings(void* s, std::string & title, std::string & content)
	{
		zmq_msg_t msg;
		int more = 0, size = 0;
		size_t moreSize = sizeof(more);
		title.clear();
		content.clear();

		//first frame: title
		zmq_msg_init(&msg);
		size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);
		if (size < 0) {
			zmq_msg_close(&msg);
			return size;
		}

		title.insert(0, (const char*)zmq_msg_data(&msg), size);

		zmq_getsockopt(s, ZMQ_RCVMORE, &more, &moreSize);
		if (!more) {
			zmq_msg_close(&msg);
			return -1;
		}

		//2rd frame: real data
		zmq_msg_init(&msg);
		size = zmq_msg_recv(&msg, s, ZMQ_DONTWAIT);
		if (size < 0) {
			zmq_msg_close(&msg);
			return size;
		}
		content.insert(content.begin(), (unsigned char*)zmq_msg_data(&msg), (unsigned char*)zmq_msg_data(&msg) + size);

		zmq_msg_close(&msg);
		return 0;
	}
#endif

	static void CloseSocketQuick(void **s) {
		assert(s);
		if (*s) {
			int val = 0;
			zmq_setsockopt(*s, ZMQ_LINGER, &val, sizeof(val));
			zmq_close(*s);
			*s = nullptr;
		}
	}
}


#endif
