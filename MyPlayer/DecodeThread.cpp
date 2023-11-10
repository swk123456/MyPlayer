#include "DecodeThread.h"

DecodeThread::DecodeThread(AVPacketQueue* packetQueue, AVFrameQueue* frameQueue, std::string threadName)
	: packetQueue(packetQueue), frameQueue(frameQueue), threadName(threadName)
{
}

DecodeThread::~DecodeThread()
{
}

int DecodeThread::Init(AVCodecParameters* par)
{
	if (!par)
	{
		printf("%s %s(%d) par is null\n", threadName.c_str(), __FUNCTION__, __LINE__);
		return -1;
	}
	codecCtx = avcodec_alloc_context3(NULL);
	int ret = avcodec_parameters_to_context(codecCtx, par);
	if (ret < 0)
	{
		av_strerror(ret, errStr, sizeof(errStr));
		printf("%s avcodec_parameters_to_context failed, ret:%d, error:%s\n", threadName.c_str(), ret, errStr);
		return -1;
	}
	const AVCodec* codec = avcodec_find_decoder(codecCtx->codec_id);
	if (!codec)
	{
		printf("%s avcodec_find_decoder failed\n",threadName.c_str());
		return -1;
	}
	ret = avcodec_open2(codecCtx, codec, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errStr, sizeof(errStr));
		printf("%s avcodec_open2 failed, ret:%d, error:%s\n", threadName.c_str(), ret, errStr);
		return -1;
	}
	return 0;
}

int DecodeThread::Start()
{
	thread_ = new std::thread(&DecodeThread::Run, this);
	if (!thread_)
	{
		printf("%s new DecodeThread failed\n", threadName.c_str());
		return -1;
	}
	return 0;
}

int DecodeThread::Stop()
{
	Thread::Stop();
	printf("%s DecodeThread::Stop\n", threadName.c_str());
	return 0;
}

void DecodeThread::Run()
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
				frameQueue->Push(frame);
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

AVCodecContext* DecodeThread::getAVCodecContext()
{
	return codecCtx;
}
