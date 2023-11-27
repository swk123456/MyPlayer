#pragma once

#include "Queue.h"

#include <QImage>

class QImageQueue
{
public:
	QImageQueue();
	~QImageQueue();

	void Abort();
	int Size();
	int Push(QImage* val);
	QImage* Pop(const int timeout);
	QImage* Front();

private:
	void release();

	Queue<QImage*> queue_;
};

