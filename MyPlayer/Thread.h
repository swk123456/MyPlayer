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
	int abort_ = 0;//当该参数为1时退出线程
	std::thread* thread_ = nullptr;
};

