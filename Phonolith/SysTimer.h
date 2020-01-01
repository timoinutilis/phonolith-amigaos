#ifndef _SYSTIMER_
#define _SYSTIMER_

#include <devices/timer.h>

class SysTimer
{
	public:
		SysTimer();
		~SysTimer();

		void reset();
		unsigned long getMicros();

	protected:
		timeval lastTime;

};

#endif


