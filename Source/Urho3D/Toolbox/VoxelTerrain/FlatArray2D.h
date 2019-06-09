#pragma once

#include "../../Container/Vector.h"

namespace Urho3D
{
	template<class T>
	class FlatArray2D
	{
	protected:
		Vector<T> Data;
		int Width;
		int Height;

		int GetIndex(int x, int y) const
		{
			return (Height * x) + y;
		}

	public:
		int Size()
		{
			return Height * Width;
		}

		FlatArray2D(int width, int height, T def)
		{
			Data.Resize(width * height);
			Width = width;
			Height = height;

			for (int i = 0; i < Size(); i++)
			{
				Data[i] = def;
			}
		}

		void Set(int x, int y, T value)
		{
			Data[GetIndex(x, y)] = value;
		}

		T Get(int x, int y) const
		{
			return Data[GetIndex(x, y)];
		}
	};
}