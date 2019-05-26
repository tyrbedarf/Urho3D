#pragma once

#include "../../IO/Log.h"
#include "../../Math/Vector3.h"
#include "../../Math/Vector3i.h"
#include "../../Core/Object.h"
#include "../../Container/Str.h"
#include "../../Math/Vector3d.h"

namespace Urho3D
{
	double GetMax(double a, double b, double c);

	class VoxerSettings : public Object
	{
		URHO3D_OBJECT(VoxerSettings, Object)

	public:
		VoxerSettings(Context* ctx) :
			Object(ctx),
			mVoxelSize(0.5f),
			mVoxelCount(16, 16, 16),
			mViewRange(2, 2, 2),
			mServer(false),
			mChunkDimension(0.0f)
		{
			UpdateCunkDimension();
			UpdateMaxDistanceToDestroy();
		}

		float GetVoxelSize() const
		{
			return mVoxelSize;
		}

		void SetVoxelSize(float value)
		{
			if (value < 0.05f)
			{
				URHO3D_LOGDEBUG("Invalid voxel size given. The minimum value for voxel size is 0.05.");
				return;
			}

			mVoxelSize = value;
			UpdateCunkDimension();
			UpdateMaxDistanceToDestroy();

		}

		const Vector3i& GetVoxelCount() const
		{
			return mVoxelCount;
		}

		void SetVoxelCount(const Vector3i& value)
		{
			if (value.x < 4 || value.y < 4 || value.z < 4)
			{
				URHO3D_LOGDEBUG("Invalid voxel count given. The minimum count for each direction is 4.");
				return;
			}

			mVoxelCount = value;
			UpdateCunkDimension();
			UpdateMaxDistanceToDestroy();
		}

		const Vector3i& GetViewRange() const
		{
			return mViewRange;
		}

		void SetViewRange(const Vector3i& value)
		{
			if (value.x < 1 || value.y < 1 || value.z < 1)
			{
				URHO3D_LOGDEBUG("Minimum value for each direction of the view range must be larger than 1");
				return;
			}

			mViewRange = value;
			UpdateMaxDistanceToDestroy();
		}

		const Vector3d& GetChunkDimension() const
		{
			return mChunkDimension;
		}

		bool IsServer() const
		{
			return mServer;
		}

		double GetDistToDestroy() const
		{
			return mDistToDestroy;
		}


		void UpdateCunkDimension()
		{
			mChunkDimension = Vector3d(
				mVoxelCount.x * mVoxelSize,
				mVoxelCount.y * mVoxelSize,
				mVoxelCount.z * mVoxelSize);
		}

		void UpdateMaxDistanceToDestroy()
		{
			/// We compare against squre magnitude so make the
			/// max distance squared as well. Plus a little to avoid
			/// flickering on the margins.

			mDistToDestroy = GetMax
			(
				std::pow((mViewRange.x * mChunkDimension.x), 2.5f),
				std::pow((mViewRange.y * mChunkDimension.y), 2.5f),
				std::pow((mViewRange.z * mChunkDimension.z), 2.5f)
			);
		}

		protected:
			float mVoxelSize;
			Vector3i mVoxelCount;
			Vector3i mViewRange;
			Vector3d mChunkDimension;
			bool mServer;
			double mDistToDestroy;
	};
}