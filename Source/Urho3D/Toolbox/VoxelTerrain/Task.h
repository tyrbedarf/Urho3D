#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace Urho3D
{
	struct Task
	{
	protected:
		/// The batch of work this task belongs to.
		/// Task depending on this task will start after
		/// all tasks of this batch have been execute and are
		/// finished.
		std::atomic<int>* mBatch;

		/// This should only start if the counter is zero.
		std::atomic<int>* mDependency;

		/// True, if the task has finished
		std::atomic<bool> mFinished;

		/// Default function assign on creating a new task.
		void NoOp(void*)
		{

		}

	public:
		/// Code to execute
		std::function<void(void*)> Function;
		void* Data;

		Task(std::atomic<int>* counter, std::atomic<int>* dependency)
		{
			mBatch = counter;
			if(mBatch != nullptr) mBatch++;

			mDependency = dependency;
			mFinished.store(false);

			Function = nullptr;

			Data = nullptr;
		}

		/// True, if the task can be executed. In other words:
		/// true if all dependencies finished.
		bool CanExecute()
		{
			return
				mDependency == nullptr ||
				mDependency->load() < 1;
		}

		/// Execute this task.
		void Execute()
		{
			Function(Data);
			if (mBatch != nullptr)
			{
				mBatch--;
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