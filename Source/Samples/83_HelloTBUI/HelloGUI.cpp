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

#include "HelloGui.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloGui)

HelloGui::HelloGui(Context* context) :
	Sample(context)
{
}

void HelloGui::Start()
{
	// Execute base class startup
	Sample::Start();

	// Create "Hello GUI"
	CreateUI();

	// Finally subscribe to the update event. Note that by subscribing events at this point we have already missed some events
	// like the ScreenMode event sent by the Graphics subsystem when opening the application window. To catch those as well we
	// could subscribe in the constructor instead.
	SubscribeToEvents();

	// Set the mouse mode to use in the sample
	Sample::InitMouseMode(MM_FREE);
}

void HelloGui::CreateUI()
{
	tbUI* ui = GetSubsystem<tbUI>();
	ui->Initialize("TB/resources/language/lng_en.tb.txt");
	ui->LoadDefaultPlayerSkin();

	uiView_ = new tbUIView(context_);

	tbUILayout* layout = new tbUILayout(context_);
	layout->SetAxis(UI_AXIS_Y);

	tbUICheckBox* checkBox = new tbUICheckBox(context_);
	checkBox->SetId("Checkbox");

	layout->AddChild(checkBox);

	tbUIButton* button = new tbUIButton(context_);
	button->SetText("Button");
	button->SetId("Button");

	layout->AddChild(button);

	tbUIEditField* edit = new tbUIEditField(context_);
	layout->AddChild(edit);
	edit->SetId("EditField");

	window_ = new tbUIWindow(context_);
	window_->SetSettings((UI_WINDOW_SETTINGS)(UI_WINDOW_SETTINGS_TITLEBAR | UI_WINDOW_SETTINGS_CLOSE_BUTTON));

	window_->SetText("Hello Atomic GUI!");

	window_->AddChild(layout);

	window_->ResizeToFitContent();

	uiView_->AddChild(window_);
	window_->Center();
}

void HelloGui::SubscribeToEvents()
{
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloGui, HandleUpdate));

	SubscribeToEvent(E_WIDGETEVENT, URHO3D_HANDLER(HelloGui, HandleWidgetEvent));
	SubscribeToEvent(E_WIDGETDELETED, URHO3D_HANDLER(HelloGui, HandleWidgetDeleted));
}

void HelloGui::HandleWidgetEvent(StringHash eventType, VariantMap& eventData)
{
	using namespace WidgetEvent;

	if (eventData[P_TYPE] == UI_EVENT_TYPE_CLICK)
	{
		tbUIWidget* widget = static_cast<tbUIWidget*>(eventData[P_TARGET].GetPtr());
		if (widget)
		{
			window_->SetText(ToString("Hello: %s", widget->GetId().CString()));
		}

	}
}

void HelloGui::HandleWidgetDeleted(StringHash eventType, VariantMap& eventData)
{

}


void HelloGui::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	// Do nothing for now, could be extended to eg. animate the display
}
