#pragma once

#include "../Math/Vector3i.h"
#include "../Math/Vector3d.h"

namespace Urho3D
{
	int GetIndexFromVector(const Vector3i& lhs, int x, int y, int z);
	int GetIndexFromVector(const Vector3i& lhs, const Vector3i& rhs);
	Vector3i GetVector(const Vector3i& lhs, int i);
	int GetArrayCount(const Vector3i lhs);
}