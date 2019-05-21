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

	public:
		VoxerStatistics();

		void AddInitialized();

		void AddMeshed();

		void AddInitTime(double time);

		void AddMeshTime(double time);

		int GetInitialized() const;

		int GetMeshed() const;

		long GetInitTime() const;

		long GetMeshTime() const;

		void Log();
	};
}