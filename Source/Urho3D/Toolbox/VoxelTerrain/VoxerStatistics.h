#pragma once

#include <atomic>
#include "../../IO/Log.h"

namespace Urho3D
{
	class VoxerStatistics
	{
	private:
		std::atomic<int> mInitializedChunks;
		std::atomic<int> mMeshedChunks;
		std::atomic<long> mInitTime;
		std::atomic<long> mMeshTime;
		std::atomic<long> mSimplifyMeshTime;
		std::atomic<long> mEmptyChunksSkipped;
		std::atomic<long> mSolidChunksSkipped;

	public:
		VoxerStatistics();

		void AddInitialized();
		int GetInitialized() const;

		void AddMeshed();
		int GetMeshed() const;

		void AddInitTime(double time);
		long GetInitTime() const;

		void AddMeshTime(double time);
		long GetMeshTime() const;

		void AddSimplifyMeshTime(double time);
		long GetSimplifyMeshTime() const;

		long GetEmptyChunksSkipped() const;
		void AddEmptyChunksSkipped();

		long GetSolidChunksSkipped() const;
		void AddSolidChunksSkipped();

		void Log();

		String GetStats() const;
	};
}