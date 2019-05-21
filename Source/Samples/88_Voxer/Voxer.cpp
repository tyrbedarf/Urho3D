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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/tbUI/tbUI.h>
#include <Urho3D/UI/tbUI/tbUIEvents.h>
#include <Urho3D/UI/tbUI/tbUIFontDescription.h>
#include <Urho3D/UI/tbUI/tbUIView.h>
#include <Urho3D/UI/tbUI/tbUILayout.h>
#include <Urho3D/UI/tbUI/tbUICheckBox.h>
#include <Urho3D/UI/tbUI/tbUITextField.h>
#include <Urho3D/UI/tbUI/tbUIButton.h>
#include <Urho3D/UI/tbUI/tbUIEditField.h>
#include <Urho3D/UI/tbUI/tbUIWindow.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Toolbox/Mesh/ProceduralMesh.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/UI/tbUI/tbUITextField.h>
#include <Urho3D/Core/WorkQueue.h>

#include "Voxer.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(VoxerSample)

VoxerSample::VoxerSample(Context* context) :
	Sample(context),
	batch_1(0),
	batch_2(0)
{
}

void VoxerSample::Start()
{
	Sample::Start();

	CreateUI();
	CreateScene();
	SetupViewport();

	SubscribeToEvents();

	auto workqueue = GetSubsystem<WorkQueue>();
	int task = 0;
	for (int i = 0; i < 100; i++)
	{
		task++;
		workqueue->AddTask(
			[](void* data)
			{
				for (int i = 0; i < 100000; i++)
				{
					Time::Sleep(0);
				}
			},
			&i,
			&batch_1,
			nullptr);
	}

	for (int i = 0; i < 50; i++)
	{
		workqueue->AddTask(
			[](void* data)
			{
				for (int i = 0; i < 100000; i++)
				{
					Time::Sleep(0);
				}
			},
			&i,
			&batch_2,
			&batch_1);
	}

	Sample::InitMouseMode(MM_FREE);
}

void VoxerSample::CreateScene()
{
	auto* cache = GetSubsystem<ResourceCache>();
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();

	// Create a Zone for ambient light & fog control
	Node* zoneNode = scene_->CreateChild("Zone");
	auto* zone = zoneNode->CreateComponent<Zone>();
	zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
	zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
	zone->SetFogStart(200.0f);
	zone->SetFogEnd(300.0f);

	// Create a scene node for the camera, which we will move around
	// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
	cameraNode_ = new Node(context_);
	cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	auto* camera = cameraNode_->CreateComponent<Camera>();
	camera->SetFarClip(300.0f);

	// Create a directional light
	Node* lightNode = scene_->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
	auto* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.4f, 1.0f, 0.4f));
	light->SetSpecularIntensity(2.5f);

	material = cache->GetResource<Material>("Materials/StoneTiled.xml");
}

void VoxerSample::SetupViewport()
{
	auto* renderer = GetSubsystem<Renderer>();
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}

void VoxerSample::CreateUI()
{
	tbUI* ui = GetSubsystem<tbUI>();
	ui->Initialize("TB/resources/language/lng_en.tb.txt");
	ui->LoadDefaultPlayerSkin();

	uiView_ = new tbUIView(context_);

	tbUILayout* layout = new tbUILayout(context_);
	layout->SetAxis(UI_AXIS_Y);

	window_ = new tbUIWindow(context_);
	window_->SetSettings((UI_WINDOW_SETTINGS)(
		UI_WINDOW_SETTINGS_TITLEBAR |
		UI_WINDOW_SETTINGS_CLOSE_BUTTON));

	window_->SetText("Hello Custom Mesh!");
	window_->AddChild(layout);
	window_->ResizeToFitContent();
	window_->SetPosition(20, 20);
	window_->SetSize(450, 100);

	text_ = new tbUITextField(context_);
	text_->SetText("Press F8 to toggle Wireframe mode.");
	layout->AddChild(text_);

	uiView_->AddChild(window_);
}

void VoxerSample::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(VoxerSample, HandleUpdate));
	SubscribeToEvent(E_WIDGETDELETED, URHO3D_HANDLER(VoxerSample, HandleWidgetDeleted));
}

void VoxerSample::ToggleWireFrame()
{
	FillMode mode = material->GetFillMode() == FillMode::FILL_WIREFRAME ? FillMode::FILL_SOLID : FillMode::FILL_WIREFRAME;
	material->SetFillMode(mode);
}

void VoxerSample::HandleWidgetDeleted(StringHash eventType, VariantMap& eventData)
{
	GetSubsystem<Engine>()->Exit();
}

void VoxerSample::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
	float timeStep = eventData[P_TIMESTEP].GetFloat();
	MoveCamera(timeStep);
}

void VoxerSample::MoveCamera(float timeStep)
{
	if (GetSubsystem<UI>()->GetFocusElement())
		return;

	auto* input = GetSubsystem<Input>();

	const float MOVE_SPEED = 20.0f;
	const float MOUSE_SENSITIVITY = 0.1f;

	IntVector2 mouseMove = input->GetMouseMove();
	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

	if (input->GetKeyDown(KEY_W))
		cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);

	if (input->GetKeyDown(KEY_S))
		cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);

	if (input->GetKeyDown(KEY_A))
		cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);

	if (input->GetKeyDown(KEY_D))
		cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

	if (input->GetKeyPress(KEY_F8))
	{
		ToggleWireFrame();
	}

	if (input->GetKeyPress(KEY_F9))
	{
		URHO3D_LOGDEBUG(
			"Position: " + String(cameraNode_->GetPosition()) +
			" Rotation: " + String(cameraNode_->GetRotation()));
	}
}
