#include "AVSync.h"

AVSync::AVSync()
{
}

AVSync::~AVSync()
{
}

void AVSync::InitClock()
{
	SetClock(NAN);
}

//由audio pts维护
void AVSync::SetClock(double pts)
{
	double time = getMicroseconds() / 1000000.0;//秒
	ptsDrift = pts - time;
}

//由video读取
double AVSync::GetClock()
{
	double time = getMicroseconds() / 1000000.0;//秒
	return ptsDrift + time;
}

time_t AVSync::getMicroseconds()
{
	system_clock::time_point timePointNew = system_clock::now();
	system_clock::duration duration = timePointNew.time_since_epoch();
	time_t us = duration_cast<microseconds>(duration).count();
	return us;
}
