#pragma once

#include "VoxerSystem.h"
#include "../../Scene/Scene.h"
#include "../../Graphics/Model.h"
#include "../../Graphics/StaticModel.h"
#include "../../Graphics/Material.h"
#include "../../Core/Profiler.h"
#include "../../Engine/EngineEvents.h"
#include "../../Core/CoreEvents.h"

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
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(VoxerSystem, HandlePostRenderUpdate));
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
		mDebugRenderer = mScene->CreateComponent<DebugRenderer>();
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
				mScene->RemoveChild(it->second);
				mSpawnedChunks.erase(it);
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

	void VoxerSystem::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
		mChunkProvider->DrawChunkBounds(mDebugRenderer);
	}
}