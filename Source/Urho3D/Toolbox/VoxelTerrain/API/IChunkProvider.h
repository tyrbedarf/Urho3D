#pragma once

#include "Math/Vector3d.h"
#include <vector>

#include "Core/Object.h"

namespace Urho3D
{
    class IChunkProvider : public Object
    {
		URHO3D_OBJECT(IChunkProvider, Object)

	public:
		IChunkProvider(Context* ctx) : Object(ctx)
		{

		}

        /// <summary>
        /// Called through VoxerSystem during each update.
        /// </summary>
        /// <param name="playerPositions"></param>
        virtual void Update(const std::vector<Vector3d>& playerPositions) = 0;

		virtual void FinishUpdateCycle() = 0;

		/// <summary>
		/// Called through VoxerSystem when the system should stop and clean up all resources.
		/// </summary>
		/// <param name="playerPositions"></param>
		virtual void Shutdown() = 0;
	};
}
