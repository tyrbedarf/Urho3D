#include "Chunk.h"

#include "VoxerSystem.h"

namespace Urho3D
{
	VoxerStatistics* Chunk::mStats = nullptr;

	int Chunk::GetNeighborHash(int x, int y, int z) const
	{
		int r = 0;

		r |= x < 0 ? 1 << 0 : 0;
		r |= y < 0 ? 1 << 1 : 0;
		r |= z < 0 ? 1 << 2 : 0;

		r |= x > 0 ? 1 << 3 : 0;
		r |= y > 0 ? 1 << 4 : 0;
		r |= z > 0 ? 1 << 5 : 0;

		return r;
	}

	int Chunk::GetIndex(int x, int y, int z, Vector3i& neighborPosition) const
	{
		bool this_chunk = true;
		neighborPosition = Vector3i(x, y, z);
		if (x < 0)
		{
			this_chunk = false;
			neighborPosition.x += mVoxelLayout.x;
		}

		if (y < 0)
		{
			this_chunk = false;
			neighborPosition.y += mVoxelLayout.y;
		}

		if (z < 0)
		{
			this_chunk = false;
			neighborPosition.z += mVoxelLayout.z;
		}

		if (x >= mVoxelLayout.x)
		{
			this_chunk = false;
			neighborPosition.x -= mVoxelLayout.x;
		}

		if (y >= mVoxelLayout.y)
		{
			this_chunk = false;
			neighborPosition.y -= mVoxelLayout.y;
		}

		if (z >= mVoxelLayout.z)
		{
			this_chunk = false;
			neighborPosition.z -= mVoxelLayout.z;
		}

		if (!this_chunk)
		{
			return -1;
		}

		return GetIndexFromVector(mVoxelLayout, x, y, z);
	}

	void Chunk::Reset(Vector3d pos)
	{
		mWorldPosition = pos;

		mInitialized.store(0);
		mInitializing.store(0);

		mMeshed.store(0);
		mMeshing.store(0);

		mMeshInGame.store(0);

		mNeighborhood.clear();
	}

	void Chunk::SetNeighbor(int x, int y, int z, Chunk* c)
	{
		auto hash = GetNeighborHash(x, y, z);
		if (hash == 0)
		{
			/// This is us.
			return;
		}

		auto it = mNeighborhood.find(hash);
		if (it == mNeighborhood.end());
		{
			mNeighborhood[hash] = c;
		}

		/// Did our status change from border to internal chunk?
		/// Than make sure we can remesh.
		if (IsBorderChunk() && mNeighborhood.size() >= 26 && GetInitialized())
		{
			mMeshing.store(0);
			mMeshed.store(0);
		}

		SetIsBorderChunk(mNeighborhood.size() < 26);
	}

	void Chunk::Initialize()
	{
		if (mInitializing.exchange(1) != 0)
		{
			/// Already initializing
			return;
		}

		mStats->AddInitialized();
		std::clock_t c_start = std::clock();
		auto t_start = std::chrono::high_resolution_clock::now();

		auto voxelSize = mVoxelSize;
		const float size = 0.05f;
		for (int x = 0; x < mVoxelLayout.x; x++)
		{
			for (int y = 0; y < mVoxelLayout.y; y++)
			{
				for (int z = 0; z < mVoxelLayout.z; z++)
				{
					auto voxel_pos = (Vector3d(x, y, z) * voxelSize) + mWorldPosition;
					auto height = SimplexNoise::noise((float)voxel_pos.x * size, (float)voxel_pos.z * size) * -4.0f;
					int index = GetIndexFromVector(mVoxelLayout, x, y, z);
					if (voxel_pos.y > height)
					{
						mData[index] = Voxel::GetAir();
					}
					else
					{
						mData[index] = Voxel::GetStone();
					}
				}
			}
		}

		if (mInitialized.exchange(1) != 0 != 0)
		{
			URHO3D_LOGERROR("Found a chunk that has been initialized twice.");
		}

		std::clock_t c_end = std::clock();
		auto t_end = std::chrono::high_resolution_clock::now();
		auto time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		mStats->AddInitTime(time);
	}

