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
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include "HelloGUI3D.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloGui3D)

HelloGui3D::HelloGui3D(Context* context) :
	Sample(context)
{
}

void HelloGui3D::Start()
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

void HelloGui3D::Cleanup()
{
	if (view3D_)
	{
		view3D_->Remove();
	}
}

void HelloGui3D::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
	// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
	// use, but now we just use full screen and default render path configured in the engine command line options
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}


void HelloGui3D::CreateScene()
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

	StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
	planeObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

	// Create a UIComponent and set it to render on the box model
	uiComponent_ = planeNode->CreateComponent<tbUIComponent>();
	uiComponent_->SetStaticModel(planeObject);
	// Create the same UIView as 2D, though for 3D, and set it as component's view
	uiComponent_->SetUIView(CreateUI(true));

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


tbUIView* HelloGui3D::CreateUI(bool renderToTexture)
{
	int size = 220;

	tbUIView* view = renderToTexture ? new tbUIView(context_) : uiView_;

	if (renderToTexture)
	{
		view3D_ = view;
		view->SetRenderToTexture(true, size, size);
	}

	tbUILayout* mainLayout = new tbUILayout(context_);
	mainLayout->SetAxis(UI_AXIS_Y);
	mainLayout->SetSpacing(16);

	tbUILayout* topLayout = new tbUILayout(context_);
	topLayout->SetAxis(UI_AXIS_X);
	topLayout->SetSpacing(8);

	tbUIImageWidget* imageWidget = new tbUIImageWidget(context_);
	imageWidget->SetImage("Textures/atomic_logo.png");
	topLayout->AddChild(imageWidget);

	tbUILayout* sideLayout = new tbUILayout(context_);
	sideLayout->SetAxis(UI_AXIS_Y);

	tbUICheckBox* checkBox = new tbUICheckBox(context_);
	checkBox->SetId("Checkbox");
	sideLayout->AddChild(checkBox);

	tbUIButton* button = new tbUIButton(context_);
	button->SetText("Button");
	button->SetId("Button");
	sideLayout->AddChild(button);

	topLayout->AddChild(sideLayout);
	mainLayout->AddChild(topLayout);

	tbUISeparator* sep = new tbUISeparator(context_);
	mainLayout->AddChild(sep);

	tbUIEditField* edit = new tbUIEditField(context_);
	edit->SetLayoutMinWidth(220);
	edit->SetId("EditField");
	edit->SetTextAlign(UI_TEXT_ALIGN_CENTER);
	edit->SetText(renderToTexture ? "I'm a 3D UI" : "I'm a 2D UI");
	mainLayout->AddChild(edit);

	SharedPtr<tbUIWindow> window(new tbUIWindow(context_));

	UI_WINDOW_SETTINGS settings = (UI_WINDOW_SETTINGS)(renderToTexture ? (UI_WINDOW_SETTINGS_NONE) :
		(UI_WINDOW_SETTINGS_TITLEBAR | UI_WINDOW_SETTINGS_CLOSE_BUTTON));

	window->SetSettings(settings);
	window->SetText(renderToTexture ? "GUI 3D" : "GUI 2D");
	window->AddChild(mainLayout);
	window->SetSize(renderToTexture ? view->GetWidth() : size, renderToTexture ? view->GetHeight() : size);

	view->AddChild(window);

	if (!renderToTexture)
	{
		window->SetPosition(view->GetWidth() / 4, view->GetHeight() / 2 - (size / 2));
	}
	else
	{
		window->Center();
	}

	return view;

}

bool HelloGui3D::Raycast(float maxDistance, Vector3& hitPos, Vector3& hitNormal, Drawable*& hitDrawable)
{
	hitDrawable = 0;

	Input* input = GetSubsystem<Input>();

	IntVector2 pos = input->GetMousePosition();
	// Check the cursor is visible and there is no UI element in front of the cursor
	if (!input->IsMouseVisible())
		return false;

	Graphics* graphics = GetSubsystem<Graphics>();
	Camera* camera = cameraNode_->GetComponent<Camera>();
	Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());
	// Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	scene_->GetComponent<Octree>()->Raycast(query);

	for (unsigned i = 0; i < results.Size(); i++)
	{
		RayQueryResult& result = results[i];

		if (uiComponent_->GetStaticModel() != result.drawable_)
			continue;

		hitPos = result.position_;
		hitNormal = result.normal_;
		hitDrawable = result.drawable_;

		return true;
	}

	return false;
}


void HelloGui3D::SubscribeToEvents()
{
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloGui3D, HandleUpdate));
}

void HelloGui3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	if (!view3D_)
		return;

	Vector3 hitPos;
	Vector3 hitNormal;
	Drawable* hitDrawable;
	bool result = Raycast(250.0f, hitPos, hitNormal, hitDrawable);

	Input* input = GetSubsystem<Input>();
	if (!input->GetMouseButtonDown(MOUSEB_LEFT))
	{
		if (!result)
		{
			view3D_->ResignFocus();
		}
		else
		{
			view3D_->SetFocus();
		}
	}

	// Spawn fire on shift click
	if (result && input->GetMouseButtonPress(MOUSEB_LEFT) && input->GetQualifierDown(QUAL_SHIFT))
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Node* node = uiComponent_->GetNode()->CreateChild("GreatBallOfFire");
		node->SetScale(0.3f);
		node->SetWorldPosition(hitPos + (hitNormal * .15f));
		node->SetWorldDirection(hitNormal);
		node->Pitch(90);

		ParticleEmitter* emitter = node->CreateComponent<ParticleEmitter>();
		emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/UIFire.xml"));
	}

	// Take the frame time step, which is stored as a float
	float timeStep = eventData[P_TIMESTEP].GetFloat();

	Node* node = uiComponent_->GetStaticModel()->GetNode();

	node->Yaw(6.0f * timeStep * 1.5f);
	node->Roll(-6.0f * timeStep * 1.5f);
	node->Pitch(-6.0f * timeStep * 1.5f);

	Time* time = GetSubsystem<Time>();

	// enable for some extra movement
	const bool travelOnZ = false;

	if (travelOnZ)
	{
		Vector3 pos = node->GetPosition();
		pos.z_ = 3.0f * Sin<float>(time->GetElapsedTime() * 100.0f * 1.5f);
		node->SetPosition(pos);
	}

}
