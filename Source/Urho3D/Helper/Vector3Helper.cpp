#include "Vector3Helper.h"

#include "Math/Vector3i.h"

namespace Urho3D
{
	int GetIndexFromVector(const Vector3i& lhs, int x, int y, int z)
	{
		return (z * lhs.x * lhs.y) + (y * lhs.x) + x;
	}

	int GetIndexFromVector(const Vector3i& lhs, const Vector3i& rhs)
	{
		return GetIndexFromVector(lhs, rhs.x, rhs.y, rhs.z);
	}

	Vector3i GetVector(const Vector3i& lhs, int i)
	{
		int idx = i;
		int z = idx / (lhs.x * lhs.y);
		idx -= z * lhs.x * lhs.y;
		int y = idx / lhs.x;
		int x = idx % lhs.x;

		return Vector3i(x, y, z);
	}

	int GetArrayCount(const Vector3i lhs)
	{
		return lhs.x * lhs.y * lhs.z;
	}
}