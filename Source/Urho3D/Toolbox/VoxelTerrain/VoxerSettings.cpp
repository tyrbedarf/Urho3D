#include "VoxerSettings.h"

namespace Urho3D
{
	double GetMax(double a, double b, double c)
	{
		double r = a;
		if (r < b)
		{
			r = b;
		}

		if (r < c)
		{
			r = c;
		}

		return r;
	}
}