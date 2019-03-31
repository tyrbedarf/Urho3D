//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/UI/tbUI/tbUI.h>
#include <Urho3D/UI/tbUI/tbUIEvents.h>
#include <Urho3D/UI/tbUI/tbUIFontDescription.h>
#include <Urho3D/UI/tbUI/tbUIView.h>
#include <Urho3D/UI/tbUI/tbUIComponent.h>
#include <Urho3D/UI/tbUI/tbUILayout.h>
#include <Urho3D/UI/tbUI/tbUICheckBox.h>
#include <Urho3D/UI/tbUI/tbUIImageWidget.h>
#include <Urho3D/UI/tbUI/tbUITextField.h>
#include <Urho3D/UI/tbUI/tbUIButton.h>
#include <Urho3D/UI/tbUI/tbUIEditField.h>
#include <Urho3D/UI/tbUI/tbUISeparator.h>
#include <Urho3D/UI/tbUI/tbUIWindow.h>
#include <Urho3D/UI/tbUI/tbUIFinderWindow.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include "87_HelloFinderWindow.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloFinderWindow)

HelloFinderWindow::HelloFinderWindow(Context* context) :
	Sample(context)
{
}

void HelloFinderWindow::Start()
{
	// Execute base class startup
	Sample::Start();

	// Create 2D "Hello GUI"
	tbUI* ui = GetSubsystem<tbUI>();
	ui->Initialize("TB/resources/language/lng_en.tb.txt");
	ui->LoadDefaultPlayerSkin();

	uiView_ = new tbUIView(context_);
	CreateUI();

	// Create the scene content
	CreateScene();

	SetupViewport();

	// Finally subscribe to the update event. Note that by subscribing events at this point we have already missed some events
	// like the ScreenMode event sent by the Graphics subsystem when opening the application window. To catch those as well we
	// could subscribe in the constructor instead.
	SubscribeToEvents();

	// Set the mouse mode to use in the sample
	Sample::InitMouseMode(MM_FREE);
}

void HelloFinderWindow::Cleanup()
{

}

void HelloFinderWindow::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
	// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
	// use, but now we just use full screen and default render path configured in the engine command line options
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}


void HelloFinderWindow::CreateScene()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_);

	// Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
	// show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
	// is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
	// optimizing manner
	scene_->CreateComponent<Octree>();

	// Create a child scene node (at world origin) and a StaticModel component into it.
	Node* planeNode = scene_->CreateChild("Box");
	planeNode->SetScale(Vector3(5.0f, 5.0f, 5.0f));
	planeNode->SetRotation(Quaternion(90, Vector3::LEFT));

	// Create a directional light to the world so that we can see something. The light scene node's orientation controls the
	// light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
	// The light will use default settings (white light, no shadows)
	Node* lightNode = scene_->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3::FORWARD); // The direction vector does not need to be normalized
	Light* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);

	// Create a scene node for the camera, which we will move around
	// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
	cameraNode_ = scene_->CreateChild("Camera");
	cameraNode_->CreateComponent<Camera>();

	// Set an initial position for the camera scene node above the plane
	cameraNode_->SetPosition(Vector3(-3.0f, 0.0f, -30.0f));
}


tbUIView* HelloFinderWindow::CreateUI(bool renderToTexture)
{
	int size = 220;

	tbUIView* view = renderToTexture ? new tbUIView(context_) : uiView_;

	window_ = SharedPtr<tbUIFinderWindow>(new tbUIFinderWindow(context_, view, "finder_window"));
	window_->FindFile("Choose a file", "c:/", 1, 400, 400);

	return view;
}


void HelloFinderWindow::SubscribeToEvents()
{
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloFinderWindow, HandleUpdate));
}

void HelloFinderWindow::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
}
