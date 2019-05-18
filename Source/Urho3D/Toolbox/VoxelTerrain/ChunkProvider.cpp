#include "ChunkProvider.h"

#include <array>
#include "Task.h"
#include "TaskSystem.h"
#include "Core/Context.h"
#include "Core/Object.h"

#include <unordered_map>

namespace Urho3D
{
	bool chunkOrder(const IChunk* lhs, const IChunk* rhs)
	{
		return lhs->GetInitializationMarker() < rhs->GetInitializationMarker();
	}

	ChunkProvider::ChunkProvider(Context* ctx, IVoxerSettings* settings) :
		IChunkProvider(ctx)
	{
		mSettings = settings;
		mTaskSystem = GetSubsystem<TaskSystem>();

		mInitialing = nullptr;
		mMeshing = nullptr;
	}

	void ChunkProvider::Update(const std::vector<Vector3d>& playerPositions)
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
		for (std::unordered_map<Vector3d, IChunk*>::iterator it = mActiveChunks.begin();
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

	void ChunkProvider::SpawnChunks(const std::vector<Vector3d>& playerPositions)
	{
		if (playerPositions.size() < 1)
		{
			return;
		}

		if (mInitialing != nullptr)
		{
			return;
		}

		mInitialing = new std::atomic<int>();
		mMeshing = new std::atomic<int>();

		int positions = mSettings->IsServer() ? playerPositions.size() : 1;
		auto cd = mSettings->GetChunkDimension();
		auto vr = mSettings->GetViewRange();
		std::vector<IChunk*> Workload;

		for (int i = 0; i < positions; i++)
		{
			auto np = NormalizeChunkPosition(playerPositions[i]);

			double startx = np.x - (cd.x * vr.x);
			double starty = np.y - (cd.y * vr.y);
			double startz = np.z - (cd.z * vr.z);

			double stopx = np.x + (cd.x * (float)vr.x);
			double stopy = np.y + (cd.y * (float)vr.y);
			double stopz = np.z + (cd.z * (float)vr.z);

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
		std::vector<IChunk*> init_tasks;
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
					auto chunk = reinterpret_cast<IChunk*>(data);
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
					auto chunk = reinterpret_cast<IChunk*>(data);
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
				auto cp = reinterpret_cast<IChunkProvider*>(data);
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

	void ChunkProvider::DespawnChunks(const std::vector<Vector3d>& playerPositions)
	{
		/// Placed inside Settings since it depends on voxel size, voxel count and
		/// the view range.
		double maxDist = mSettings->GetDistToDestroy();

		std::vector<Vector3d> keys;
		for (std::unordered_map<Vector3d, IChunk*>::iterator it = mActiveChunks.begin();
			it != mActiveChunks.end();
			++it)
		{
			keys.push_back(it->first);
		}

		for (int j = 0; j < keys.size(); j++)
		{
			bool destroy = true;
			auto c = mActiveChunks[keys[j]];
			for (int i = 0; i < playerPositions.size(); i++)
			{
				if (!c->GetInitialized() || !c->Meshed() || !c->IsMeshInGame())
				{
					destroy = false;
					break;
				}

				/// Remove everything that further away than max distance.
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

	IChunk* ChunkProvider::CreateChunk(Vector3d pos)
	{
		IChunk* r = nullptr;
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
		mActiveChunks.insert(std::pair<Vector3d, IChunk*>(pos, r));

		return r;
	}

	IChunk* ChunkProvider::GetChunk(Vector3d pos)
	{
		auto it = mActiveChunks.find(pos);
		if (it != mActiveChunks.end())
		{
			return it->second;
		}

		return nullptr;
	}

	IChunk* ChunkProvider::NewChunk()
	{
		if (mObjectPool.empty())
		{
			return new Chunk(context_, mSettings->GetVoxelCount(), mSettings->GetVoxelSize());
		}

		auto r = mObjectPool.front();
		mObjectPool.pop();

		return r;
	}

	/*void ChunkProvider::InitChunkTask(ftl::TaskScheduler *taskScheduler, void *arg)
	{
		auto chunk = reinterpret_cast<IChunk *>(arg);
		chunk->Initialize();
	}

	void ChunkProvider::CreateMeshTask(ftl::TaskScheduler *taskScheduler, void *arg)
	{
		auto chunk = reinterpret_cast<IChunk *>(arg);
		taskScheduler->WaitForCounter(chunk->GetFiberBarier(), 0);

		chunk->CreateMesh();
	}

	void ChunkProvider::ResetCounterTask(ftl::TaskScheduler *taskScheduler, void *arg)
	{
		auto cp = reinterpret_cast<ChunkProvider*>(arg);
		taskScheduler->WaitForCounter(cp->mInitTaskCounter, 0);
		taskScheduler->WaitForCounter(cp->mMeshTaskCounter, 0);

		delete cp->mInitTaskCounter;
		delete cp->mMeshTaskCounter;
		cp->mInitTaskCounter = nullptr;
		cp->mMeshTaskCounter = nullptr;
	}*/
}