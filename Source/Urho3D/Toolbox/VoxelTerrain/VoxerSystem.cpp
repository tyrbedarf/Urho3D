#pragma once

#include "VoxerSystem.h"
#include "../../Scene/Scene.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/StaticModel.h"
#include "../../Graphics/Material.h"

namespace Urho3D
{
	VoxerSystem* VoxerSystem::mInstance = nullptr;

	VoxerSystem::VoxerSystem(Context* ctx) :
		Object(ctx)
	{
		mInstance = this;
		mSettings = new VoxerSettings(ctx);
		mTaskSystem = GetSubsystem<WorkQueue>();
		mChunkProvider = new ChunkProvider(ctx, mSettings);

		CreateCamera();
		mResourceCache = GetSubsystem<ResourceCache>();
	}

	VoxerSystem::~VoxerSystem()
	{

	}

	void VoxerSystem::CreateCamera()
	{
		mScene = new Scene(context_);
		mRootNode = mScene->CreateChild();
		mRootNode->SetName("Chunks");

		mOctree = mScene->CreateComponent<Octree>();
	}

	void VoxerSystem::Update(const Vector<Vector3d>& playerPositions)
	{
		mChunkProvider->Update(playerPositions);
		Chunk* chunk;
		auto dim = mSettings->GetChunkDimension();
		while (mChunksToSpawn.try_dequeue(chunk))
		{
			auto model = chunk->GetModel();
			if (model == nullptr)
			{
				continue;
			}

			auto pos = chunk->GetWorldPosition();
			auto name = pos.ToString();

			/*URHO3D_LOGDEBUG("Spawning chunk: " + name);*/

			auto planeNode = mScene->CreateChild();
			planeNode->SetName(name);

			auto* planeObject = planeNode->CreateComponent<StaticModel>();

			planeObject->SetModel(model);
			planeObject->SetMaterial(mResourceCache->GetResource<Material>("Materials/StoneTiled.xml"));

			planeNode->SetPosition(Vector3(pos.x, pos.y, pos.z));
			planeNode->SetRotation(Quaternion::IDENTITY);

			/// Already spawned?
			auto it = mSpawnedChunks.find(pos);
			if (it != mSpawnedChunks.end())
			{
				mScene->RemoveChild(it->second);
				mSpawnedChunks.erase(it);
			}

			mSpawnedChunks[pos] = planeNode;
			chunk->SetMeshInGame(true);
		}

		Vector3d v(0.0, 0.0, 0.0);
		while (mChunksToDespawn.try_dequeue(v))
		{
			auto it = mSpawnedChunks.find(v);
			if (it != mSpawnedChunks.end())
			{
				/*URHO3D_LOGDEBUG("Despawning chunk: " + it->second->GetName());*/
				mScene->RemoveChild(it->second);
				mSpawnedChunks.erase(it);
			}
		}
	}

	ChunkProvider* VoxerSystem::GetChunkProvider()
	{
		return mChunkProvider;
	}

	void VoxerSystem::Shutdown()
	{
		mChunkProvider->Shutdown();

	}

	void VoxerSystem::DestroyChunk(Chunk* c)
	{
		mChunksToDespawn.enqueue(c->GetWorldPosition());
	}
}