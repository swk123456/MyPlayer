#pragma once

#include "Queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class AVFrameQueue
{
public:
	AVFrameQueue();
	~AVFrameQueue();

	void Abort();
	int Size();
	int Push(AVFrame* val);
	AVFrame* Pop(const int timeout);
	AVFrame* Front();

private:
	void release();

	Queue<AVFrame*> queue_;
};

