#include "TaskSystem.h"

#include "Engine/Engine.h"
#include "Core/Context.h"
#include "WorkerThread.h"

namespace Urho3D
{
	TaskSystem::TaskSystem(Context* ctx) :
		Object(ctx)
	{
		mRunning = true;
	}

	Task* TaskSystem::GetNext()
	{
		Task* t = nullptr;
		int maxCount = mTaskCount.load();
		int count = 0;

		while (mTasks.try_dequeue(t) && (count < maxCount))
		{
			if (t->CanExecute())
			{
				mTaskCount--;
				return t;
			}

			mTasks.enqueue(t);
			count++;
		}

		return nullptr;
	}

	bool TaskSystem::IsRunning() const
	{
		return mRunning;
	}

	void TaskSystem::Start(int workerThreads)
	{
		mWorker.resize(workerThreads);
		for (int i = 0; i < workerThreads; i++)
		{
			mWorker[i] = new std::thread(Workerthread(), this);
		}
	}

	void TaskSystem::Shutdown()
	{
		while (mTaskCount.load() > 0)
		{
			std::this_thread::yield();
		}

		/// Join all running threads to make sure they finish
		/// And they finish clean.
		mRunning = false;
		for (int i = 0; i < mWorker.size(); i++)
		{
			mWorker[i]->join();
			delete mWorker[i];
		}
	}

	void TaskSystem::AddTask(
		std::function<void(void*)> func,
		void* userData,
		std::atomic<int>* counter,
		std::atomic<int>* dependencies)
	{
		Task* t = new Task(counter, dependencies);
		t->Function = func;
		t->Data = userData;

		mTaskCount++;
		mTasks.enqueue(t);
	}
}