#include "Chunk.h"

#include "VoxerSystem.h"
#include <sstream>

namespace Urho3D
{
	VoxerStatistics* Chunk::Stats = nullptr;

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

		mNeighborhood.Clear();
		mMesh->Clear();

		Voxel mLastVoxel = Voxel::GetAir();
		isAir = true;
		isSolid = false;
	}

	void Chunk::SetNeighbor(int x, int y, int z, Chunk* c)
	{
		auto hash = GetNeighborHash(x, y, z);
		if (hash == 0)
		{
			/// This is us.
			return;
		}

		auto it = mNeighborhood.Find(hash);
		if (it == mNeighborhood.End());
		{
			mNeighborhood[hash] = c;
		}

		/// Did our status change from border to internal chunk?
		/// Than make sure we can remesh.
		if (IsBorderChunk() && mNeighborhood.Size() >= 26 && Initialized())
		{
			mMeshing.store(0);
			mMeshed.store(0);
		}

		SetIsBorderChunk(mNeighborhood.Size() < 26);
	}

	void Chunk::Initialize()
	{
		if (mInitializing.exchange(1) != 0)
		{
			/// Already initializing
			return;
		}

		Stats->AddInitialized();
		std::clock_t c_start = std::clock();
		auto t_start = std::chrono::high_resolution_clock::now();

		auto voxelSize = mVoxelSize;
		const float size = 0.05f;
		const Vector3i tmp(2, 2, 2);
		for (int x = 0; x < mVoxelLayout.x; x++)
		{
			for (int y = 0; y < mVoxelLayout.y; y++)
			{
				for (int z = 0; z < mVoxelLayout.z; z++)
				{
					auto voxel_pos = (Vector3d(x, y, z) * voxelSize) + mWorldPosition;
					auto height = SimplexNoise::noise((float)voxel_pos.x * size, (float)voxel_pos.z * size) * -2.5f;
					if (voxel_pos.y > height)
					{
						Set(Voxel::GetAir(), x, y, z, true);
					}
					else
					{
						Set(Voxel::GetStone(), x, y, z, true);
					}
				}
			}
		}

		if (mInitialized.exchange(1) != 0)
		{
			URHO3D_LOGERROR("Found a chunk that has been initialized twice.");
		}

		std::clock_t c_end = std::clock();
		auto t_end = std::chrono::high_resolution_clock::now();
		auto time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		Stats->AddInitTime(time);
	}

	void Chunk::CreateMesh()
	{
		if (mMeshing.exchange(1) != 0)
		{
			/// Already meshing
			return;
		}

		Stats->AddMeshed();
		std::clock_t c_start = std::clock();
		auto t_start = std::chrono::high_resolution_clock::now();
		mMesh->Clear();

		/// If the entire neighborhood is air as well dont bother to create a mesh.
		bool createMesh = true;
		if (isAir)
		{
			for (auto it = mNeighborhood.Begin(); it != mNeighborhood.End(); it++)
			{
				if (!it->second_->isAir)
				{
					createMesh = false;
					break;
				}
			}
		}

		if (!createMesh)
		{
			if (mMeshed.exchange(1) != 0)
			{
				URHO3D_LOGERROR("Found a chunk that has been meshed twice.");
			}

			Stats->AddEmptyChunksSkipped();
			URHO3D_LOGDEBUG("Empty chunk surrounded by empty chunks.");

			return;
		}

		/*String st = (isSolid ? "yes" : "no");
		URHO3D_LOGDEBUG(String("Is solid? ") + st);*/

		/// If all surrounding chunks are solid, dont bother creating a mesh.
		createMesh = false;
		for (auto it = mNeighborhood.Begin(); it != mNeighborhood.End(); it++)
		{
			if (!it->second_->isSolid)
			{
				createMesh = true;
				break;
			}
		}
		if (!createMesh)
		{
			if (mMeshed.exchange(1) != 0)
			{
				URHO3D_LOGERROR("Found a chunk that has been meshed twice.");
			}

			Stats->AddSolidChunksSkipped();
			URHO3D_LOGDEBUG("Surrounded by solid chunks only");

			return;
		}

		int indices[6];
		int cube_index, move_vertex;

		/// By default every vertex is right in the middle of the voxel cube.
		Vector3 vertexOffset(mVoxelSize * 0.5f, mVoxelSize * 0.5f, mVoxelSize * 0.5f);

		for (int x = 0; x < mVoxelLayout.x; x++)
		{
			for (int y = 0; y < mVoxelLayout.y; y++)
			{
				for (int z = 0; z < mVoxelLayout.z; z++)
				{
					std::tie(cube_index, move_vertex) = GetCube(x, y, z, false);

					/// Key of the vertex. Essentially the position inside the chunk.
					Vector3i pos(x, y, z);

					/// Make sure the vertex is generated.
					Vector3 actual_pos = (Vector3(x, y, z) * mVoxelSize) + vertexOffset;

					/// Than calculate the actual position and update the node position;
					/// node_position += PointTable[cube_index];
					mMesh->GetIndex(actual_pos);

					/// Tells us, which edges are being cut by the surface.
					/// This controlls which planes are being created.
					int plane_index = cube_index & 15;
					if (plane_index == 0 || plane_index == 15)
					{
						continue;
					}

					/// For each side we need two triangles, plus one material.
					for (int i = 0;
						mSurfaceData->CubeIndexToFaceLookup.Get(plane_index, i) != mSurfaceData->STOP;
						i += 7)
					{
						/// For each vertex of a quad.
						for (int j = 0; j < 6; j++)
						{
							auto item_0 = mSurfaceData->Vertices[mSurfaceData->CubeIndexToFaceLookup.Get(plane_index, i + j)] + pos;
							indices[j] = mMesh->GetIndex
							(
								(Vector3(item_0.x, item_0.y, item_0.z) * mVoxelSize) +
								vertexOffset
							);
						}

						mMesh->AddTriangleFromIndices(indices[1], indices[0], indices[2], true);
						mMesh->AddTriangleFromIndices(indices[4], indices[3], indices[5], true);
					}
				}
			}
		}

		std::clock_t c_end = std::clock();
		auto t_end = std::chrono::high_resolution_clock::now();
		auto time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		Stats->AddMeshTime(time);

		c_start = std::clock();
		t_start = std::chrono::high_resolution_clock::now();

		mMesh->SimplifyMeshLossless(false, 100);

		c_end = std::clock();
		t_end = std::chrono::high_resolution_clock::now();
		time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		Stats->AddSimplifyMeshTime(time);

		if (mMesh->GetVertexCount() > 0)
		{
			VoxerSystem::Get()->SpawnChunk(this);
		}

		if (mMeshed.exchange(1) != 0)
		{
			URHO3D_LOGERROR("Found a chunk that has been meshed twice.");
		}
	}

	void Chunk::Despawn()
	{
		VoxerSystem::Get()->DestroyChunk(this);
	}

	/// First one is cube index
	/// second indicates whether to move a vertex or not.
	std::tuple<int, int> Chunk::GetCube(int x, int y, int z, bool safe)
	{
		int cube_index = 0;
		int move_vertex = 1;
		Vector3i chun_pos(x, y, z);

		for (int i = 0; i < mSurfaceData->VoxelCubeSize; i++)
		{
			auto pos = mSurfaceData->VoxelCube[i] + chun_pos;
			bool found = false;
			Voxel voxel;
			std::tie(voxel, found) = Get(pos.x, pos.y, pos.z, safe);
			if (!found)
			{
				return std::tuple<int, int>(0, 0);
			}

			cube_index |= voxel.IsTransparent() || voxel.IsAir() || voxel.IsModel() ? 0 << i : 1 << i;

			/// If any of the voxel is a block do not move
			/// any of the neighboring nodes. This way we can combine
			/// smooth and block terrain.
			move_vertex &= voxel.IsBlock() ? 0 : 1;
		}

		return std::tuple<int, int>(cube_index, move_vertex);
	}

	void Chunk::Set(const Voxel& data, int x, int y, int z, bool safe)
	{
		if (safe)
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
				auto it = mNeighborhood.Find(hash);

				if (it == mNeighborhood.End())
				{
					URHO3D_LOGERROR("Could not find a neighboring chunk.");
					return;
				}

				it->second_->Set(data, neighborPos.x, neighborPos.y, neighborPos.z);
				return;
			}

			mData[index] = data;
			HandleVoxelUpdate(data);
		}
		else
		{
			auto index = mVoxelLayout.GetIndex(x, y, z);
			mData[index] = data;
			HandleVoxelUpdate(data);
		}
	}

	void Chunk::HandleVoxelUpdate(Voxel v)
	{
		if (v.GetId() != mLastVoxel.GetId() && !isAir)
		{
			isSolid = false;
		}

		if (!v.IsAir())
		{
			isAir = false;
			isSolid = true;
		}

		mLastVoxel = v;
	}

	std::tuple<Voxel&, bool> Chunk::Get(int x, int y, int z, bool safe)
	{
		Vector3i pos;
		auto index = GetIndex(x, y, z, pos);

		if (safe)
		{
			return std::tuple<Voxel&, bool>(mData[index], true);
		}

		if (index < 0)
		{
			auto x1 = x < 0 ? -1 : 0;
			x1 = x >= mVoxelLayout.x ? 1 : x1;

			auto y1 = y < 0 ? -1 : 0;
			y1 = y >= mVoxelLayout.y ? 1 : y1;

			auto z1 = z < 0 ? -1 : 0;
			z1 = z >= mVoxelLayout.z ? 1 : z1;

			auto hash = GetNeighborHash(x1, y1, z1);
			auto it = mNeighborhood.Find(hash);
			if (it == mNeighborhood.End())
			{
				return std::tuple<Voxel&, bool>(Voxel::GetAir(), false);
			}

			if (it->second_ == nullptr)
			{
				return std::tuple<Voxel&, bool>(Voxel::GetAir(), false);
			}

			return it->second_->Get(pos.x, pos.y, pos.z, safe);
		}

		return std::tuple<Voxel&, bool>(mData[index], true);
	}
}