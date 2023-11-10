#pragma once

#include <chrono>
#include <ctime>
#include <math.h>

using namespace std::chrono;

class AVSync
{
public:
	AVSync();
	~AVSync();

	void InitClock();

	void SetClock(double pts);
	double GetClock();

	time_t getMicroseconds();

	double ptsDrift = 0;
};

