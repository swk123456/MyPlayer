#include "DecodeVideoThread.h"

DecodeVideoThread::DecodeVideoThread(AVPacketQueue* packetQueue, AVFrameQueue* frameQueue, std::string threadName, QImageQueue* imageQueue)
	: DecodeThread(packetQueue, frameQueue, threadName), imageQueue(imageQueue)
{
}

DecodeVideoThread::~DecodeVideoThread()
{
}

int DecodeVideoThread::Start()
{
	thread_ = new std::thread(&DecodeVideoThread::Run, this);
	if (!thread_)
	{
		printf("%s new DecodeThread failed\n", threadName.c_str());
		return -1;
	}
	return 0;
}

void DecodeVideoThread::Run()
{
	AVFrame* frame = av_frame_alloc();
	int ret = 0;
	while (true)
	{
		if (abort_ == 1)
		{
			break;
		}
		if (frameQueue->Size() > 10)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		//从packetQueue读取packet
		AVPacket* packet = packetQueue->Pop(10);
		if (!packet)
		{
			printf("%s no packet!\n", threadName.c_str());
			continue;
		}
		//送给解码器
		ret = avcodec_send_packet(codecCtx, packet);
		av_packet_free(&packet);
		if (ret < 0)
		{
			av_strerror(ret, errStr, sizeof(errStr));
			printf("%s avcodec_send_packet failed, ret:%d, error:%s\n", threadName.c_str(), ret, errStr);
			break;
		}
		//从解码器读取frame
		while (true)
		{
			if (abort_ == 1)
			{
				break;
			}
			ret = avcodec_receive_frame(codecCtx, frame);
			if (ret == 0)
			{
				QImage image(Utility::getQImageFromFrame(frame));
				frameQueue->Push(frame);
				imageQueue->Push(&image);
				printf("%s frameQueue size:%d\n", threadName.c_str(), frameQueue->Size());
				continue;
			}
			else if (ret == AVERROR(EAGAIN))
			{
				break;
			}
			else
			{
				abort_ = 1;
				av_strerror(ret, errStr, sizeof(errStr));
				printf("%s avcodec_receive_frame failed, ret:%d, error:%s\n", threadName.c_str(), ret, errStr);
				break;
			}
		}
		//把frame发送给frameQueue

	}
}
