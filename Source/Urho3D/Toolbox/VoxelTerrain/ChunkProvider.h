#pragma once

#include <unordered_map>
#include <queue>

#include "API/IVoxerSettings.h"
#include "API/IChunk.h"
#include "API/IChunkProvider.h"
#include "API/IVoxerSystem.h"

#include "Core/Object.h"
#include "Core/WorkQueue.h"
#include "TaskSystem.h"

#include "Chunk.h"

namespace Urho3D
{
	bool chunkOrder(const IChunk* lhs, const IChunk* rhs);

	class ChunkProvider : public IChunkProvider
	{
		URHO3D_OBJECT(ChunkProvider, IChunkProvider)

	private:
		IVoxerSettings* mSettings;

		std::unordered_map<Vector3d, IChunk*> mActiveChunks;
		std::queue<IChunk*> mObjectPool;

		std::atomic<int>* mInitialing;
		std::atomic<int>* mMeshing;

		SharedPtr<TaskSystem> mTaskSystem;

	public:
		ChunkProvider(Context* ctx, IVoxerSettings* settings);

		virtual void Update(const std::vector<Vector3d>& playerPositions);
		virtual void FinishUpdateCycle();
		virtual void Shutdown();

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
		IChunk* CreateChunk(Vector3d pos);

		/// <summary>
		/// Just return a chunk, do not create new ones.
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		IChunk* GetChunk(Vector3d pos);

		IChunk* NewChunk();

		void DestroyChunk(const Vector3d pos)
		{
			auto it = mActiveChunks.find(pos);
			mObjectPool.push(it->second);
			mActiveChunks.erase(it);
		}
	};
}