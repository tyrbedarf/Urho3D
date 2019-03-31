//
// Copyright (c) 2008-2019 the Urho3D project.
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
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Toolbox/Database/DatabaseConstants.h>
#include <Urho3D/UI/tbUI/tbUI.h>
#include <Urho3D/UI/tbUI/tbUIView.h>
#include <Urho3D/UI/tbUI/tbUIEvents.h>
#include <Urho3D/Toolbox/Database/DatatableViewWidget.h>

#include "HelloUrhoOrm.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloUrhoOrm)

void Player::RegisterObject(Context* context)
{
	context->RegisterFactory<Player>();

	// Only attributes can be serialized automatically.
	URHO3D_ACCESSOR_ATTRIBUTE(
		"PlayerId",
		GetPlayerId,
		SetPlayerId,
		int,
		-1,
		AM_FILE)
		// Add meta information to the accessor. In this case a primary key
		.SetMetadata(DatabaseConstants::META_PRIMARY_KEY, Variant(true))
		// Add information about how the widget is beeing configured.
		// This is only needed if the data is presented inside a database view
		// widget.
		.SetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH, Variant(50));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"PlayerName",
		GetPlayerName,
		SetPlayerName,
		String,
		"A Players Name",
		AM_FILE)
		// Add meta information to the accessor. In this case a not null constraint.
		.SetMetadata(DatabaseConstants::META_NOT_NULL, Variant(true))
		.SetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH, Variant(200));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"eMail",
		GetEMail,
		SetEMail,
		String,
		"An eMail adress",
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_NOT_NULL, Variant(true))
		// Add meta information to the accessor. In this case a unique constraint.
		.SetMetadata(DatabaseConstants::META_UNIQUE, Variant(true))
		.SetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH, Variant(200));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"Velocity",
		GetVelocity,
		SetVelocity,
		float,
		0.0f,
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH, Variant(80));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"Health",
		GetHealth,
		SetHealth,
		int,
		100,
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH, Variant(80));
}

HelloUrhoOrm::HelloUrhoOrm(Context* context) :
	Sample(context)
{
}

void HelloUrhoOrm::Start()
{
	// Execute base class startup
	Sample::Start();

	Player::RegisterObject(context_);

	CreateUI();

	SubscribeToEvents();

	// Set the mouse mode to use in the sample
	Sample::InitMouseMode(MM_FREE);

	// The database file.
	auto file = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Databases/orm_test.sqlite";

	// Create the actual database context.
	auto connectionString = "file:" + file + "";
	dbContext_ = SharedPtr<DatabaseContext>(new DatabaseContext(context_, connectionString));
	// Create a table for each type you want to store inside the database.
	// All types must inherit from serializable.
	dbContext_->AddTable(Player::GetTypeInfoStatic());

	if (!GetSubsystem<FileSystem>()->FileExists(file))
	{
		// Database does not exist, so create tables and add some samples
		dbContext_->CreateDatabase();

		auto object = SharedPtr<Player>(new Player(context_));
		object->SetPlayerName("John Doe");
		object->SetVelocity(1.5f);
		object->SetHealth(100);
		object->SetEMail("mail@example.de");

		dbContext_->Update(object);

		object = SharedPtr<Player>(new Player(context_));
		object->SetPlayerName("Timo Beil");
		object->SetVelocity(2.5f);
		object->SetHealth(86);
		object->SetEMail("mail2@example.de");

		dbContext_->Update(object);

		object = SharedPtr<Player>(new Player(context_));
		object->SetPlayerName("Anne Strasse");
		object->SetVelocity(1.1f);
		object->SetHealth(20);
		object->SetEMail("mail3@example.de");

		dbContext_->Update(object);
	}

	// Select a player object. The argument is the where part of an sql query.
	auto result = dbContext_->SelectQuery<Player>("SELECT * FROM 'Player'");
	auto item = result.At(0);
	URHO3D_LOGERROR("Player Name: " + item->GetPlayerName() + " Velocity: " + String(item->GetVelocity()));

	// Set the mouse mode to use in the sample
	Sample::InitMouseMode(MM_FREE);

	window_ = SharedPtr<DatatableViewWidget>(new DatatableViewWidget(context_, Player::GetTypeInfoStatic()));
	window_->SetData(result);
	window_->SetSize(200, 200);

	uiView_->AddChild(window_);
}

void HelloUrhoOrm::CreateUI()
{
	tbUI* ui = GetSubsystem<tbUI>();
	ui->Initialize("TB/resources/language/lng_en.tb.txt");
	ui->LoadDefaultPlayerSkin();

	uiView_ = new tbUIView(context_);
}

void HelloUrhoOrm::SubscribeToEvents()
{
	SubscribeToEvent(E_WIDGETEVENT, URHO3D_HANDLER(HelloUrhoOrm, HandleWidgetEvent));
}

void HelloUrhoOrm::HandleWidgetEvent(StringHash eventType, VariantMap& eventData)
{
}