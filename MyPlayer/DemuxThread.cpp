#include "DemuxThread.h"

DemuxThread::DemuxThread(AVPacketQueue* audioQueue, AVPacketQueue* videoQueue)
{
	this->audioQueue = audioQueue;
	this->videoQueue = videoQueue;
}

DemuxThread::~DemuxThread()
{
}

int DemuxThread::Init(const char* url, long long& totalPts)
{
	if (!url)
	{
		printf("%s(%d) url is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if (!audioQueue || !videoQueue)
	{
		printf("%s(%d) audioQueue or videoQueue is null\n", __FUNCTION__, __LINE__);
		return -1;
	}
	ifmt_ctx_ = avformat_alloc_context();
	int ret = avformat_open_input(&ifmt_ctx_, url, NULL, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errorStr, sizeof(errorStr));
		printf("%s(%d) avformat_open_input failed:%d %s\n", __FUNCTION__, __LINE__, ret, errorStr);
		return -1;
	}
	ret = avformat_find_stream_info(ifmt_ctx_, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errorStr, sizeof(errorStr));
		printf("%s(%d) avformat_find_stream_info failed:%d %s\n", __FUNCTION__, __LINE__, ret, errorStr);
		return -1;
	}

	av_dump_format(ifmt_ctx_, 0, url, 0);

	totalPts = ifmt_ctx_->duration;

	audioStream = av_find_best_stream(ifmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	videoStream = av_find_best_stream(ifmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	printf("%s(%d) av_find_best_stream: audioStream:%d videoStream:%d\n", __FUNCTION__, __LINE__, audioStream, videoStream);
	if (audioStream < 0 || videoStream < 0)
	{
		printf("no audio or no video\n");
		return -1;
	}
	return 0;
}

int DemuxThread::Start()
{
	if (thread_ && pause_ == 1)
	{
		pause_ = 0;
		return 0;
	}
	thread_ = new std::thread(&DemuxThread::Run, this);
	if (!thread_)
	{
		printf("new DemuxThread failed\n");
		return -1;
	}
	printf("new DemuxThread success\n");
	return 0;
}

int DemuxThread::Stop()
{
	Thread::Stop();
	printf("DemuxThread::Stop\n");
	return 0;
}

void DemuxThread::Run()
{
	AVPacket packet;
	int ret = 0;
	while (true)
	{
		if (abort_ == 1)
		{
			break;
		}
		if (pause_ == 1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}
		if (audioQueue->Size() > 100 || videoQueue->Size() > 100)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		if (!ifmt_ctx_)
		{
			break;
		}
		ret = av_read_frame(ifmt_ctx_, &packet);
		if (ret < 0)
		{
			av_strerror(ret, errorStr, sizeof(errorStr));
			printf("%s(%d) av_read_frame failed:%d %s\n", __FUNCTION__, __LINE__, ret, errorStr);
			break;
		}
		if (packet.stream_index == audioStream)//视频包队列
		{
			audioQueue->Push(&packet);
			printf("audio stream size:%d\n", audioQueue->Size());
		}
		else if (packet.stream_index == videoStream)//音频包队列
		{
			videoQueue->Push(&packet);
			printf("video stream size:%d\n", videoQueue->Size());
		}
		else
		{
			av_packet_unref(&packet);//释放资源
		}
	}
	avformat_close_input(&ifmt_ctx_);
	std::cout << "DemuxThread::Run() leave" << std::endl;
}

int DemuxThread::ResetStartPts(long long& pts)
{
	if (pts > ifmt_ctx_->duration)
	{
		printf("%s(%d) ResetStartPts failed becase pts is too large\n", __FUNCTION__, __LINE__);
		return -1;
	}
	int ret = avformat_flush(ifmt_ctx_);
	if (ret < 0)
	{
		printf("%s(%d) avformat_flush failed\n", __FUNCTION__, __LINE__);
		return -1;
	}
	long long seekPos = pts;
	ret = av_seek_frame(ifmt_ctx_, -1, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	if (ret < 0)
	{
		printf("%s(%d) audio av_seek_frame failed\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

AVCodecParameters* DemuxThread::AudioCodecParameters()
{
	if (audioStream != -1)
	{
		return ifmt_ctx_->streams[audioStream]->codecpar;
	}
	return nullptr;
}

AVCodecParameters* DemuxThread::VedioCodecParameters()
{
	if (videoStream != -1)
	{
		return ifmt_ctx_->streams[videoStream]->codecpar;
	}
	return nullptr;
}

AVRational DemuxThread::AudioStreamTimebase()
{
	return ifmt_ctx_->streams[audioStream]->time_base;
}

AVRational DemuxThread::VideoStreamTimebase()
{
	return ifmt_ctx_->streams[videoStream]->time_base;
}
