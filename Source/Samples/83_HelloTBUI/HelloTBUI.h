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
#include <Urho3D/ThirdParty/TurboBadger/tb_widgets.h>
#include <Urho3D/Scene/Serializable.h>

namespace Urho3D
{
	class tbUIWindow;
	class tbUIView;
}

#define GET_SET(name, type)\
	private:\
		type m##name;\
	public:\
		void Set##name(type value) { m##name = value; } \
		type Get##name() const { return m##name; }

#define GET_SET_STRING(name)\
	private:\
		String m##name;\
	public:\
		void Set##name(const String& value) { m##name = value; } \
		const String& Get##name() const { return m##name; }

#define QUICK_ACCESOR_STRING(name)\
	URHO3D_ACCESSOR_ATTRIBUTE(\
	#name,\
	Get##name,\
	Set##name,\
	String,\
	String::EMPTY,\
	AM_FILE)

#define QUICK_ACCESOR(name, type, default_value)\
	URHO3D_ACCESSOR_ATTRIBUTE(\
	#name,\
	Get##name,\
	Set##name,\
	type,\
	default_value,\
	AM_FILE)

class ExampleSerializeable : public Serializable
{
	URHO3D_OBJECT(ExampleSerializeable, Serializable)

		GET_SET_STRING(PlayerName);
	GET_SET(PlayerHealth, int);

public:
	ExampleSerializeable(Context* context) : Serializable(context)
	{

	}
	/// Register object factory.
	static void RegisterObject(Context* context)
	{
		/*context->RegisterFactory<ExampleSerializeable>();*/
		QUICK_ACCESOR_STRING(PlayerName);
		QUICK_ACCESOR(PlayerHealth, int, 100);
	}
};

class HelloGui : public Sample
{
	URHO3D_OBJECT(HelloGui, Sample)

public:
	/// Construct.
	HelloGui(Context* context);

	/// Setup after engine initialization and before running the main loop.
	virtual void Start();

protected:

private:

	void CreateUI();
	/// Subscribe to application-wide logic update events.
	void SubscribeToEvents();
	/// Handle the logic update event.
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

	void HandleWidgetEvent(StringHash eventType, VariantMap& eventData);

	void HandleWidgetDeleted(StringHash eventType, VariantMap& eventData);

	WeakPtr<tbUIWindow> window_;
	WeakPtr<tbUIWindow> window2_;
	WeakPtr<tbUIView> uiView_;
	SharedPtr<ExampleSerializeable> example_;
};
