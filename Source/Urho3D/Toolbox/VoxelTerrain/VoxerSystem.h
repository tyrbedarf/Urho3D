#pragma once

#include "Container/ConcurentQueue.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Resource/ResourceCache.h"
#include "Core/Object.h"

#include "ChunkProvider.h"
#include "VoxerSettings.h"

#include <unordered_map>

namespace Urho3D
{
	class VoxerSystem : public Urho3D::Object
	{
		URHO3D_OBJECT(VoxerSystem, Urho3D::Object);

	private:
		static VoxerSystem* mInstance;

		SharedPtr<VoxerSettings> mSettings;
		SharedPtr<ChunkProvider> mChunkProvider;

		moodycamel::ConcurrentQueue<Chunk*> mChunksToSpawn;
		moodycamel::ConcurrentQueue<Vector3d> mChunksToDespawn;

		SharedPtr<Scene> mScene;
		SharedPtr<ResourceCache> mResCache;

		SharedPtr<Node> mSceneRoot;

		std::unordered_map<Vector3d, Node*> mSpawnedChunks;

		void CreateScene();

	public:
		VoxerSystem(Context* c);
		~VoxerSystem();

		static VoxerSystem* Get()
		{
			return mInstance;
		}

		static VoxerSettings* Settings()
		{
			return mInstance->GetSettings();
		}

		static ChunkProvider* ChunkProvider()
		{
			return mInstance->GetChunkProvider();
		}

		static void Spawn(Chunk* c)
		{
			mInstance->SpawnChunk(c);
		}

		static void Destroy(Chunk* c)
		{
			mInstance->DestroyChunk(c);
		}

		virtual VoxerSettings* GetSettings() const
		{
			return mSettings;
		}

		virtual ChunkProvider* GetChunkProvider() const
		{
			return mChunkProvider;
		}

		virtual void Update(std::vector< Vector3d>& playerPositions);

		virtual void Shutdown()
		{
			mChunkProvider->Shutdown();
		}

		virtual void SpawnChunk(Chunk* c)
		{
			mChunksToSpawn.enqueue(c);
		}

		virtual void DestroyChunk(Chunk* c)
		{
			mChunksToDespawn.enqueue(c->GetWorldPosition());
		}
	};
}