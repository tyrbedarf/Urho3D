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

#include "HelloUrhoOrm.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloUrhoOrm)

void Player::RegisterObject(Context* context)
{
	context->RegisterFactory<Player>();

	URHO3D_ACCESSOR_ATTRIBUTE(
		"PlayerId",
		GetPlayerId,
		SetPlayerId,
		int,
		-1,
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_PRIMARY_KEY, Variant(true));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"PlayerName",
		GetPlayerName,
		SetPlayerName,
		String,
		"A Players Name",
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_NOT_NULL, Variant(true));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"eMail",
		GetEMail,
		SetEMail,
		String,
		"An eMail adress",
		AM_FILE)
		.SetMetadata(DatabaseConstants::META_NOT_NULL, Variant(true))
		.SetMetadata(DatabaseConstants::META_UNIQUE, Variant(true));

	URHO3D_ACCESSOR_ATTRIBUTE(
		"Velocity",
		GetVelocity,
		SetVelocity,
		float,
		0.0f,
		AM_FILE);

	URHO3D_ACCESSOR_ATTRIBUTE(
		"Health",
		GetHealth,
		SetHealth,
		int,
		100,
		AM_FILE);
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

	// Save a file
	auto file = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Databases/orm_test.sqlite";
	auto connectionString = "file:" + file + "";
	dbContext_ = SharedPtr<DatabaseContext>(new DatabaseContext(context_, connectionString));
	dbContext_->AddTable(Player::GetTypeInfoStatic());
	dbContext_->CreateDatabase();

	auto object = SharedPtr<Player>(new Player(context_));
	object->SetPlayerName("John Doe");
	object->SetVelocity(1.5f);
	object->SetHealth(100);
	object->SetEMail("mail@example.de");

	dbContext_->Update(object);

	// Set the mouse mode to use in the sample
	Sample::InitMouseMode(MM_FREE);

}