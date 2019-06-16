#pragma once

#include <string>
#include <sstream>

#include "../../ThirdParty/EASTL/functional.h"

namespace Urho3D
{
    struct Vector3i
    {
	public:
		int x;
        int y;
        int z;

		Vector3i()
			: Vector3i(0, 0, 0) { }

        Vector3i(int x)
            : Vector3i(x, x, x) { }

        Vector3i(int x1, int y1, int z1)
        {
            x = x1;
            y = y1;
            z = z1;
        }

		Vector3i(const Vector3i& vec)
			: Vector3i(vec.x, vec.y, vec.z)
		{

		}

		std::string ToString() const
        {
			std::stringstream ss;
			ss << "(" << x << ", " << y << ", " << z << ")";
            return ss.str();
        }

        /// <summary>
        /// Return the number of elements we need for creating a flat array.
        /// </summary>
        /// <returns></returns>
        int GetArrayCount() const
        {
            return x * y * z;
        }

        /// <summary>
        /// Add two vectors.
        /// </summary>
        /// <param name="lhs">Left hand side</param>
        /// <param name="rhs">Right hand side</param>
        /// <returns></returns>
        Vector3i operator+(const Vector3i& rhs) const
        {
            return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z);
        }

		Vector3i operator*(int i) const
		{
			return Vector3i(x * i, y * i, z * i);
		}

        /// <summary>
        /// Subtracts two vectors.
        /// </summary>
        /// <param name="lhs">Left hand side</param>
        /// <param name="rhs">Right hand side</param>
        /// <returns></returns>
        Vector3i operator -(const Vector3i& rhs) const
        {
            return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        /// <summary>
        /// Returns the position inside a flattend 3d array given the three components.
        /// Since the voxel layout of a chunk is expressed as vector3i this method
        /// is only called on the corredponding VoxelLayout vector, that
		/// handles how voxel are layed out in a chunk.
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        /// <returns></returns>
        int GetIndex(int x1, int y1, int z1) const
        {
            return (z1 * x * y) + (y1 * x) + x1;
        }

        int GetIndex(const Vector3i& v) const
        {
            return GetIndex(v.x, v.y, v.z);
        }

        /// <summary>
        /// Return the vector representing an index inside a chunk and turn it into a vector again.
        /// Since the voxel layout of a chunk is expressed as vector3i this method
        /// is only called on the corredponding VoxelLayout vector, that handles the voxel layout.
        /// </summary>
        /// <param name="i">Index</param>
        /// <returns></returns>
        Vector3i GetVector(int i) const
        {
            int idx = i;
            int z0 = idx / (x * y);
            idx -= z0 * x * y;
            int y0 = idx / x;
            int x0 = idx % x;

            return Vector3i(x0, y0, z0);
        }

        bool operator==(const Vector3i& rhs) const
        {
            return
                x == rhs.x &&
                y == rhs.y &&
                z == rhs.x;
        }

        bool operator!=(const Vector3i& rhs) const
        {
            return !(*this == rhs);
        }

		size_t GetHashCode() const
        {
            return (x << 20) ^ (y << 10) ^ (z);
        }

        bool Equals(Vector3i other) const
        {
            return
                x == other.x &&
                y == other.y &&
                z == other.z;
        }
	};
}

namespace eastl
{
	template <>
	struct hash<Urho3D::Vector3i>
	{
		size_t operator()(const Urho3D::Vector3i& k) const
		{
			return k.GetHashCode();
		}
	};
}
