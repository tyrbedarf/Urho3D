#pragma once

#include "TaskSystem.h"

namespace Urho3D
{
	class Workerthread
	{
	protected:
		const int mYieldTime = 100;

	public:
		void operator()(TaskSystem* x);
	};
}