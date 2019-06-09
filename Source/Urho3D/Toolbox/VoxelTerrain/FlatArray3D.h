#pragma once

#include "../../Math/Vector3i.h"
#include "../../Container/Vector.h"

namespace Urho3D
{
	template<class T>
	class FlatArray3D
	{
	private:
		Vector3i Layout;

	protected:
		Vector<T> Data;

	public:
		FlatArray3D(Vector3i layout)
		{
			Data.Resize(layout.GetArrayCount());
			Layout = layout;
		}

		virtual T Get(int x, int y, int z) const
		{
			return Data[Layout.GetIndex(x, y, z)];
		}

		virtual T Set(int x, int y, int z, T value)
		{
			Data[Layout.GetIndex(x, y, z)] = value;
		}

		/// Direct access to the underlaying data. No checks!
		virtual T Get(int x) const
		{
			return Data[x];
		}

		virtual T Set(int x, T value)
		{
			Data[x] = value;
		}

		Vector3i GetVector(int pos)
		{
			return Layout.GetVector(pos);
		}

		int Count
		{
			return Layout.GetArrayCount();
		}
	};
}