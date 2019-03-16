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
#include <Urho3D/Toolbox/IO/JsonSerializer.h>

#include "HelloJsonSerialize.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloJsonSerialize)

void JsonTestPayLoad::RegisterObject(Context* context)
{
	context->RegisterFactory<JsonTestPayLoad>();

	URHO3D_ACCESSOR_ATTRIBUTE(
		"PlayerName",
		GetPlayerName,
		SetPlayerName,
		String,
		"A Players Name",
		AM_FILE);

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

	URHO3D_ACCESSOR_ATTRIBUTE(
		"IntData",
		GetIntData,
		SetIntData,
		VariantVector,
		VariantVector(),
		AM_FILE);

	URHO3D_ACCESSOR_ATTRIBUTE(
		"FloatData",
		GetFloatData,
		SetFloatData,
		VariantVector,
		VariantVector(),
		AM_FILE);

	URHO3D_ACCESSOR_ATTRIBUTE(
		"StringData",
		GetStringData,
		SetStringData,
		VariantVector,
		VariantVector(),
		AM_FILE);

	URHO3D_ACCESSOR_ATTRIBUTE(
		"IntVector",
		GetIntVector3,
		SetIntVector3,
		IntVector3,
		IntVector3(1, 1, 1),
		AM_FILE);
}

HelloJsonSerialize::HelloJsonSerialize(Context* context) :
	Sample(context)
{
}

void HelloJsonSerialize::Start()
{
	// Execute base class startup
	Sample::Start();

	JsonTestPayLoad::RegisterObject(context_);

	// Save a file
	auto file = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/testJsonSerializer.json";
	JsonSerializer serialize(context_);
	auto object = SharedPtr<JsonTestPayLoad>(new JsonTestPayLoad(context_));
	object->SetPlayerName("John Doe");
	object->SetVelocity(1.5f);
	object->SetHealth(100);
	for (int i = 0; i < 10; i++)
	{
		object->AddData(i);
		object->AddData((float) (i * 5.34566f));
		object->AddData(String(i * 5.34566f));
	}

	if (!serialize.Save(object, file, true))
	{
		URHO3D_LOGDEBUG("Could not serialize payload.");
		return;
	}
	else
	{
		URHO3D_LOGDEBUG("File written to " + file);
	}

	auto deserialize = serialize.Load<JsonTestPayLoad>(file);
	if (deserialize)
	{
		URHO3D_LOGDEBUG("Loaded item: " + deserialize->GetPlayerName());
	}
	else
	{
		URHO3D_LOGDEBUG("Error loading file.");
	}
}