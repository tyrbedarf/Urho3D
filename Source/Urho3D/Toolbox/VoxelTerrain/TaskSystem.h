#pragma once

#include <thread>
#include "../Container/ConcurentQueue.h"
#include <vector>

#include "../Core/Object.h"
#include "Task.h"

namespace Urho3D
{
	class TaskSystem : public Object
	{
		URHO3D_OBJECT(TaskSystem, Object)

	protected:
		moodycamel::ConcurrentQueue<Task*> mTasks;
		std::vector<std::thread*> mWorker;
		bool mRunning;
		std::atomic<int> mTaskCount;

	public:
		TaskSystem(Context* ctx);

		virtual Task* GetNext();
		virtual bool IsRunning() const;
		virtual void Start(int workerThreads);
		virtual void Shutdown();

		///
		virtual void AddTask(
			std::function<void(void*)> func,
			void* userData,
			std::atomic<int>* counter,
			std::atomic<int>* dependencies);
	};
}