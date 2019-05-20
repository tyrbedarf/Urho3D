#pragma once

#include "VoxerSystem.h"
#include "Graphics/Octree.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"


namespace Urho3D
{
	VoxerSystem* VoxerSystem::mInstance = nullptr;

	VoxerSystem::VoxerSystem(Context* c) : Object(c)
	{
		mInstance = this;
		mSettings = new VoxerSettings(c);
		CreateScene();

		mResCache = GetSubsystem<ResourceCache>();

		mChunkProvider = new ChunkProvider(c, mSettings);
	}

	VoxerSystem::~VoxerSystem()
	{
		delete mSettings;
		delete mChunkProvider;
	}

	void VoxerSystem::CreateScene()
	{
		mScene = new Scene(context_);
		mScene->CreateComponent<Octree>();

		mSceneRoot = mScene->CreateChild();
		mSceneRoot->SetName("Root");
		mSceneRoot->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		mSceneRoot->SetRotation(Quaternion::IDENTITY);
		mSceneRoot->SetScale(Vector3(1.0f, 1.0f, 1.0f));
	}

	void VoxerSystem::Update(std::vector< Vector3d>& playerPositions)
	{
		mChunkProvider->Update(playerPositions);
		IChunk* chunk;
		auto dim = mSettings->GetChunkDimension();
		while (mChunksToSpawn.try_dequeue(chunk))
		{
			auto pos = chunk->GetWorldPosition();
			auto name = pos.ToString();

			auto plane = mSceneRoot->CreateChild();
			plane->SetName(name);
			auto planeModel = mResCache->GetResource<Model>("Models/Plane.mdl");
			auto planeObject = plane->CreateComponent<StaticModel>();
			planeObject->SetModel(planeModel);
			planeObject->SetMaterial(mResCache->GetResource<Material>("Materials/StoneTiled.xml"));

			plane->SetScale(Vector3(1.0f, 1.0f, 1.0f));
			plane->SetPosition(Vector3((float) pos.x, (float) pos.y, (float) pos.z));
			plane->SetRotation(Quaternion::IDENTITY);

			/// Already spawned?
			auto it = mSpawnedChunks.find(pos);
			if (it != mSpawnedChunks.end())
			{
				mScene->RemoveChild(it->second);
				mSpawnedChunks.erase(it);
			}

			mSpawnedChunks[pos] = plane;
			chunk->SetMeshInGame(true);
		}

		Vector3d v(0);
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
}