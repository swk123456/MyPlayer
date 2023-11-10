#include "Thread.h"

Thread::Thread()
{
}

Thread::~Thread()
{
}

int Thread::Start()
{
	return 0;
}

int Thread::Stop()
{
	abort_ = 1;
	if (thread_)
	{
		thread_->join();
		delete thread_;
		thread_ = nullptr;
	}
	return 0;
}
