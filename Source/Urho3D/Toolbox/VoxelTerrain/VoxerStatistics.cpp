#include "VoxerStatistics.h"

namespace Urho3D
{
	VoxerStatistics::VoxerStatistics()
	{

	}

	void VoxerStatistics::AddInitialized()
	{
		++mInitializedChunks;
	}

	void VoxerStatistics::AddMeshed()
	{
		++mMeshedChunks;
	}

	void VoxerStatistics::AddInitTime(double time)
	{
		mInitTime.fetch_add((long)time);
	}

	void VoxerStatistics::AddMeshTime(double time)
	{
		mInitTime.fetch_add((long)time);
	}

	int VoxerStatistics::GetInitialized() const
	{
		return mInitializedChunks.load();
	}

	int VoxerStatistics::GetMeshed() const
	{
		return mMeshedChunks.load();
	}

	long VoxerStatistics::GetInitTime() const
	{
		return mInitTime.load();
	}

	long VoxerStatistics::GetMeshTime() const
	{
		return mMeshTime.load();
	}

	void VoxerStatistics::Log()
	{
		URHO3D_LOGDEBUGF(
			"Chunk Statistics:\n Initialized: {} Init Time: {} Average: {} \n Meshed: {} Mesh Time: {} Average: {}",
			GetInitialized(),
			GetInitTime(),
			GetInitTime() / GetInitialized(),
			GetMeshed(),
			GetMeshTime(),
			GetMeshTime() / GetMeshed());
	}
}