#pragma once

#include <thread>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	int Start();
	int Stop();
	virtual void Run() = 0;

protected:
	int abort_ = 0;//���ò���Ϊ1ʱ�˳��߳�
	std::thread* thread_ = nullptr;
};

