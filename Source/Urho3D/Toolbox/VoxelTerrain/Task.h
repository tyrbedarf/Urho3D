#pragma once

#include <atomic>

namespace Voxer
{
	struct Task
	{
	protected:
		/// Counter this task should update
		std::atomic<int>* mCounter;

		/// This should only start if the counter is zero.
		std::atomic<int>* mDependency;

		/// True, if the task has finished
		std::atomic<bool> mFinished;

		void NoOp(void*)
		{

		}

	public:
		/// Code to execute
		std::function<void(void*)> Function;
		void* Data;

		Task(std::atomic<int>* counter, std::atomic<int>* dependency)
		{
			mCounter = counter;
			if(mCounter != nullptr) mCounter++;

			mDependency = dependency;
			mFinished.store(false);
			Function = nullptr;
			Data = nullptr;
		}

		bool CanExecute()
		{
			return
				mDependency != nullptr &&
				mDependency->load() < 1;
		}

		void Execute()
		{
			Function(Data);
			if (mCounter != nullptr)
			{
				mCounter--;
			}

			mFinished.store(true);
		}

		/// Make sure the task has been finished.
		void Complete()
		{
			while (!mFinished.load())
			{
				std::this_thread::yield();
			}
		}
	};
}