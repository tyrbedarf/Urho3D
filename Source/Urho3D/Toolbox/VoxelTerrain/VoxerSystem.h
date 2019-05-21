#pragma once

#include "../../Container/ConcurentQueue.h"
#include "../../Math/Vector3d.h"
#include "../../Scene/Node.h"
#include "../../Core/WorkQueue.h"

#include "VoxerSettings.h"
#include "ChunkProvider.h"
#include "Chunk.h"

#include <unordered_map>

namespace Urho3D
{
	class VoxerSystem : public Object
	{
	private:
		static VoxerSystem* mInstance;

		SharedPtr<VoxerSettings> mSettings;
		SharedPtr<ChunkProvider> mChunkProvider;
		SharedPtr<WorkQueue> mTaskSystem;
		moodycamel::ConcurrentQueue<Chunk*> mChunksToSpawn;
		moodycamel::ConcurrentQueue<Vector3d> mChunksToDespawn;

		SharedPtr<Scene> mScene;

		std::unordered_map<Vector3d, Node*> mSpawnedChunks;

	public:
		VoxerSystem(Context* ctx);
		~VoxerSystem();

		static VoxerSystem* Get()
		{
			return mInstance;
		}

		VoxerSettings* GetSettings() const
		{
			return mSettings;
		}

		void Update(std::vector< Vector3d>& playerPositions);

		ChunkProvider* GetChunkProvider();

		void Shutdown();

		void DestroyChunk(Chunk* c);

		void SpawnChunk(Chunk* c)
		{
			mChunksToSpawn.enqueue(c);
		}
	};
}