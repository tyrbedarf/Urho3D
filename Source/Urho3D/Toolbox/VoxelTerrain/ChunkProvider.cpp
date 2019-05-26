#include "ChunkProvider.h"

#include <array>

namespace Urho3D
{
	bool chunkOrder(const Chunk* lhs, const Chunk* rhs)
	{
		return lhs->GetInitializationMarker() < rhs->GetInitializationMarker();
	}

	ChunkProvider::ChunkProvider(Context* ctx, VoxerSettings* settings) :
		Object(ctx)
	{
		mSettings = settings;
		mTaskSystem = GetSubsystem<WorkQueue>();

		mInitialing = nullptr;
		mMeshing = nullptr;
	}

	void ChunkProvider::Update(const Vector<Vector3d>& playerPositions)
	{
		SpawnChunks(playerPositions);
		DespawnChunks(playerPositions);
	}

	void ChunkProvider::Shutdown()
	{
		/// Wait for all tasks to finish
		URHO3D_LOGDEBUG("Waiting for all pending tasks to finish.");

		/// Remove all Chunks
		URHO3D_LOGDEBUG("Destroying active chunks");
		for (std::unordered_map<Vector3d, Chunk*>::iterator it = mActiveChunks.begin();
			it != mActiveChunks.end();
			++it)
		{
			auto c = it->second;
			delete c;
		}

		mActiveChunks.clear();

		URHO3D_LOGDEBUG("Cleaning up object pool");
		while (mObjectPool.size() > 0)
		{
			auto c = mObjectPool.front();
			delete c;
			mObjectPool.pop();
		}

		URHO3D_LOGDEBUG("Chunk provider out!");
	}

	void ChunkProvider::SpawnChunks(const Vector<Vector3d>& playerPositions)
	{
		if (playerPositions.Size() < 1)
		{
			return;
		}

		if (mInitialing != nullptr)
		{
			return;
		}

		mInitialing = new std::atomic<int>();
		mMeshing = new std::atomic<int>();

		int positions = mSettings->IsServer() ? playerPositions.Size() : 1;
		Vector3d cd = mSettings->GetChunkDimension();
		Vector3i vr = mSettings->GetViewRange();
		std::vector<Chunk*> Workload;

		for (int i = 0; i < positions; i++)
		{
			auto np = NormalizeChunkPosition(playerPositions[i]);

			double startx = np.x - (cd.x * vr.x);
			double starty = np.y - (cd.y * vr.y);
			double startz = np.z - (cd.z * vr.z);

			double stopx = np.x + (cd.x * (float) vr.x);
			double stopy = np.y + (cd.y * (float) vr.y);
			double stopz = np.z + (cd.z * (float) vr.z);

			for (double x2 = startx; x2 < stopx; x2 += cd.x)
			{
				for (double y2 = starty; y2 < stopy; y2 += cd.y)
				{
					for (double z2 = startz; z2 < stopz; z2 += cd.z)
					{
						auto position = NormalizeChunkPosition(Vector3d(x2, y2, z2));
						auto dist = (position - playerPositions[i]).SqrMagnitude();
						auto ch = CreateChunk(position);

						if (ch == nullptr)
						{
							continue;
						}

						ch->SetInitializationMarker(dist);

						auto dir = position - np;
						auto key = dir.SqrMagnitude();
						Workload.push_back(ch);
					}
				}
			}
		}

		std::sort(Workload.begin(), Workload.end(), chunkOrder);
		std::vector<Chunk*> init_tasks;
		for (int i = 0; i < Workload.size(); i++)
		{
			/// Setup neighborhood for each chunk
			auto c = Workload[i];
			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					for (int z = -1; z <= 1; z++)
					{
						auto px = (double)x * cd.x;
						auto py = (double)y * cd.y;
						auto pz = (double)z * cd.z;
						auto pos = Vector3d(px, py, pz) + c->GetWorldPosition();
						auto neighbor = GetChunk(pos);

						if (neighbor == nullptr)
						{
							continue;
						}

						c->SetNeighbor(x, y, z, neighbor);
					}
				}
			}

			/// Add initializer task
			mTaskSystem->AddTask(
				[](void* data)
				{
					auto chunk = reinterpret_cast<Chunk*>(data);
					chunk->Initialize();
				},
				c,
				mInitialing,
				nullptr);

