#pragma once

#include "IVoxerSettings.h"
#include "API/IChunkProvider.h"
#include "API/IChunk.h"

namespace Urho3D
{
    class IVoxerSystem
    {
	protected:
		static IVoxerSystem* mInstance;

	public:
		static IVoxerSystem* Instance()
		{
			return mInstance;
		}

        /// <summary>
        /// Get currently used settings.
        /// </summary>
		virtual IVoxerSettings* GetSettings() const = 0;

        /// <summary>
        /// Get the current chunk provider
        /// </summary>
		virtual IChunkProvider* GetChunkProvider() const = 0;

        /// <summary>
        /// Called from the main thread to update all subsystems.
        /// </summary>
        /// <param name="playerPositions"></param>
        virtual void Update(std::vector<Vector3d>& playerPositions) = 0;

		virtual void SpawnChunk(IChunk* c) = 0;
		virtual void DestroyChunk(IChunk* c) = 0;

        /// <summary>
        /// Called to stop all subsystem.
        /// </summary>
        virtual void Shutdown() = 0;
	};
}
