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

	// CreateProceduralMesh();

	SubscribeToEvents();

	Sample::InitMouseMode(MM_FREE);
}

void HelloCustomMesh::CreateScene()
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

	// Create a directional light
	Node* lightNode = scene_->CreateChild("DirectionalLight");
	lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
	auto* light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetColor(Color(0.4f, 1.0f, 0.4f));
	light->SetSpecularIntensity(2.5f);

	//Vector3 vertices[] =
	//{
	//	Vector3(0.0f, 0.0f, 0.0),
	//	Vector3(1.0f, 0.0f, 0.0),
	//	Vector3(0.0f, 0.0f, 1.0),
	//	Vector3(1.0f, 0.0f, 1.0),

	//	Vector3(2.0f, 0.0f, 0.0), // 4
	//	Vector3(0.0f, 0.0f, 2.0),
	//	Vector3(2.0f, 0.0f, 2.0)
	//};

	//const size_t numOfTriangles = 4;
	//unsigned short triangles[]
	//{
	//	0, 1, 2,
	//	1, 3, 2,
	//	1, 4, 3,
	//	4, 6, 3
	//};

	ProceduralMesh mesh(context_);
	mesh.FromModel(cache->GetResource<Model>("Models/SubdividedPlane/Plane.mdl"), 0, 0);
	/*for (int i = 0; i < numOfTriangles; i++)
	{
		mesh.AddTriangle(
			vertices[triangles[(i * 3) + 0]],
			vertices[triangles[(i * 3) + 1]],
			vertices[triangles[(i * 3) + 2]]);
	}*/


	Node* node = scene_->CreateChild("Plane");
	node->SetScale(Vector3(10.0f, 1.0f, 10.0f));
	node->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	material = cache->GetResource<Material>("Materials/StoneTiled.xml");
	auto* object = node->CreateComponent<StaticModel>();

	auto model = mesh.GetModel();
	if (!model)
	{
		URHO3D_LOGDEBUG("Model has not been created.");
		return;
	}

	auto geom = model->GetGeometry(0, 0);
	if (!geom)
	{
		URHO3D_LOGDEBUG("Geometry has not been created.");
		return;
	}

	/*auto indexBuffer = geom->GetIndexBuffer();
	auto* indices = (const unsigned char*) indexBuffer->Lock(0, indexBuffer->GetIndexCount());
	auto indexSize = indexBuffer->GetIndexSize();
	auto indexCount = indexBuffer->GetIndexCount();
	for (int i = 0; i < indexCount; i++)
	{
		unsigned short src = *reinterpret_cast<const unsigned short*>(indices + i * indexSize);
		URHO3D_LOGDEBUG(String(i + 1) + ". Index: " + String(src));
	}*/

	auto vertexBuffer = geom->GetVertexBuffer(0);
	auto* verts = (const unsigned char*)vertexBuffer->Lock(0, vertexBuffer->GetVertexCount());
	auto vertexSize = vertexBuffer->GetVertexSize();
	auto vertexCount = vertexBuffer->GetVertexCount();
	URHO3D_LOGDEBUG("Size: " + String(vertexSize) + " Count: " + String(vertexCount));
	for (int i = 0; i < vertexCount; i++)
	{
		const Vector3& src = *reinterpret_cast<const Vector3*>(verts + i * vertexSize);
		URHO3D_LOGDEBUG(String(i + 1) + ". Vertex: " + String(src));

		const Vector3& src2 = *reinterpret_cast<const Vector3*>(verts + (i * vertexSize) + (3 * sizeof(float)));
		URHO3D_LOGDEBUG(String(i + 1) + ". Normal: " + String(src2));

		const Vector2& src3 = *reinterpret_cast<const Vector2*>(verts + (i * vertexSize) + (6 * sizeof(float)));
		URHO3D_LOGDEBUG(String(i + 1) + ". UV: " + String(src3));
	}

	object->SetModel(model);
	object->SetMaterial(material);

	// Create a scene node for the camera, which we will move around
	// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
	cameraNode_ = new Node(context_);
	cameraNode_->SetPosition(Vector3(0.0f, 10.0f, 0.0f));
	cameraNode_->LookAt(node->GetPosition());
	auto* camera = cameraNode_->CreateComponent<Camera>();
	camera->SetFarClip(300.0f);
}

void HelloCustomMesh::CreateProceduralMesh()
{
	const unsigned numVertices = 18;

	float vertexData[] = {
		// Position             Normal
		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,

		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,

		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f
	};

	const unsigned short indexData[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17
	};

	// Calculate face normals now
	for (unsigned i = 0; i < numVertices; i += 3)
	{
		Vector3& v1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i]));
		Vector3& v2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1)]));
		Vector3& v3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2)]));
		Vector3& n1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i + 3]));
		Vector3& n2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1) + 3]));
		Vector3& n3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2) + 3]));

		Vector3 edge1 = v1 - v2;
		Vector3 edge2 = v1 - v3;
		n1 = n2 = n3 = edge1.CrossProduct(edge2).Normalized();
	}

	SharedPtr<Model> fromScratchModel(new Model(context_));
	SharedPtr<VertexBuffer> vb(new VertexBuffer(context_));
	SharedPtr<IndexBuffer> ib(new IndexBuffer(context_));
	SharedPtr<Geometry> geom(new Geometry(context_));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vb->SetShadowed(true);
	// We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
	// defining the vertex elements explicitly to allow any element types and order
	PODVector<VertexElement> elements;
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
	vb->SetSize(numVertices, elements);
	vb->SetData(vertexData);

	ib->SetShadowed(true);
	ib->SetSize(numVertices, false);
	ib->SetData(indexData);

	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);

	fromScratchModel->SetNumGeometries(1);
	fromScratchModel->SetGeometry(0, 0, geom);
	fromScratchModel->SetBoundingBox(BoundingBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)));

	// Though not necessary to render, the vertex & index buffers must be listed in the model so that it can be saved properly
	Vector<SharedPtr<VertexBuffer> > vertexBuffers;
	Vector<SharedPtr<IndexBuffer> > indexBuffers;
	vertexBuffers.Push(vb);
	indexBuffers.Push(ib);

	// Morph ranges could also be not defined. Here we simply define a zero range (no morphing) for the vertex buffer
	PODVector<unsigned> morphRangeStarts;
	PODVector<unsigned> morphRangeCounts;
	morphRangeStarts.Push(0);
	morphRangeCounts.Push(0);
	fromScratchModel->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
	fromScratchModel->SetIndexBuffers(indexBuffers);

	Node* node = scene_->CreateChild("FromScratchObject");
	node->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
	auto* object = node->CreateComponent<StaticModel>();
	object->SetModel(fromScratchModel);
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
		FillMode mode = material->GetFillMode() == FillMode::FILL_WIREFRAME ? FillMode::FILL_SOLID : FillMode::FILL_WIREFRAME;
		material->SetFillMode(mode);
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
