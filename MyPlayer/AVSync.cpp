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

//��audio ptsά��
void AVSync::SetClock(double pts)
{
	double time = getMicroseconds() / 1000000.0;//��
	ptsDrift = pts - time;
}

//��video��ȡ
double AVSync::GetClock()
{
	double time = getMicroseconds() / 1000000.0;//��
	return ptsDrift + time;
}

time_t AVSync::getMicroseconds()
{
	system_clock::time_point timePointNew = system_clock::now();
	system_clock::duration duration = timePointNew.time_since_epoch();
	time_t us = duration_cast<microseconds>(duration).count();
	return us;
}