			init_tasks.push_back(c);
		}


		///extract the surface.
		/// Use task dependencies to make sure all chunks have been initialized
		/// before we try to extract the surface.
		for (int i = 0; i < init_tasks.size(); i++)
		{
			auto c = init_tasks[i];
			if (c->Meshing())
			{
				continue;
			}

			mTaskSystem->AddTask(
				[](void* data)
				{
					auto chunk = reinterpret_cast<Chunk*>(data);
					chunk->CreateMesh();
				},
				c,
				mMeshing,
				mInitialing);
		}

		/// Finally clean up, otherwise this metho runs only once
		mTaskSystem->AddTask(
			[](void* data)
			{
				auto cp = reinterpret_cast<ChunkProvider*>(data);
				cp->FinishUpdateCycle();
			},
			this,
			nullptr,
			mMeshing);

	}

	void ChunkProvider::FinishUpdateCycle()
	{
		delete mInitialing;
		delete mMeshing;

		mInitialing = nullptr;
		mMeshing = nullptr;
	}

	void ChunkProvider::DespawnChunks(const Vector<Vector3d>& playerPositions)
	{
		/// Placed inside Settings since it depends on voxel size, voxel count and
		/// the view range.
		double maxDist = mSettings->GetDistToDestroy();

		std::vector<Vector3d> keys;
		for (std::unordered_map<Vector3d, Chunk*>::iterator it = mActiveChunks.begin();
			it != mActiveChunks.end();
			++it)
		{
			keys.push_back(it->first);
		}

		for (int j = 0; j < keys.size(); j++)
		{
			bool destroy = true;
			auto c = mActiveChunks[keys[j]];
			for (int i = 0; i < playerPositions.Size(); i++)
			{
				if (!c->Initialized() || !c->Meshed() || !c->IsMeshInGame())
				{
					destroy = false;
					break;
				}

				/// Remove everything that is further away than max distance.
				auto dist = (keys[j] - playerPositions[i]).SqrMagnitude();
				if (dist < maxDist)
				{
					destroy = false;
					break;
				}
			}

			if (destroy)
			{
				DestroyChunk(keys[j]);
			}
		}
	}

	Vector3d ChunkProvider::NormalizeChunkPosition(const Vector3d& position) const
	{
		auto ChunkDimension = mSettings->GetChunkDimension();

		double x = std::floor(position.x / ChunkDimension.x) * ChunkDimension.x;
		double y = std::floor(position.y / ChunkDimension.y) * ChunkDimension.y;
		double z = std::floor(position.z / ChunkDimension.z) * ChunkDimension.z;

		return Vector3d(x, y, z);
	}

	Vector3d ChunkProvider::NormalizeVoxelPosition(const Vector3d& position) const
	{
		auto VoxelDimension = mSettings->GetVoxelSize();
		double x = std::floor(position.x / VoxelDimension) * VoxelDimension;
		double y = std::floor(position.y / VoxelDimension) * VoxelDimension;
		double z = std::floor(position.z / VoxelDimension) * VoxelDimension;

		return Vector3d(x, y, z);
	}

	Chunk* ChunkProvider::CreateChunk(Vector3d pos)
	{
		Chunk* r = nullptr;
		auto it = mActiveChunks.find(pos);
		if (it != mActiveChunks.end())
		{
			/// Is it a former border chunk?
			/// We will know after all chunks of this batch have been collected
			/// and the neighbors have been set up. So return all border chunks here
			/// to make sure meshing can take place again
			r = it->second;
			if (r->IsBorderChunk())
			{
				return r;
			}

			return nullptr;
		}

		r = NewChunk();
		r->Reset(pos);
		mActiveChunks.insert(std::pair<Vector3d, Chunk*>(pos, r));

		return r;
	}

	Chunk* ChunkProvider::GetChunk(Vector3d pos)
	{
		auto it = mActiveChunks.find(pos);
		if (it != mActiveChunks.end())
		{
			return it->second;
		}

		return nullptr;
	}

	Chunk* ChunkProvider::NewChunk()
	{
		if (mObjectPool.empty())
		{
			return new Chunk(context_, mSettings->GetVoxelCount(), mSettings->GetVoxelSize());
		}

		auto r = mObjectPool.front();
		mObjectPool.pop();

		return r;
	}

	void ChunkProvider::DestroyChunk(const Vector3d pos)
	{
		auto it = mActiveChunks.find(pos);
		auto c = it->second;
		c->Despawn();
		mObjectPool.push(c);
		mActiveChunks.erase(it);
	}
}