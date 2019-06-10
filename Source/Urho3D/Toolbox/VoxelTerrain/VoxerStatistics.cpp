#include "VoxerStatistics.h"

namespace Urho3D
{
	VoxerStatistics::VoxerStatistics()
	{
		mInitializedChunks.store(0);
		mMeshedChunks.store(0);
		mInitTime.store(0);
		mMeshTime.store(0);
		mSimplifyMeshTime.store(0);
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
		mMeshTime.fetch_add((long)time);
	}

	void VoxerStatistics::AddSimplifyMeshTime(double time)
	{
		mSimplifyMeshTime.fetch_add((long) time);
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

	long VoxerStatistics::GetSimplifyMeshTime() const
	{
		return mSimplifyMeshTime.load();
	}

	void VoxerStatistics::Log()
	{
		URHO3D_LOGDEBUG(GetStats());
	}

	String VoxerStatistics::GetStats() const
	{
		String stats;
		stats.AppendWithFormat("Chunk Statistics:\n\tInitialized: %d Init Time: %d Average: %d \n\tMeshed: %d Mesh Time: %d Average: %d \n\tSimplify Time: %d Average: %d",
			GetInitialized(),
			GetInitTime(),
			GetInitTime() / GetInitialized(),
			GetMeshed(),
			GetMeshTime(),
			GetMeshTime() / GetMeshed(),
			GetSimplifyMeshTime(),
			GetSimplifyMeshTime() / GetMeshed());

		return stats;
	}
}