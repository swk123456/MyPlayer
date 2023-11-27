#include "VideoQtOutput.h"

VideoQtOutput::VideoQtOutput(AVSync* avsync, AVFrameQueue* frameQueue, QImageQueue* imageQueue, int video_width, int video_height, AVRational timebase, VideoForm* form)
	: avsync(avsync), frameQueue(frameQueue), imageQueue(imageQueue), videoWidth(video_width), videoHeight(video_height), timebase(timebase), form(form)
{
}

VideoQtOutput::~VideoQtOutput()
{
}

int VideoQtOutput::Init()
{
	return 0;
}

int VideoQtOutput::Start()
{
    thread_ = new std::thread(&VideoQtOutput::Run, this);
    if (!thread_)
    {
        printf("new VideoQtOutput failed\n");
        return -1;
    }
    return 0;
}

int VideoQtOutput::Stop()
{
    Thread::Stop();
    printf("VideoQtOutput::Stop\n");
    return 0;
}

void VideoQtOutput::Run()
{
    double remainTime = 0.0;
    while (true)
    {
        if (abort_ == 1)
        {
            break;
        }
        if (remainTime > 0.0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(remainTime * 1000)));
        }
        videoRefresh(remainTime);
    }
}

void VideoQtOutput::videoRefresh(double& remainTime)
{
	AVFrame* frame = nullptr;
    QImage* image = nullptr;
	frame = frameQueue->Front();
	if (frame)
	{
		double pts = frame->pts * av_q2d(timebase);
		double diff = pts - avsync->GetClock();
		printf("video pts:%0.3lf, diff:%0.3f\n", pts, diff);
		if (diff > 0.0)
		{
			remainTime = diff;
			if (remainTime > REFRESH_RATE)
			{
				remainTime = REFRESH_RATE;
			}
			return;
		}
        image = imageQueue->Front();
        //image->save(QString("./pictures/%1.png").arg(int(pts * 100)), "PNG", 100);
        form->onSendImage(*image, int(pts * 100), 40);
        imageQueue->Pop(1);

		frame = frameQueue->Pop(1);
		av_frame_free(&frame);
	}
}