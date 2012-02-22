//	basic linear algebra framework
#include <math.h>

#include "blaf.h"

namespace blaf
{
	double rad2deg( const double radians )
	{
		return radians * 180.0 * dPIi;
	}

	float rad2deg( const float radians )
	{
		return radians * 180.0f * fPIi;
	}



	double deg2rad( const double degrees )
	{
		return degrees * dPI * d180i;
	}

	float deg2rad( const float degrees )
	{
		return degrees * fPI * f180i;
	}
}