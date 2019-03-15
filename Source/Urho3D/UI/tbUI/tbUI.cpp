//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_system.h>
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/animation/tb_widget_animation.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>
#include <TurboBadger/tb_font_renderer.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/tb_widgets_reader.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_inline_select.h>
#include <TurboBadger/tb_tab_container.h>
#include <TurboBadger/tb_toggle_container.h>
#include <TurboBadger/tb_scroll_container.h>
#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_popup_window.h>
#include <TurboBadger/image/tb_image_widget.h>
#include <TurboBadger/tb_atomic_widgets.h>

void register_tbbf_font_renderer();
void register_stb_font_renderer();
void register_freetype_font_renderer();

using namespace tb;

#include "../../Core/CoreEvents.h"
#include "../../IO/Log.h"
#include "../../IO/FileSystem.h"
#include "../../Input/Input.h"
#include "../../Input/InputEvents.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/GraphicsEvents.h"
#include "../../Graphics/Texture2D.h"
#include "../../Graphics/VertexBuffer.h"

#include "tbUIEvents.h"

#include "tbUIRenderer.h"
#include "tbUI.h"
#include "tbUIView.h"
#include "tbUIButton.h"
#include "tbUITextField.h"
#include "tbUIEditField.h"
#include "tbUILayout.h"
#include "tbUIImageWidget.h"
#include "tbUIClickLabel.h"
#include "tbUICheckBox.h"
#include "tbUISelectList.h"
#include "tbUIMessageWindow.h"
#include "tbUISkinImage.h"
#include "tbUITabContainer.h"
#include "tbUISceneView.h"
#include "tbUIDragDrop.h"
#include "tbUIContainer.h"
#include "tbUISection.h"
#include "tbUIInlineSelect.h"
#include "tbUIScrollContainer.h"
#include "tbUISeparator.h"
#include "tbUIDimmer.h"
#include "tbUISelectDropdown.h"
#include "tbUIMenuWindow.h"
#include "tbUIPopupWindow.h"
#include "tbUISlider.h"
#include "tbUIColorWidget.h"
#include "tbUIColorWheel.h"
#include "tbUIBargraph.h"
#include "tbUIPromptWindow.h"
#include "tbUIFinderWindow.h"
#include "tbUIPulldownMenu.h"
#include "tbUIComponent.h"
#include "tbUIRadioButton.h"
#include "tbUIScrollBar.h"
#include "tbUIDockWindow.h"
#include "tbUIButtonGrid.h"

namespace tb
{

	void TBSystem::RescheduleTimer(double fire_time)
	{

	}

}

namespace Urho3D
{
	void RegisterTBUILibrary(Context* context)
	{
		tbUIComponent::RegisterObject(context);
	}

	WeakPtr<Context> tbUI::uiContext_;

	tbUI::tbUI(Context* context) :
		Object(context),
		rootWidget_(0),
		inputDisabled_(false),
		keyboardDisabled_(false),
		initialized_(false),
		skinLoaded_(false),
		consoleVisible_(false),
		exitRequested_(false),
		changedEventsBlocked_(0),
		tooltipHoverTime_(0.0f)
	{

		RegisterTBUILibrary(context);

		SubscribeToEvent(E_EXITREQUESTED, URHO3D_HANDLER(tbUI, HandleExitRequested));
	}

	tbUI::~tbUI()
	{
		if (initialized_)
		{
			initialized_ = false;

			tb::TBAnimationManager::AbortAllAnimations();
			tb::TBWidgetListener::RemoveGlobalListener(this);

			TBFile::SetReaderFunction(0);
			TBID::tbidRegisterCallback = 0;

			tb::TBWidgetsAnimationManager::Shutdown();

			delete rootWidget_;
			widgetWrap_.Clear();

			// leak
			//delete TBUIRenderer::renderer_;

			tb_core_shutdown();
		}

		uiContext_ = 0;

	}

	void tbUI::HandleExitRequested(StringHash eventType, VariantMap& eventData)
	{
		Shutdown();
	}

	void tbUI::Shutdown()
	{

	}

	bool tbUI::GetFocusedWidget()
	{
		if (!TBWidget::focused_widget)
			return false;

		return TBWidget::focused_widget->IsOfType<TBEditField>();
	}

	void tbUI::SetBlockChangedEvents(bool blocked)
	{
		if (blocked)
			changedEventsBlocked_++;
		else
		{
			changedEventsBlocked_--;

			if (changedEventsBlocked_ < 0)
			{
				URHO3D_LOGERROR("UI::BlockChangedEvents - mismatched block calls, setting to 0");
				changedEventsBlocked_ = 0;
			}
		}

	}

