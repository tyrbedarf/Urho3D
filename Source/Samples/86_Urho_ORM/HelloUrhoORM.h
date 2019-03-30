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

#pragma once

#include "Sample.h"
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/ThirdParty/TurboBadger/tb_widgets.h>

#include <Urho3D/Toolbox/Database/DatabaseContext.h>
#include <Urho3D/Toolbox/Database/DatatableViewWidget.h>

namespace Urho3D
{
	class tbUIWindow;
	class tbUIView;
}

class Player : public Serializable
{
	URHO3D_OBJECT(Player, Serializable);

private:
	int mPlayerId;
	String mPlayerName;
	String mEMail;
	float mVelocity;
	int mHealth;

public:
	Player(Context* context) : Serializable(context)
	{

	}

	/// Register object factory.
	static void RegisterObject(Context* context);

	const int GetPlayerId() const			{ return mPlayerId; }
	void SetPlayerId(const int value)		{ mPlayerId = value; }

	const String& GetPlayerName() const		{ return mPlayerName; }
	void SetPlayerName(const String& name)	{ mPlayerName = name; }

	const String& GetEMail() const			{ return mEMail; }
	void SetEMail(const String& value)		{ mEMail = value; }

	float GetVelocity() const				{ return mVelocity; }
	void SetVelocity(float value)			{ mVelocity = value; }

	int GetHealth() const					{ return mHealth; }
	void SetHealth(int value)				{ mHealth = value; }
};

namespace Urho3D
{
	class tbUIWidget;
	class tbUIView;
}


class HelloUrhoOrm : public Sample
{
	URHO3D_OBJECT(HelloUrhoOrm, Sample)

public:
	/// Construct.
	HelloUrhoOrm(Context* context);

	/// Setup after engine initialization and before running the main loop.
	virtual void Start();

protected:
	SharedPtr<DatabaseContext> dbContext_;

	WeakPtr<DatatableViewWidget> window_;
	WeakPtr<tbUIView> uiView_;

	void CreateUI();

	/// Subscribe to application-wide logic update events.
	void SubscribeToEvents();

	/// Handle the logic update event.
	void HandleWidgetEvent(StringHash eventType, VariantMap& eventData);


private:
};
