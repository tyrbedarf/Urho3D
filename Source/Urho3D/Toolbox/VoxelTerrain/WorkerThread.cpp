#include "WorkerThread.h"

namespace Urho3D
{
	void Workerthread::operator()(TaskSystem* x)
	{
		while (x->IsRunning())
		{
			auto next = x->GetNext();
			if (next == nullptr)
			{
				std::this_thread::yield();
				continue;
			}

			next->Execute();
			delete next;
		}
	}
}