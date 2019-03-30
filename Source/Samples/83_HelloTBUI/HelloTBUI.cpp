//
// Copyright (c) 2008-2016 the Urho3D project.
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2018-2019 the Urho3D project.
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
#include <Urho3D/UI/tbUI/tbUISlider.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include "HelloTBUI.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(HelloGui)

HelloGui::HelloGui(Context* context) :
	Sample(context)
{
	ExampleSerializeable::RegisterObject(context);
}

void HelloGui::Start()
{
	// Execute base class startup
	Sample::Start();

	// Create "Hello GUI"
	CreateUI();

	// Finally subscribe to the update event. Note that by subscribing events at this
	// point we have already missed some events
	// like the ScreenMode event sent by the Graphics subsystem when opening the
	// application window. To catch those as well we
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

	const int min_with = 150;

	// Input Area
	tbUILayout* input_layout = new tbUILayout(context_);
	input_layout->SetAxis(UI_AXIS_Y);

	tbUILayout* name_layout = new tbUILayout(context_);
	name_layout->SetAxis(UI_AXIS_X);

	tbUITextField* lbl_name = new tbUITextField(context_);
	lbl_name->SetText("Player Name");
	lbl_name->SetLayoutMinWidth(min_with);
	lbl_name->SetTextAlign(UI_TEXT_ALIGN_LEFT);
	name_layout->AddChild(lbl_name);

	tbUIEditField* edit_name = new tbUIEditField(context_);
	name_layout->AddChild(edit_name);
	edit_name->SetLayoutMinWidth(min_with);
	edit_name->SetId("edit_name");

	tbUILayout* health_layout = new tbUILayout(context_);
	health_layout->SetAxis(UI_AXIS_X);

	tbUITextField* lbl_health = new tbUITextField(context_);
	lbl_health->SetText("Health");
	lbl_health->SetTextAlign(UI_TEXT_ALIGN_LEFT);
	lbl_health->SetLayoutMinWidth(min_with);
	health_layout->AddChild(lbl_health);

	tbUISlider* edit_health = new tbUISlider(context_);
	health_layout->AddChild(edit_health);
	edit_health->SetId("edit_health");
	edit_health->SetLimits(0.0f, 100.0f);
	edit_health->SetLayoutMinWidth(min_with);

	input_layout->AddChild(name_layout);
	input_layout->AddChild(health_layout);

	window_ = new tbUIWindow(context_);
	window_->SetSettings((UI_WINDOW_SETTINGS)(UI_WINDOW_SETTINGS_TITLEBAR | UI_WINDOW_SETTINGS_CLOSE_BUTTON));

	window_->SetText("Change values here!");

	window_->AddChild(input_layout);

	// Widget need a size, otherwise they wont show up.
	window_->ResizeToFitContent();

	uiView_->AddChild(window_);
	window_->Center();
	window_->SetPosition(window_->GetRect().left_ - 200, window_->GetRect().top_);

	example_ = SharedPtr<ExampleSerializeable>(new ExampleSerializeable(context_));
	example_->SetPlayerName("Player 1");
	example_->SetPlayerHealth(0);
	window_->SetSerializable("edit_name", example_, "PlayerName");
	window_->SetSerializable("edit_health", example_, "PlayerHealth");

	edit_name->SetText(example_->GetPlayerName());

	// Update from the serializable after UI_EVENT_TYPE_CHANGED was send
	output_ = new tbUIWindow(context_);
	output_->SetSettings((UI_WINDOW_SETTINGS)(UI_WINDOW_SETTINGS_TITLEBAR | UI_WINDOW_SETTINGS_CLOSE_BUTTON));
	output_->SetText("See effect here!");

	tbUILayout* input_layout2 = new tbUILayout(context_);
	input_layout2->SetAxis(UI_AXIS_Y);

	tbUILayout* name_layout2 = new tbUILayout(context_);
	name_layout2->SetAxis(UI_AXIS_X);

	tbUITextField* lbl_name2 = new tbUITextField(context_);
	lbl_name2->SetText("Player Name");
	lbl_name2->SetLayoutMinWidth(min_with);
	lbl_name2->SetTextAlign(UI_TEXT_ALIGN_LEFT);
	name_layout2->AddChild(lbl_name2);

	tbUITextField* edit_name2 = new tbUITextField(context_);
	name_layout2->AddChild(edit_name2);
	edit_name2->SetLayoutMinWidth(min_with);
	edit_name2->SetId("edit_name2");

	tbUILayout* health_layout2 = new tbUILayout(context_);
	health_layout2->SetAxis(UI_AXIS_X);

	tbUITextField* lbl_health2 = new tbUITextField(context_);
	lbl_health2->SetText("Health");
	lbl_health2->SetTextAlign(UI_TEXT_ALIGN_LEFT);
	lbl_health2->SetLayoutMinWidth(min_with);
	health_layout2->AddChild(lbl_health2);

	tbUITextField* edit_health2 = new tbUITextField(context_);
	health_layout2->AddChild(edit_health2);
	edit_health2->SetId("edit_health2");
	edit_health2->SetLayoutMinWidth(min_with);

	input_layout2->AddChild(name_layout2);
	input_layout2->AddChild(health_layout2);

	edit_name2->SetText(example_->GetPlayerName());
	edit_health2->SetText("0");

	output_->AddChild(input_layout2);
	output_->ResizeToFitContent();
	uiView_->AddChild(output_);
	output_->Center();
	output_->SetPosition(output_->GetRect().left_ + 200, output_->GetRect().top_);
}

void HelloGui::SubscribeToEvents()
{
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(HelloGui, HandleUpdate));

	SubscribeToEvent(E_WIDGETEVENT, URHO3D_HANDLER(HelloGui, HandleWidgetEvent));
	SubscribeToEvent(E_WIDGETDELETED, URHO3D_HANDLER(HelloGui, HandleWidgetDeleted));
}

void HelloGui::HandleWidgetEvent(StringHash eventType, VariantMap& eventData)
{
	using namespace WidgetEvent;

	if (eventData[P_TYPE] == UI_EVENT_TYPE_CHANGED)
	{
		if (example_ && output_)
		{
			/*URHO3D_LOGDEBUG(
				"PlayerName: " + example_->GetPlayerName() +
				" Health: " + String(example_->GetPlayerHealth()));*/

			auto player_name = output_->FindWidget("edit_name2");
			if (player_name)
			{
				player_name->SetText(example_->GetPlayerName());
			}

			auto player_health = output_->FindWidget("edit_health2");
			if (player_health)
			{
				player_health->SetText(String(example_->GetPlayerHealth()));
			}
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
