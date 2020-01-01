#ifndef _CORECALC_
#define _CORECALC_

#include <cmath>

class CoreCalc
{
	public:
		static float volumeFactor(int volume)
		{
			return (float)(pow(volume, 1.5) / pow(100, 1.5));
		}

		static float panoramaAmpLeft(signed int panning)
		{
			return 1.0 + log10(0.1 + (63 - panning) / 126.0 * 0.9);
		}

		static float panoramaAmpRight(signed int panning)
		{
			return 1.0 + log10(0.1 + (63 + panning) / 126.0 * 0.9);
		}

};

#endif

