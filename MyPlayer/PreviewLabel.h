#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>
#include "Thread.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil\opt.h>
}

class PreviewLabel : public QWidget, public Thread
{
	Q_OBJECT
public:
	PreviewLabel(QWidget* parent = nullptr);

	void setTime(QString fileName, int time);
	void closeLabel();

	virtual int Start();
	virtual int Stop();
	virtual void Run();

private:
	QImage* generateVideoPreview(const QString& videoFilePath, int time);

private:
	QVBoxLayout* layout = nullptr;
	QLabel* timeLabel = nullptr;
	QLabel* pictureLabel = nullptr;

	int time = 0;
	QImage* image = nullptr;

	AVFormatContext* formatContext = nullptr;
	AVCodecContext* codecContext = nullptr;
	int videoStreamIndex = -1;
	double timeBase = 0;
	QString filePath = "";
	QString newFilePath = "";
};

