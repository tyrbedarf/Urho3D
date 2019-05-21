#pragma once

#include <inttypes.h>
#include "../../IO/Log.h"

namespace Urho3D
{
	struct Voxel
	{
		int16_t mId;
		int8_t mHitpoints;
		int mAttributes;

	public:
		int GetId()
		{
			return mId;

		}

		void SetId(int value)
		{
			if (value >= 65536)
			{
				URHO3D_LOGDEBUG("Max value for voxel id is 65535.");
				return;
			}

			mId = (int16_t) value;
		}

		int GetHitpoints()
		{
			return mHitpoints;

		}

		void SetHitpoints(int value)
		{
			if (value >= 256)
			{
				URHO3D_LOGDEBUG("Max value for voxel hitpoint is 255");
				return;
			}

			mHitpoints = (int8_t) value;
		}

		bool IsTransparent()
		{

			return ((mAttributes & (1 << 0)) >> 0) > 0;
		}

		void SetTransparent(bool value)
		{
			if (value)
				mAttributes ^= 1 << 0;
			else
				mAttributes &= ~(1 << 0);
		}

		bool IsBlock()
		{
			return ((mAttributes & (1 << 1)) >> 1) > 0;
		}

		void SetBlock(bool value)
		{
			if (value)
				mAttributes ^= 1 << 1;
			else
				mAttributes &= ~(1 << 1);
		}

		bool IsModel()
		{
			return ((mAttributes & (1 << 2)) >> 2) > 0;
		}

		void SetModel(bool value)
		{
			if (value)
				mAttributes ^= 1 << 2;
			else
				mAttributes &= ~(1 << 2);
		}

		bool IsAir()
		{
			return mId == 0;
		}

		static Voxel GetAir()
		{
			Voxel r;
			r.mId = 1;
			r.SetTransparent(true);
			r.SetBlock(false);

			return r;
		}

		static Voxel GetStone()
		{
			Voxel r;
			r.mId = 2;
			r.SetTransparent(false);
			r.SetBlock(false);

			return r;
		}
	};
}