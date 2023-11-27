#include "QImageQueue.h"

QImageQueue::QImageQueue()
{
}

QImageQueue::~QImageQueue()
{
}

void QImageQueue::Abort()
{
	release();
	queue_.Abort();
}

int QImageQueue::Size()
{
	return queue_.Size();
}

int QImageQueue::Push(QImage* val)
{
	QImage* tmp_image = new QImage(*val);
	return queue_.Push(tmp_image);
}

QImage* QImageQueue::Pop(const int timeout)
{
	QImage* tmp_image = nullptr;
	int ret = queue_.Pop(tmp_image, timeout);
	if (ret < 0)
	{
		if (ret == -1)
		{
			printf("queue_ abort\n");
		}
	}
	return tmp_image;
}

QImage* QImageQueue::Front()
{
	QImage* tmp_image = nullptr;
	int ret = queue_.Front(tmp_image);
	if (ret < 0)
	{
		if (ret == -1)
		{
			printf("queue_ abort\n");
		}
	}
	return tmp_image;
}

void QImageQueue::release()
{
	while (true)
	{
		QImage* tmp_image = nullptr;
		int ret = queue_.Pop(tmp_image, 1);
		if (ret < 0)
		{
			break;
		}
		else
		{
			free(tmp_image);
			tmp_image = nullptr;
		}
	}
}