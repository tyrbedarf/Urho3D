#pragma once

#include <queue>

#include <EASTL/unordered_map.h>
#include <EASTL/queue.h>

#include "../../Math/Vector3d.h"
#include "../../Math/Vector3i.h"
#include "../../Core/Object.h"
#include "../../Core/WorkQueue.h"
#include "../../Graphics/DebugRenderer.h"
#include "../../Math/Color.h"

#include "VoxerSettings.h"
#include "Chunk.h"

namespace Urho3D
{
	class ChunkProvider : public Object
	{
		URHO3D_OBJECT(ChunkProvider, Object)

	private:
		SharedPtr<VoxerSettings> mSettings;
		SharedPtr<SurfaceData> mSurfaceData;

		eastl::unordered_map<Vector3d, SharedPtr<Chunk>> mActiveChunks;
		eastl::queue<Chunk*> mObjectPool;

		std::atomic<int>* mInitialing;
		std::atomic<int>* mMeshing;

		SharedPtr<WorkQueue> mTaskSystem;

		bool mDrawDebugGeometry;

		/// Console Commands
		void SubscribeToEvents();
		void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);
		void AddAutoComplete();

	public:
		ChunkProvider(Context* ctx, VoxerSettings* settings);

		void Update(const Vector<Vector3d>& playerPositions);
		void FinishUpdateCycle();
		void Shutdown();

		void SpawnChunks(const Vector<Vector3d>& playerPositions);

		/// <summary>
		/// A server must maintain chunks for more than one player. On the client side the list
		/// contains only one element.
		/// </summary>
		/// <param name="playerPositions"></param>
		void DespawnChunks(const Vector<Vector3d>& playerPositions);

		Vector3d NormalizeChunkPosition(const Vector3d& position) const;
		Vector3d NormalizeVoxelPosition(const Vector3d& position) const;

		/// <summary>
		/// Create a visible chunk or pull one from the list of buffered chunks.
		/// Newly created chunks are always marked as not buffered.
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		SharedPtr<Chunk> CreateChunk(Vector3d pos);

		/// <summary>
		/// Just return a chunk, do not create new ones.
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		SharedPtr<Chunk> GetChunk(Vector3d pos);

		Chunk* NewChunk();

		void DestroyChunk(const Vector3d pos);

		void ToggleDrawChunkBounds()
		{
			mDrawDebugGeometry = mDrawDebugGeometry ? false : true;
		}

		void DrawChunkBounds(SharedPtr<DebugRenderer> renderer) const;
	};
}