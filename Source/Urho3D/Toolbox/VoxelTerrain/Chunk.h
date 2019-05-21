#pragma once

#include "VoxerStatistics.h"
#include "../../Helper/Vector3Helper.h"
#include "../../Math/SimplexNoise.h"
#include "../../Core/Object.h"

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
		static VoxerStatistics* mStats;

	protected:
		const int VoxelCubeSize = 8;
		const Vector3i VoxelCube[8] =
		{
			Vector3i(0, 0, 0),
			Vector3i(1, 0, 0),
			Vector3i(0, 1, 0),
			Vector3i(0, 0, 1),

			Vector3i(1, 0, 1),
			Vector3i(1, 1, 0),
			Vector3i(0, 1, 1),
			Vector3i(1, 1, 1),
		};

		double mInitMarker;
		float mVoxelSize;
		Vector3d mWorldPosition;

		std::unordered_map<int, Chunk*> mNeighborhood;

		Voxel* mData;
		Vector3i mVoxelLayout;

		std::atomic<int> mInitialized;
		std::atomic<int> mInitializing;
		std::atomic<int> mMeshed;
		std::atomic<int> mMeshing;
		std::atomic<int> mBorderChunk;
		std::atomic<int> mMeshInGame;

		int GetNeighborHash(int x, int y, int z) const;

		int GetIndex(int x, int y, int z, Vector3i& neighborPosition) const;

		void SetIsBorderChunk(bool value)
		{
			mBorderChunk.store(value ? 1 : 0);
		}

	public:
		Chunk(Context* ctx, const Vector3i voxelLayout, float voxelSize) :
			Object(ctx),
			mInitMarker(0),
			mWorldPosition(0),
			mVoxelSize(voxelSize)
		{
			mData = nullptr;
			mVoxelLayout = voxelLayout;
			mStats = new VoxerStatistics();
		}

		~Chunk()
		{
			if (mData)
			{
				delete[] mData;
			}

			delete mStats;
		}

		const Vector3d& GetWorldPosition() const
		{
			return mWorldPosition;
		}

		bool GetInitialized() const
		{
			return false;
		}

		bool Initializing() const
		{
			return false;
		}

		bool Meshed() const
		{
			return false;
		}

		bool Meshing() const
		{
			return false;
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
		std::tuple<int, int> GetCubeIndexSafe(int x, int y, int z);

		virtual void Despawn()
		{

		}

		virtual void Set(const Voxel& data, int x, int y, int z);

		virtual Voxel& Get(int x, int y, int z, bool& found);

		virtual bool CanExtractSurface() const
		{
			return false;
		}
	};
}