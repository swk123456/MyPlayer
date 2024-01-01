#pragma once

#include "Queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class AVPacketQueue
{
public:
	AVPacketQueue();
	~AVPacketQueue();

	void Abort();
	int Size();
	int Push(AVPacket* val);
	int Clear();
	AVPacket* Pop(const int timeout);
	AVPacket* Front();

private:
	void release();//สอทลืสิด
	
	Queue<AVPacket*> queue_;
};

