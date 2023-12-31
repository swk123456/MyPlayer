#include "AVFrameQueue.h"

AVFrameQueue::AVFrameQueue()
{
}

AVFrameQueue::~AVFrameQueue()
{
}

void AVFrameQueue::Abort()
{
	release();
	queue_.Abort();
}

int AVFrameQueue::Size()
{
	return queue_.Size();
}

int AVFrameQueue::Push(AVFrame* val)
{
	AVFrame* tmp_frame = av_frame_alloc();
	av_frame_move_ref(tmp_frame, val);
	return queue_.Push(tmp_frame);
}

int AVFrameQueue::Clear()
{
	release();
	if (queue_.Size() != 0)
	{
		return -1;
	}
	return 0;
}

AVFrame* AVFrameQueue::Pop(const int timeout)
{
	AVFrame* tmp_frame = nullptr;
	int ret = queue_.Pop(tmp_frame, timeout);
	if (ret < 0)
	{
		if (ret == -1)
		{
			printf("queue_ abort\n");
		}
	}
	return tmp_frame;
}

AVFrame* AVFrameQueue::Front()
{
	AVFrame* tmp_frame = nullptr;
	int ret = queue_.Front(tmp_frame);
	if (ret < 0)
	{
		if (ret == -1)
		{
			printf("queue_ abort\n");
		}
	}
	return tmp_frame;
}

void AVFrameQueue::release()
{
	while (true)
	{
		AVFrame* tmp_frame = nullptr;
		int ret = queue_.Pop(tmp_frame, 1);
		if (ret < 0)
		{
			break;
		}
		else
		{
			av_frame_free(&tmp_frame);
		}
	}
}
