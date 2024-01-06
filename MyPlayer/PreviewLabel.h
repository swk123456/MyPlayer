#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
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

protected:
	void paintEvent(QPaintEvent* event) override;//重新绘制组件

private:
	QImage* generateVideoPreview(const QString& videoFilePath, int time);
	QImage* getQImageFromFrame(AVFrame* pFrame);

private:
	QVBoxLayout* layout = nullptr;
	QHBoxLayout* timeLayout = nullptr;
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

