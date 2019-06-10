#pragma once

#include "VoxerSystem.h"
#include "../../Scene/Scene.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/StaticModel.h"
#include "../../Graphics/Material.h"
#include "../../Core/Profiler.h"
#include "../../Engine/EngineEvents.h"

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

		SubscribeToEvent(E_ENGINE_QUIT, URHO3D_HANDLER(VoxerSystem, Shutdown));
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

		URHO3D_PROFILE(UpdateVoxerSystem);

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
			auto it = mSpawnedChunks.Find(pos);
			if (it != mSpawnedChunks.End())
			{
				mScene->RemoveChild(it->second_);
				mSpawnedChunks.Erase(it);
			}

			mSpawnedChunks[pos] = planeNode;
			chunk->SetMeshInGame(true);
		}

		Vector3d v(0.0, 0.0, 0.0);
		while (mChunksToDespawn.try_dequeue(v))
		{
			auto it = mSpawnedChunks.Find(v);
			if (it != mSpawnedChunks.End())
			{
				/*URHO3D_LOGDEBUG("Despawning chunk: " + it->second->GetName());*/
				mScene->RemoveChild(it->second_);
				mSpawnedChunks.Erase(it);
			}
		}
	}

	ChunkProvider* VoxerSystem::GetChunkProvider()
	{
		return mChunkProvider;
	}

	void VoxerSystem::Shutdown(StringHash eventType, VariantMap& eventData)
	{
		/*mChunkProvider->Shutdown();*/
		using namespace EngineQuit;
		eventData[P_WAIT] = true;
	}

	void VoxerSystem::DestroyChunk(Chunk* c)
	{
		mChunksToDespawn.enqueue(c->GetWorldPosition());
	}
}