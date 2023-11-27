#pragma once

#include <QImage>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/pixdesc.h>
}

class Utility
{
public:
	static QImage getQImageFromFrame(const AVFrame* pFrame);
};

