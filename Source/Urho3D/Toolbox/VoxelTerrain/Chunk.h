#pragma once

#include "../../Container/EaStlAllocator.h"
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>

#include "VoxerStatistics.h"
#include "../../Helper/Vector3Helper.h"
#include "../../Math/SimplexNoise.h"
#include "../../Core/Object.h"
#include "../../Toolbox/Mesh/ProceduralMesh.h"

#include "SurfaceData.h"

#include <tuple>
#include <atomic>
#include <chrono>
#include <ctime>

#include "Voxel.h"

namespace Urho3D
{
	class Chunk : public Object
	{
		URHO3D_OBJECT(Chunk, Object)

	public:
		static VoxerStatistics* Stats;

	protected:
		double mInitMarker;
		float mVoxelSize;
		Vector3d mWorldPosition;

		eastl::hash_map<int, Chunk*> mNeighborhood;
		eastl::vector<Voxel> mData;

		Vector3i mVoxelLayout;

		Voxel mLastVoxel;
		bool isAir;
		bool isSolid;

		/// READONLY!
		const SurfaceData* mSurfaceData;

		std::atomic<int> mInitialized;
		std::atomic<int> mInitializing;
		std::atomic<int> mMeshed;
		std::atomic<int> mMeshing;
		std::atomic<int> mBorderChunk;
		std::atomic<int> mMeshInGame;

		SharedPtr<ProceduralMesh> mMesh;

		int GetNeighborHash(int x, int y, int z) const;

		int GetIndex(int x, int y, int z, Vector3i& neighborPosition) const;

		void SetIsBorderChunk(bool value)
		{
			mBorderChunk.store(value ? 1 : 0);
		}

	public:
		Chunk(
			Context* ctx,
			const Vector3i voxelLayout,
			float voxelSize,
			const SurfaceData* surfData) :
			Object(ctx),
			mInitMarker(0),
			mWorldPosition(0),
			mVoxelSize(voxelSize),
			mSurfaceData(surfData)
		{
			mVoxelLayout = voxelLayout;
			mData.resize(mVoxelLayout.GetArrayCount(), Voxel::GetAir());
			mMesh = new ProceduralMesh(context_);
		}

		~Chunk()
		{
		}

		const Vector3d& GetWorldPosition() const
		{
			return mWorldPosition;
		}

		bool Initialized() const
		{
			return mInitialized.load() > 0;
		}

		bool Initializing() const
		{
			return mInitializing.load() > 0;
		}

		bool Meshed() const
		{
			return mMeshed.load() > 0;
		}

		bool Meshing() const
		{
			return mMeshing.load() > 0;
		}

		bool IsMeshInGame() const
		{
			return mMeshInGame.load() > 0;
		}

		void SetMeshInGame(bool value)
		{
			mMeshInGame.store(value ? 1 : 0);
		}

		double GetInitializationMarker() const
		{
			return mInitMarker;
		}

		void SetInitializationMarker(double value)
		{
			mInitMarker = value;
		}

		bool IsBorderChunk() const
		{
			return mBorderChunk.load() > 0;
		}

		void Reset(Vector3d pos);

		void SetNeighbor(int x, int y, int z, Chunk* c);
		void Initialize();
		void CreateMesh();

		/// First one is cube index
		/// second indicates whether to move a vertex or not.
		std::tuple<int, int> GetCube(int x, int y, int z, bool safe = true);

		void Despawn();

		void Set(const Voxel& data, int x, int y, int z, bool safe = false);

		std::tuple<Voxel&, bool> Get(int x, int y, int z, bool safe = true);

		void HandleVoxelUpdate(Voxel v);

		bool CanExtractSurface() const
		{
			return false;
		}

		SharedPtr<Model> GetModel()
		{
			return mMesh->GetModel();
		}
	};
}