	void Chunk::CreateMesh()
	{
		if (mMeshing.exchange(1) != 0)
		{
			/// Already meshing
			return;
		}

		mStats->AddMeshed();
		std::clock_t c_start = std::clock();
		auto t_start = std::chrono::high_resolution_clock::now();

		int cube_index, move_vertex;
		for (int x = 0; x < mVoxelLayout.x; x++)
		{
			for (int y = 0; y < mVoxelLayout.y; y++)
			{
				for (int z = 0; z < mVoxelLayout.z; z++)
				{
					std::tie(cube_index, move_vertex) = GetCubeIndexSafe(x, y, z);

					/// Use the position inside the chunk as key for a node
					auto node_position = Vector3(x, y, z) * mVoxelSize;

					/// Than calculate the actual position and update the node position;
					//node_position += PointTable[cube_index];
					//mMesh.SetVertex(node_index, node_position);

					/// Tells us, which edges are being cut by the surface.
					/// This controlls which planes are being created.
					int plane_index = cube_index & 15;
					if (plane_index == 0 || plane_index == 15)
					{
						continue;
					}
				}
			}
		}

		VoxerSystem::Get()->SpawnChunk(this);

		if (mMeshed.exchange(1) != 0)
		{
			URHO3D_LOGERROR("Found a chunk that has been meshed twice.");
		}

		std::clock_t c_end = std::clock();
		auto t_end = std::chrono::high_resolution_clock::now();
		auto time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		mStats->AddMeshTime(time);
	}

	/// First one is cube index
	/// second indicates whether to move a vertex or not.
	std::tuple<int, int> Chunk::GetCubeIndexSafe(int x, int y, int z)
	{
		int cube_index = 0;
		int move_vertex = 1;
		Vector3i chun_pos(x, y, z);

		for (int i = 0; i < VoxelCubeSize; i++)
		{
			auto pos = VoxelCube[i] + chun_pos;
			bool found = false;
			auto voxel = Get(pos.x, pos.y, pos.z, found);
			if (!found)
			{
				return std::tuple<int, int>(0, 0);
			}

			cube_index |= voxel.IsTransparent() || voxel.IsAir() || voxel.IsModel() ? 1 << i : 0;

			/// If any of the voxel is a block do not move
			/// any of the neighboring nodes. This way we can combine
			/// smooth and block terrain.
			move_vertex &= voxel.IsBlock() ? 0 : 1;
		}

		return std::tuple<int, int>(cube_index, move_vertex);
	}

	void Chunk::Set(const Voxel& data, int x, int y, int z)
	{
		Vector3i neighborPos;
		auto index = GetIndex(x, y, z, neighborPos);
		if (index < 0)
		{
			auto x1 = x < 0 ? -1 : 0;
			x1 = x >= mVoxelLayout.x ? 1 : x1;

			auto y1 = y < 0 ? -1 : 0;
			y1 = y >= mVoxelLayout.y ? 1 : y1;

			auto z1 = z < 0 ? -1 : 0;
			z1 = z >= mVoxelLayout.z ? 1 : z1;

			auto hash = GetNeighborHash(x1, y1, z1);
			auto it = mNeighborhood.find(hash);

			if (it == mNeighborhood.end())
			{
				URHO3D_LOGERROR("Could not find a neighboring chunk.");
				return;
			}

			it->second->Set(data, neighborPos.x, neighborPos.y, neighborPos.z);
			return;
		}

		mData[index] = data;
	}

	Voxel& Chunk::Get(int x, int y, int z, bool& found)
	{
		Vector3i pos;
		auto index = GetIndex(x, y, z, pos);
		if (index < 0)
		{
			auto x1 = x < 0 ? -1 : 0;
			x1 = x >= mVoxelLayout.x ? 1 : x1;

			auto y1 = y < 0 ? -1 : 0;
			y1 = y >= mVoxelLayout.y ? 1 : y1;

			auto z1 = z < 0 ? -1 : 0;
			z1 = z >= mVoxelLayout.z ? 1 : z1;

			auto hash = GetNeighborHash(x1, y1, z1);
			auto it = mNeighborhood.find(hash);
			if (it == mNeighborhood.end())
			{
				found = false;
				return Voxel::GetAir();
			}

			if (it->second == nullptr)
			{
				found = false;
				return Voxel::GetAir();
			}

			found = true;
			return it->second->Get(pos.x, pos.y, pos.z, found);
		}

		if (mData == nullptr)
		{
			found = false;
			return Voxel::GetAir();
		}

		found = true;
		return mData[index];
	}
}