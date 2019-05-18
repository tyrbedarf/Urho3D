#pragma once

#include <unordered_map>
#include <math.h>
#include <string>
#include <sstream>

#include "Math/Vector3.h"

namespace Urho3D
{
	struct Vector3d
	{
	public:
		double x;
		double y;
		double z;

		Vector3d() : Vector3d(0) { }

		Vector3d(const Vector3& copy)
			: Vector3d(copy.x_, copy.y_, copy.z_) { }

		Vector3d(const Vector3d& copy)
			: Vector3d(copy.x, copy.y, copy.z) { }

		Vector3d(double x)
			: Vector3d(x, x, x) { }

		Vector3d(double x1, double y1, double z1)
		{
			x = x1;
			y = y1;
			z = z1;
		}

		static Vector3d Zero() { return Vector3d(0); }
		static Vector3d One() { return Vector3d(1); }
		static Vector3d Forward() { return Vector3d(1, 0, 0); }
		static Vector3d Up() { return Vector3d(0, 1, 0); }

		Vector3d operator +(const Vector3d& rhs)
		{
			return Vector3d(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		Vector3d operator -(const Vector3d& rhs)
		{
			return Vector3d(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		Vector3d operator *(double lhs)
		{
			return Vector3d(lhs * x, lhs * y, lhs * z);
		}

		Vector3d operator /(double d)
		{
			return Vector3d(x / d, y / d, z / d);
		}


		double SqrMagnitude()
		{
			return (x * x) + (y * y) + (z * z);
		}

		double Magnitude()
		{
			return std::sqrt(SqrMagnitude());
		}

		Vector3d Cross(const Vector3d& rhs)
		{
			return Vector3d(
				y * rhs.z - z * rhs.y,
				z * rhs.x - x * rhs.z,
				x * rhs.y - y * rhs.x);
		}

		Vector3d Normalize()
		{
			double num = Magnitude();
			if (num > 9.99999974737875E-06)
				return *this / num;
			else
				return Zero();
		}

		size_t GetHashCode() const
		{
			return
				(std::hash<double>()(x) << 20) ^
				(std::hash<double>()(y) << 10) ^
				(std::hash<double>()(z) << 0);
		}

		bool operator ==(const Vector3d& rhs)
		{
			return *this == rhs;
		}

		bool operator !=(Vector3d rhs)
		{
			return !(*this == rhs);
		}

		Vector3 ToV3()
		{
			return Vector3(x, y, z);
		}

		std::string ToString()
		{
			using namespace std;
			stringstream ss;
			ss << x << ", " << y << ", " << z;

			return ss.str();
		}
	};
}

namespace std
{
	template <>
	struct hash<Urho3D::Vector3d>
	{
		size_t operator()(const Urho3D::Vector3d& k) const
		{
			return k.GetHashCode();
		}
	};

	bool operator ==(const Urho3D::Vector3d& lhs, const Urho3D::Vector3d& rhs);
}