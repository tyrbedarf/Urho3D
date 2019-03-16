//
// Copyright (c) 2008-2016 the Urho3D project.
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>

namespace Urho3D
{
	class tbUIWindow;
	class tbUIView;
}

class JsonTestPayLoad : public Serializable
{
	URHO3D_OBJECT(JsonTestPayLoad, Serializable);

private:
	String mPlayerName;
	float mVelocity;
	int mHealth;
	VariantVector mIntData;
	VariantVector mFloatData;
	VariantVector mStringData;

	IntVector3 mIntVector;

public:
	JsonTestPayLoad(Context* context) : Serializable(context)
	{

	}

	/// Register object factory.
	static void RegisterObject(Context* context);

	const String& GetPlayerName() const				{ return mPlayerName; }
	void SetPlayerName(const String& name)			{ mPlayerName = name; }

	const IntVector3& GetIntVector3() const			{ return mIntVector; }
	void SetIntVector3(const IntVector3& vector)	{ mIntVector = vector; }

	const VariantVector& GetIntData() const			{ return mIntData; }
	void SetIntData(const VariantVector& value)		{ mIntData = value; }

	const VariantVector& GetStringData() const		{ return mStringData; }
	void SetStringData(const VariantVector& value)	{ mStringData = value; }

	const VariantVector& GetFloatData() const		{ return mFloatData; }
	void SetFloatData(const VariantVector& value)	{ mFloatData = value; }

	void AddData(int d)		{ mIntData.Push(d); }
	void AddData(float d)	{ mFloatData.Push(d); }
	void AddData(String d)	{ mStringData.Push(d); }

	float GetVelocity() const		{ return mVelocity; }
	void SetVelocity(float value)	{ mVelocity = value; }

	int GetHealth() const		{ return mHealth; }
	void SetHealth(int value)	{ mHealth = value; }
};

class HelloJsonSerialize : public Sample
{
	URHO3D_OBJECT(HelloJsonSerialize, Sample)

public:
	/// Construct.
	HelloJsonSerialize(Context* context);

	/// Setup after engine initialization and before running the main loop.
	virtual void Start();

protected:

private:
};
