#pragma once

class TimerCallback
{
public:
	virtual void Callback() {}
};

class Timer
{
public:
	typedef void(TimerCallback::*Callback)();

	void Loop() 
	{
		(target->*callback)();
	}

	long Period = 0;
	TimerCallback *target;
	Callback callback = nullptr;
};