	void tbUI::Initialize(const String& languageFile)
	{
		Graphics* graphics = GetSubsystem<Graphics>();
		assert(graphics);
		assert(graphics->IsInitialized());
		graphics_ = graphics;

		uiContext_ = context_;

		TBFile::SetReaderFunction(TBFileReader);
		TBID::tbidRegisterCallback = tbUI::TBIDRegisterStringCallback;

		TBWidgetsAnimationManager::Init();

		renderer_ = new tbUIRenderer(graphics_->GetContext());
		tb_core_init(renderer_, languageFile.CString());

		//register_tbbf_font_renderer();
		//register_stb_font_renderer();
		register_freetype_font_renderer();

		rootWidget_ = new TBWidget();

		int width = graphics_->GetWidth();
		int height = graphics_->GetHeight();
		rootWidget_->SetSize(width, height);
		rootWidget_->SetVisibilility(tb::WIDGET_VISIBILITY_VISIBLE);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(tbUI, HandleUpdate));
		SubscribeToEvent(E_SCREENMODE, URHO3D_HANDLER(tbUI, HandleScreenMode));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(tbUI, HandlePostUpdate));
		SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(tbUI, HandleRenderUpdate));

		// register the UIDragDrop subsystem (after we have subscribed to events, so it is processed after)
		context_->RegisterSubsystem(new tbUIDragDrop(context_));

		tb::TBWidgetListener::AddGlobalListener(this);

		initialized_ = true;

		//TB_DEBUG_SETTING(LAYOUT_BOUNDS) = 1;
	}

	void tbUI::LoadSkin(const String& skin, const String& overrideSkin)
	{
		// Load the default skin, and override skin (if any)
		tb::g_tb_skin->Load(skin.CString(), overrideSkin.CString());
		skinLoaded_ = true;
	}

	void tbUI::LoadDefaultPlayerSkin()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		String skin = "TB/resources/skin/skin.tb.txt";
		String overrideSkin;

		// see if we have an override skin
		SharedPtr<File> skinFile = cache->GetFile("TB/resources/skin/skin.tb.txt", false);
		if (skinFile.NotNull())
		{
			skinFile->Close();
			skin = "TB/resources/skin/skin.tb.txt";
		}

		// see if we have an override skin
		SharedPtr<File> overrideFile = cache->GetFile("TB/resources/Override/skin.ui.txt", false);

		if (overrideFile.NotNull())
		{
			overrideFile->Close();
			overrideSkin = "TB/resources/Override/skin.ui.txt";
		}

		LoadSkin(skin, overrideSkin);

		if (skin == "TB/resources/skin/skin.tb.txt")
		{
			AddFont("Fonts/BlueHighway.ttf", "BlueHighway");
			SetDefaultFont("BlueHighway", 14);
		}
	}

	void tbUI::SetDefaultFont(const String& name, int size)
	{
		tb::TBFontDescription fd;
		fd.SetID(tb::TBIDC(name.CString()));
		fd.SetSize(tb::g_tb_skin->GetDimensionConverter()->DpToPx(size));
		tb::g_font_manager->SetDefaultFontDescription(fd);

		// Create the font now.
		tb::TBFontFace *font = tb::g_font_manager->CreateFontFace(tb::g_font_manager->GetDefaultFontDescription());

		// Render some glyphs in one go now since we know we are going to use them. It would work fine
		// without this since glyphs are rendered when needed, but with some extra updating of the glyph bitmap.
		if (font)
			font->RenderGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~•·åäöüÅÄÖÜ");
	}

	void tbUI::AddFont(const String& fontFile, const String& name)
	{
		tb::g_font_manager->AddFontInfo(fontFile.CString(), name.CString());
	}

	void tbUI::AddUIView(tbUIView* uiView)
	{
		rootWidget_->AddChild(uiView->GetInternalWidget());
		uiViews_.Push(SharedPtr<tbUIView>(uiView));

		if (!focusedView_ && uiView)
		{
			uiView->SetFocus();
		}
	}

	void tbUI::RemoveUIView(tbUIView* uiView)
	{
		if (focusedView_ == uiView)
		{
			SetFocusedView(0);
		}

		rootWidget_->RemoveChild(uiView->GetInternalWidget());
		uiViews_.Remove(SharedPtr<tbUIView>(uiView));
	}

	void tbUI::SetFocusedView(tbUIView* uiView)
	{
		if (focusedView_ == uiView)
		{
			return;
		}

		focusedView_ = uiView;

		if (focusedView_)
		{
			focusedView_->BecomeFocused();
		}
		else
		{
			focusedView_ = 0;

			// look for first auto activated UIView, and recurse
			Vector<SharedPtr<tbUIView>>::Iterator itr = uiViews_.Begin();

			while (itr != uiViews_.End())
			{
				if ((*itr)->GetAutoFocus())
				{
					SetFocusedView(*itr);
					return;
				}

				itr++;
			}

		}
	}

	void tbUI::Render(bool resetRenderTargets)
	{
		Vector<SharedPtr<tbUIView>>::Iterator itr = uiViews_.Begin();

		while (itr != uiViews_.End())
		{
			(*itr)->Render(resetRenderTargets);

			itr++;
		}

	}


	void tbUI::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
	{
		SendEvent(E_UIUPDATE);

		TBMessageHandler::ProcessMessages();
		TBAnimationManager::Update();

		rootWidget_->InvokeProcessStates();
		rootWidget_->InvokeProcess();
	}

	void tbUI::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
		Vector<SharedPtr<tbUIView>>::Iterator itr = uiViews_.Begin();

		while (itr != uiViews_.End())
		{
			(*itr)->UpdateUIBatches();
			itr++;
		}

	}

	void tbUI::TBFileReader(const char* filename, void** data, unsigned* length)
	{
		*data = 0;
		*length = 0;

		ResourceCache* cache = uiContext_->GetSubsystem<ResourceCache>();
		SharedPtr<File> file = cache->GetFile(filename);
		if (!file || !file->IsOpen())
		{
			URHO3D_LOGERRORF("UI::TBFileReader: Unable to load file: %s", filename);
			return;
		}

		unsigned size = file->GetSize();

		if (!size)
			return;

		void* _data = malloc(size);
		if (!_data)
			return;

		if (file->Read(_data, size) != size)
		{
			free(_data);
			return;
		}

		*length = size;
		*data = _data;

	}

	void tbUI::GetTBIDString(unsigned id, String& value)
	{
		if (!id)
		{
			value = "";
		}
		else
		{
			value = tbidToString_[id];
		}
	}

	void tbUI::TBIDRegisterStringCallback(unsigned id, const char* value)
	{
		uiContext_->GetSubsystem<tbUI>()->tbidToString_[id] = String(value);
	}

	bool tbUI::LoadResourceFile(TBWidget* widget, const String& filename)
	{

		tb::TBNode node;

		if (!node.ReadFile(filename.CString()))
			return false;

		tb::g_widgets_reader->LoadNodeTree(widget, &node);
		return true;
	}


	void tbUI::HandleScreenMode(StringHash eventType, VariantMap& eventData)
	{
		using namespace ScreenMode;
		rootWidget_->SetSize(eventData[P_WIDTH].GetInt(), eventData[P_HEIGHT].GetInt());
	}

	void tbUI::HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		if (exitRequested_) {
			SendEvent(E_EXITREQUESTED);
			exitRequested_ = false;
			return;
		}

		tooltipHoverTime_ += eventData[Update::P_TIMESTEP].GetFloat();

		if (tooltipHoverTime_ >= 0.5f)
		{
			tbUIWidget* hoveredWidget = GetHoveredWidget();
			if (hoveredWidget && !tooltip_ && (hoveredWidget->GetShortened() || hoveredWidget->GetTooltip().Length() > 0))
			{
				tooltip_ = new tbUIPopupWindow(context_, true, hoveredWidget, "tooltip");
				tbUILayout* tooltipLayout = new tbUILayout(context_, UI_AXIS_Y, true);
				if (hoveredWidget->GetShortened())
				{
					tbUITextField* fullTextField = new tbUITextField(context_, true);
					fullTextField->SetText(hoveredWidget->GetText());
					tooltipLayout->AddChild(fullTextField);
				}
				if (hoveredWidget->GetTooltip().Length() > 0)
				{
					tbUITextField* tooltipTextField = new tbUITextField(context_, true);
					tooltipTextField->SetText(hoveredWidget->GetTooltip());
					tooltipLayout->AddChild(tooltipTextField);
				}
				Input* input = GetSubsystem<Input>();
				IntVector2 mousePosition = input->GetMousePosition();
				tooltip_->AddChild(tooltipLayout);
				tooltip_->Show(mousePosition.x_ + 8, mousePosition.y_ + 8);
			}
		}
		else
		{
			if (tooltip_) tooltip_->Close();
		}

	}

	tbUIWidget* tbUI::GetHoveredWidget()
	{
		return WrapWidget(TBWidget::hovered_widget);
	}

	bool tbUI::IsWidgetWrapped(tb::TBWidget* widget)
	{
		return widgetWrap_.Contains(widget);
	}

	bool tbUI::UnwrapWidget(tb::TBWidget* widget)
	{
		if (widgetWrap_.Contains(widget))
		{
			widget->SetDelegate(0);
			widgetWrap_.Erase(widget);
			return true;
		}

		return false;

	}

	void tbUI::PruneUnreachableWidgets()
	{
		HashMap<tb::TBWidget*, SharedPtr<tbUIWidget>>::Iterator itr;

		for (itr = widgetWrap_.Begin(); itr != widgetWrap_.End(); )
		{
			if ((*itr).first_->GetParent() || (*itr).second_->Refs() > 1)
			{
				itr++;
				continue;
			}

			itr.GotoNext();

			VariantMap eventData;
			eventData[WidgetDeleted::P_WIDGET] = (tbUIWidget*)(*itr).second_;
			(*itr).second_->SendEvent(E_WIDGETDELETED, eventData);

			tb::TBWidget* toDelete = (*itr).first_;
			UnwrapWidget(toDelete);
			delete toDelete;

		}
	}

	void tbUI::WrapWidget(tbUIWidget* widget, tb::TBWidget* tbwidget)
	{
		assert(!widgetWrap_.Contains(tbwidget));
		widgetWrap_[tbwidget] = widget;
	}

	tbUIWidget* tbUI::WrapWidget(tb::TBWidget* widget)
	{
		if (!widget)
			return NULL;

		if (widgetWrap_.Contains(widget))
			return widgetWrap_[widget];

		// switch this to use a factory?

		// this is order dependent as we're using IsOfType which also works if a base class

		if (widget->IsOfType<TBPopupWindow>())
		{
			tbUIPopupWindow* popupWindow = new tbUIPopupWindow(context_, false);
			popupWindow->SetWidget(widget);
			WrapWidget(popupWindow, widget);
			return popupWindow;
		}

		if (widget->IsOfType<TBDimmer>())
		{
			tbUIDimmer* dimmer = new tbUIDimmer(context_, false);
			dimmer->SetWidget(widget);
			WrapWidget(dimmer, widget);
			return dimmer;
		}

		if (widget->IsOfType<TBScrollContainer>())
		{
			tbUIScrollContainer* container = new tbUIScrollContainer(context_, false);
			container->SetWidget(widget);
			WrapWidget(container, widget);
			return container;
		}

		if (widget->IsOfType<TBInlineSelect>())
		{
			tbUIInlineSelect* select = new tbUIInlineSelect(context_, false);
			select->SetWidget(widget);
			WrapWidget(select, widget);
			return select;
		}

		if (widget->IsOfType<TBSlider>())
		{
			tbUISlider* slider = new tbUISlider(context_, false);
			slider->SetWidget(widget);
			WrapWidget(slider, widget);
			return slider;
		}

		if (widget->IsOfType<TBScrollBar>())
		{
			tbUIScrollBar* slider = new tbUIScrollBar(context_, false);
			slider->SetWidget(widget);
			WrapWidget(slider, widget);
			return slider;
		}

		if (widget->IsOfType<TBColorWidget>())
		{
			tbUIColorWidget* colorWidget = new tbUIColorWidget(context_, false);
			colorWidget->SetWidget(widget);
			WrapWidget(colorWidget, widget);
			return colorWidget;
		}

		if (widget->IsOfType<TBColorWheel>())
		{
			tbUIColorWheel* colorWheel = new tbUIColorWheel(context_, false);
			colorWheel->SetWidget(widget);
			WrapWidget(colorWheel, widget);
			return colorWheel;
		}

		if (widget->IsOfType<TBSection>())
		{
			tbUISection* section = new tbUISection(context_, false);
			section->SetWidget(widget);
			WrapWidget(section, widget);
			return section;
		}

		if (widget->IsOfType<TBSeparator>())
		{
			tbUISeparator* sep = new tbUISeparator(context_, false);
			sep->SetWidget(widget);
			WrapWidget(sep, widget);
			return sep;
		}

		if (widget->IsOfType<TBContainer>())
		{
			tbUIContainer* container = new tbUIContainer(context_, false);
			container->SetWidget(widget);
			WrapWidget(container, widget);
			return container;
		}

		if (widget->IsOfType<TBSelectDropdown>())
		{
			tbUISelectDropdown* select = new tbUISelectDropdown(context_, false);
			select->SetWidget(widget);
			WrapWidget(select, widget);
			return select;
		}

		if (widget->IsOfType<TBPulldownMenu>())
		{
			tbUIPulldownMenu* select = new tbUIPulldownMenu(context_, false);
			select->SetWidget(widget);
			WrapWidget(select, widget);
			return select;
		}

		if (widget->IsOfType<TBButton>())
		{
			// don't wrap the close button of a TBWindow.close
			if (widget->GetID() == TBIDC("TBWindow.close"))
				return 0;

			tbUIButton* button = new tbUIButton(context_, false);
			button->SetWidget(widget);
			WrapWidget(button, widget);
			return button;
		}

		if (widget->IsOfType<TBTextField>())
		{
			tbUITextField* textfield = new tbUITextField(context_, false);
			textfield->SetWidget(widget);
			WrapWidget(textfield, widget);
			return textfield;
		}

		if (widget->IsOfType<TBEditField>())
		{
			tbUIEditField* editfield = new tbUIEditField(context_, false);
			editfield->SetWidget(widget);
			WrapWidget(editfield, widget);
			return editfield;
		}

		if (widget->IsOfType<TBSkinImage>())
		{
			tbUISkinImage* skinimage = new tbUISkinImage(context_, "", false);
			skinimage->SetWidget(widget);
			WrapWidget(skinimage, widget);
			return skinimage;
		}

		if (widget->IsOfType<TBImageWidget>())
		{
			tbUIImageWidget* imagewidget = new tbUIImageWidget(context_, false);
			imagewidget->SetWidget(widget);
			WrapWidget(imagewidget, widget);
			return imagewidget;
		}
		if (widget->IsOfType<TBClickLabel>())
		{
			tbUIClickLabel* nwidget = new tbUIClickLabel(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBCheckBox>())
		{
			tbUICheckBox* nwidget = new tbUICheckBox(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBRadioButton>())
		{
			tbUIRadioButton* nwidget = new tbUIRadioButton(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBBarGraph>())
		{
			tbUIBargraph* nwidget = new tbUIBargraph(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBSelectList>())
		{
			tbUISelectList* nwidget = new tbUISelectList(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBMessageWindow>())
		{
			tbUIMessageWindow* nwidget = new tbUIMessageWindow(context_, NULL, "", false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBPromptWindow>())
		{
			tbUIPromptWindow* nwidget = new tbUIPromptWindow(context_, NULL, "", false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBFinderWindow>())
		{
			tbUIFinderWindow* nwidget = new tbUIFinderWindow(context_, NULL, "", false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<TBTabContainer>())
		{
			tbUITabContainer* nwidget = new tbUITabContainer(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}

		if (widget->IsOfType<tbSceneViewWidget>())
		{
			UISceneView* nwidget = new UISceneView(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}


		if (widget->IsOfType<TBLayout>())
		{
			tbUILayout* layout = new tbUILayout(context_, (UI_AXIS)widget->GetAxis(), false);
			layout->SetWidget(widget);
			WrapWidget(layout, widget);
			return layout;
		}

		if (widget->IsOfType<TBWidget>())
		{
			tbUIWidget* nwidget = new tbUIWidget(context_, false);
			nwidget->SetWidget(widget);
			WrapWidget(nwidget, widget);
			return nwidget;
		}


		return 0;
	}

	void tbUI::ConvertEvent(tbUIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data)
	{
		String refid;

		GetTBIDString(ev.ref_id, refid);
		if (refid.Length() < 1)
		{
			refid = target->GetId();
		}

		int key = ev.key;

		if (ev.special_key)
		{
			switch (ev.special_key)
			{
			case TB_KEY_ENTER:
				key = KEY_RETURN;
				break;
			case TB_KEY_BACKSPACE:
				key = KEY_BACKSPACE;
				break;
			case TB_KEY_DELETE:
				key = KEY_DELETE;
				break;
			case TB_KEY_DOWN:
				key = KEY_DOWN;
				break;
			case TB_KEY_UP:
				key = KEY_UP;
				break;
			case TB_KEY_LEFT:
				key = KEY_LEFT;
				break;
			case TB_KEY_RIGHT:
				key = KEY_RIGHT;
				break;
			default:
				break;
			}
		}

		unsigned modifierKeys = 0;

		if (ev.special_key && TB_SHIFT)
			modifierKeys |= QUAL_SHIFT;
		if (ev.special_key && TB_CTRL)
			modifierKeys |= QUAL_CTRL;
		if (ev.special_key && TB_ALT)
			modifierKeys |= QUAL_ALT;

		using namespace WidgetEvent;
		data[P_TARGET] = target;
		data[P_TYPE] = (unsigned)ev.type;
		data[P_X] = ev.target_x;
		data[P_Y] = ev.target_y;
		data[P_DELTAX] = ev.delta_x;
		data[P_DELTAY] = ev.delta_y;
		data[P_COUNT] = ev.count;
		data[P_KEY] = key;

		data[P_SPECIALKEY] = (unsigned)ev.special_key;
		data[P_MODIFIERKEYS] = modifierKeys;
		data[P_REFID] = refid;
		data[P_TOUCH] = (unsigned)ev.touch;
	}

	void tbUI::OnWidgetDelete(tb::TBWidget *widget)
	{

	}

	bool tbUI::OnWidgetDying(tb::TBWidget *widget)
	{
		return false;
	}

	void tbUI::OnWindowClose(tb::TBWindow *window)
	{
		if (widgetWrap_.Contains(window))
		{
			tbUIWidget* widget = widgetWrap_[window];
			VariantMap eventData;
			eventData[WindowClosed::P_WINDOW] = widget;
			widget->SendEvent(E_WINDOWCLOSED, eventData);
		}
	}

	void tbUI::OnWidgetFocusChanged(TBWidget *widget, bool focused)
	{
		if (widgetWrap_.Contains(widget))
		{
			VariantMap evData;
			tbUIWidget* uiWidget = widgetWrap_[widget];
			evData[UIWidgetFocusChanged::P_WIDGET] = uiWidget;
			evData[UIWidgetFocusChanged::P_FOCUSED] = focused;
			uiWidget->SendEvent(E_UIWIDGETFOCUSCHANGED, evData);
		}
	}

	void tbUI::HandleConsoleClosed(StringHash eventType, VariantMap& eventData)
	{
		consoleVisible_ = false;
	}

	tbUIWidget* tbUI::GetWidgetAt(int x, int y, bool include_children)
	{
		if (!initialized_)
			return 0;
		return WrapWidget(rootWidget_->GetWidgetAt(x, y, include_children));
	}

	bool tbUI::OnWidgetInvokeEvent(tb::TBWidget *widget, const tb::TBWidgetEvent &ev)
	{
		tbUI* ui = GetSubsystem<tbUI>();

		if ((ev.type == EVENT_TYPE_CHANGED && !ui->GetBlockChangedEvents()) || ev.type == EVENT_TYPE_KEY_UP)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;

		}
		else if (ev.type == EVENT_TYPE_RIGHT_POINTER_UP)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;

		}
		else if (ev.type == EVENT_TYPE_POINTER_DOWN)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;
		}
		else if (ev.type == EVENT_TYPE_SHORTCUT)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;
		}
		else if (ev.type == EVENT_TYPE_TAB_CHANGED)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;
		}
		else if (ev.type == EVENT_TYPE_CLICK)
		{
			if (ev.target && ev.target->GetID() == TBID("__popup-menu"))
			{
				// popup menu
				VariantMap eventData;
				eventData[PopupMenuSelect::P_BUTTON] = this;
				String id;
				ui->GetTBIDString(ev.ref_id, id);
				eventData[PopupMenuSelect::P_REFID] = id;
				SendEvent(E_POPUPMENUSELECT, eventData);

				return true;
			}
			else
			{
				VariantMap eventData;
				ConvertEvent(WrapWidget(ev.target), ev, eventData);
				SendEvent(E_WIDGETEVENT, eventData);

				if (eventData[WidgetEvent::P_HANDLED].GetBool())
					return true;
			}
		}
		if (ev.type == EVENT_TYPE_CUSTOM)
		{
			VariantMap eventData;
			ConvertEvent(WrapWidget(ev.target), ev, eventData);
			SendEvent(E_WIDGETEVENT, eventData);

			if (eventData[WidgetEvent::P_HANDLED].GetBool())
				return true;
		}

		return false;
	}

	void tbUI::DebugShowSettingsWindow(tbUIWidget* parent)
	{

#ifdef ATOMIC_DEBUG
		if (parent && parent->GetInternalWidget())
			tb::ShowDebugInfoSettingsWindow(parent->GetInternalWidget());
#endif

	}

}
