#pragma once

#include <unordered_map>
#include <queue>

#include "../Core/Object.h"
#include "../Core/WorkQueue.h"

#include "Chunk.h"
#include "VoxerSettings.h"

namespace Urho3D
{
	bool chunkOrder(const Chunk* lhs, const Chunk* rhs);

	class ChunkProvider : public Object
	{
		URHO3D_OBJECT(ChunkProvider, Object)

	private:
		SharedPtr<VoxerSettings> mSettings;

		std::unordered_map<Vector3d, Chunk*> mActiveChunks;
		std::queue<Chunk*> mObjectPool;

		std::atomic<int>* mInitialing;
		std::atomic<int>* mMeshing;

		SharedPtr<WorkQueue> mTaskSystem;

	public:
		ChunkProvider(Context* ctx, SharedPtr<VoxerSettings> settings);

		void Update(const std::vector<Vector3d>& playerPositions);
		void FinishUpdateCycle();
		void Shutdown();

		void SpawnChunks(const std::vector<Vector3d>& playerPositions);

		/// <summary>
		/// A server must maintain chunks for more than one player. On the client side the list
		/// contains only one element.
		/// </summary>
		/// <param name="playerPositions"></param>
		void DespawnChunks(const std::vector<Vector3d>& playerPositions);

		Vector3d NormalizeChunkPosition(const Vector3d& position) const;

		Vector3d NormalizeVoxelPosition(const Vector3d& position) const;

		/// <summary>
		/// Create a visible chunk or pull one from the list of buffered chunks.
		/// Newly created chunks are always marked as not buffered.
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		Chunk* CreateChunk(Vector3d pos);

		/// <summary>
		/// Just return a chunk, do not create new ones.
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		Chunk* GetChunk(Vector3d pos);

		Chunk* NewChunk();

		void DestroyChunk(const Vector3d pos)
		{
			auto it = mActiveChunks.find(pos);
			mObjectPool.push(it->second);
			mActiveChunks.erase(it);
		}
	};
}