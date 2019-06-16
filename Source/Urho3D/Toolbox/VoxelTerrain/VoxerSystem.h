#pragma once

#include "../../Container/ConcurentQueue.h"
#include "../../Math/Vector3d.h"
#include "../../Scene/Node.h"
#include "../../Core/WorkQueue.h"
#include "../../Graphics/Octree.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/DebugRenderer.h"

#include <EASTL/hash_map.h>

#include "VoxerSettings.h"
#include "ChunkProvider.h"
#include "Chunk.h"


namespace Urho3D
{
	class VoxerSystem : public Object
	{
		URHO3D_OBJECT(VoxerSystem, Object)

	private:
		static VoxerSystem* mInstance;

		SharedPtr<VoxerSettings> mSettings;
		SharedPtr<ChunkProvider> mChunkProvider;
		SharedPtr<WorkQueue> mTaskSystem;
		moodycamel::ConcurrentQueue<Chunk*> mChunksToSpawn;
		moodycamel::ConcurrentQueue<Vector3d> mChunksToDespawn;

		SharedPtr<Scene> mScene;
		SharedPtr<Node> mRootNode;
		SharedPtr<Octree> mOctree;
		SharedPtr<ResourceCache> mResourceCache;
		SharedPtr<DebugRenderer> mDebugRenderer;

		eastl::hash_map<Vector3d, Node*> mSpawnedChunks;

		void CreateCamera();

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

		void Update(const Vector<Vector3d>& playerPositions);

		ChunkProvider* GetChunkProvider();

		/// TODO: Subscribe to shutdown event in order clean up properly.
		void Shutdown(StringHash eventType, VariantMap& eventData);

		/// Subscribe to post render update, to draw debug geometry
		void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

		/// Destroy a single chunk.
		void DestroyChunk(Chunk* c);

		void SpawnChunk(Chunk* c)
		{
			mChunksToSpawn.enqueue(c);
		}

		Scene* GetScene()
		{
			return mScene;
		}
	};
}