#pragma once

#include "Math/Vector3d.h"
#include "Math/Vector3i.h"

namespace Urho3D
{
    class IVoxerSettings
    {
	public:
        /// <summary>
        /// Number of voxels inside a chunk. Set for each direction. Each component of
        /// the given vector must have a value larger than 4.
        /// </summary>
		virtual const Vector3i& GetVoxelCount() const = 0;

        /// <summary>
        /// Size of a single voxel where 1.0f is a cube with 1 meter edge length.
        /// </summary>
		virtual float GetVoxelSize() const = 0;

        /// <summary>
        /// Actual size of a chunk taking voxel size and voxel count into account.
        /// </summary>
		virtual const Vector3d& GetChunkDimension() const = 0;

        /// <summary>
        /// True, if we are currently running as server.
        /// </summary>
		virtual bool IsServer() const = 0;

        /// <summary>
        /// Number of chunks the player can look in each direction.
        /// </summary>
		virtual const Vector3i& GetViewRange() const = 0;

        /// <summary>
        /// How far a chunk can be away from all players before its being destoryed.
        /// </summary>
		virtual double GetDistToDestroy() const = 0;

	};
}