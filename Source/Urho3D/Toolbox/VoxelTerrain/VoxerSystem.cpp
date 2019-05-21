#pragma once

#include "VoxerSystem.h"
#include "../../Scene/Scene.h"

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
	}

	VoxerSystem::~VoxerSystem()
	{
		delete mSettings;
		delete mChunkProvider;
	}

	void VoxerSystem::Update(std::vector< Vector3d>& playerPositions)
	{
		/*mChunkProvider->Update(playerPositions);
		Chunk* chunk;
		auto dim = mSettings->GetChunkDimension();
		while (mChunksToSpawn.try_dequeue(chunk))
		{
			Plane plane(Vector3::UNIT_Y, 0);

			auto pos = chunk->GetWorldPosition();
			auto name = pos.ToString();

			MeshManager::getSingleton().createPlane(
				name,
				RGN_DEFAULT,
				plane,
				dim.x,
				dim.z,
				20,
				20,
				true,
				1,
				5,
				5,
				Vector3::UNIT_Z);

			Entity* groundEntity = mSceneManager->createEntity(name);
			auto node = mSceneManager->getRootSceneNode()->createChildSceneNode();
			node->attachObject(groundEntity);
			node->setPosition(pos.x, pos.y, pos.z);

			groundEntity->setCastShadows(false);
			groundEntity->setMaterialName("Examples/Rockwall");

			/// Already spawned?
			auto it = mSpawnedChunks.find(pos);
			if (it != mSpawnedChunks.end())
			{
				auto parent = it->second->getParentSceneNode();
				parent->detachObject(it->second);
				mSceneManager->destroyEntity(it->second);
				mSpawnedChunks.erase(it);
			}

			mSpawnedChunks[pos] = groundEntity;
			chunk->SetMeshInGame(true);*/
		/*}

		Vector3d v(0);
		while (mChunksToDespawn.try_dequeue(v))
		{

		}*/
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