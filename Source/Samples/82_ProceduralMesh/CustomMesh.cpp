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

#include "CustomMesh.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloCustomMesh)

HelloCustomMesh::HelloCustomMesh(Context* context) :
	Sample(context)
{
}

void HelloCustomMesh::Start()
{
	Sample::Start();

	CreateUI();
	CreateScene();
	SetupViewport();

	SubscribeToEvents();

	Sample::InitMouseMode(MM_FREE);
}

void HelloCustomMesh::CreateScene()
{
	auto* cache = GetSubsystem<ResourceCache>();
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();

	Node* planeNode = scene_->CreateChild("Plane");
	planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
	auto* planeObject = planeNode->CreateComponent<StaticModel>();

	material_ = cache->GetResource<Material>("Materials/StoneTiled.xml");
	planeObject->SetModel(cache->GetResource<Model>("Models/SubdividedPlane/Plane.mdl"));
	planeObject->SetMaterial(material_);

	Node* lightNode = scene_->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
	auto* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);

	// Create a scene node for the camera, which we will move around
	// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
	cameraNode_ = scene_->CreateChild("Camera");
	cameraNode_->CreateComponent<Camera>();

	// Set an initial position for the camera scene node above the plane
	cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
}

void HelloCustomMesh::SetupViewport()
{
	auto* renderer = GetSubsystem<Renderer>();
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	renderer->SetViewport(0, viewport);
}

void HelloCustomMesh::CreateUI()
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
	window_->SetSize(250, 100);

	button_ = new tbUIButton(context_);
	button_->SetText("Toogle Wireframe");
	button_->SetId("Solid");

	layout->AddChild(button_);

	uiView_->AddChild(window_);
}

void HelloCustomMesh::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloCustomMesh, HandleUpdate));
	SubscribeToEvent(E_WIDGETEVENT, URHO3D_HANDLER(HelloCustomMesh, HandleWidgetEvent));
	SubscribeToEvent(E_WIDGETDELETED, URHO3D_HANDLER(HelloCustomMesh, HandleWidgetDeleted));
}

void HelloCustomMesh::HandleWidgetEvent(StringHash eventType, VariantMap& eventData)
{
	using namespace WidgetEvent;

	if (eventData[P_TYPE] == UI_EVENT_TYPE_CLICK)
	{
		tbUIWidget* widget = static_cast<tbUIWidget*>(eventData[P_TARGET].GetPtr());
		if (!widget)
		{
			return;
		}

		URHO3D_LOGDEBUG("Widget: " + widget->GetId());
		FillMode mode = material_->GetFillMode() == FillMode::FILL_WIREFRAME ? FillMode::FILL_SOLID : FillMode::FILL_WIREFRAME;
		material_->SetFillMode(mode);
	}
}

void HelloCustomMesh::HandleWidgetDeleted(StringHash eventType, VariantMap& eventData)
{

}

void HelloCustomMesh::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;
	float timeStep = eventData[P_TIMESTEP].GetFloat();
	MoveCamera(timeStep);
}

void HelloCustomMesh::MoveCamera(float timeStep)
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
}
