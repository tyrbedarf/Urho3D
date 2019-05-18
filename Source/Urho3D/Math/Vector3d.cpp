#pragma once

#include "Vector3d.h"

namespace std
{
	bool operator ==(const Urho3D::Vector3d& lhs, const Urho3D::Vector3d& rhs)
	{
		return
			std::abs(lhs.x - rhs.x) < std::numeric_limits<double>::epsilon() &&
			std::abs(lhs.y - rhs.y) < std::numeric_limits<double>::epsilon() &&
			std::abs(lhs.z - rhs.z) < std::numeric_limits<double>::epsilon();
	}
}