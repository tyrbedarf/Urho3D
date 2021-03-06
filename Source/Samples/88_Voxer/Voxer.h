//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Sample.h"

namespace Urho3D
{
	class tbUIWindow;
	class tbUIView;
	class tbUITextView;
	class VoxerSystem;
}

class VoxerSample : public Sample
{
	URHO3D_OBJECT(VoxerSample, Sample)

public:
	/// Construct.
	VoxerSample(Context* context);

	/// Setup after engine initialization and before running the main loop.
	virtual void Start();

protected:

private:
	void CreateUI();
	void CreateScene();

	/// Subscribe to application-wide logic update events.
	void SubscribeToEvents();

	/// Handle the logic update event.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleWidgetDeleted(StringHash eventType, VariantMap& eventData);

	void SetupViewport();
	void MoveCamera(float timeStep);
	void UpdateVoxerSystem(float timeStep);

	void ToggleWireFrame();

	WeakPtr<tbUIWindow> window_;
	WeakPtr<tbUIView> uiView_;

	SharedPtr<Material> material;
	SharedPtr<tbUITextField> text_;
	SharedPtr<tbUITextField> direction_;

	SharedPtr<VoxerSystem> voxer_;

	std::atomic<int> batch_1;
	std::atomic<int> batch_2;
};
