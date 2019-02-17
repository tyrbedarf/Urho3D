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

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_tab_container.h>

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUITabContainer.h"
#include "tbUILayout.h"
#include "tbUIButton.h"

using namespace tb;

namespace Urho3D
{
	tbUITabContainer::tbUITabContainer(Context* context, bool createWidget) : tbUIWidget(context, false)
	{
		if (createWidget)
		{
			widget_ = new TBTabContainer();
			widget_->SetDelegate(this);
			GetSubsystem<tbUI>()->WrapWidget(this, widget_);
		}
	}

	tbUITabContainer::~tbUITabContainer()
	{

	}

	tbUIWidget* tbUITabContainer::GetCurrentPageWidget()
	{
		if (!widget_)
			return 0;

		TBWidget* w = ((TBTabContainer*)widget_)->GetCurrentPageWidget();

		return GetSubsystem<tbUI>()->WrapWidget(w);
	}

	int tbUITabContainer::GetNumPages()
	{
		if (!widget_)
			return 0;

		return ((TBTabContainer*)widget_)->GetNumPages();

	}

	tbUILayout* tbUITabContainer::GetTabLayout()
	{
		if (!widget_)
			return 0;

		TBLayout* layout = ((TBTabContainer*)widget_)->GetTabLayout();

		if (!layout)
			return 0;

		tbUI* ui = GetSubsystem<tbUI>();
		return (tbUILayout*)ui->WrapWidget(layout);
	}

	void tbUITabContainer::SetCurrentPage(int page)
	{
		if (!widget_)
			return;

		((TBTabContainer*)widget_)->SetCurrentPage(page);

	}

	bool tbUITabContainer::OnEvent(const tb::TBWidgetEvent &ev)
	{
		return tbUIWidget::OnEvent(ev);
	}


	/// returns the current page number
	int tbUITabContainer::GetCurrentPage()
	{
		if (!widget_)
			return 0;

		return ((TBTabContainer*)widget_)->GetCurrentPage();

	}

	/// deletes a tab + page, returns true if successful
	bool tbUITabContainer::DeletePage(int page)
	{
		if (!widget_ || page < 0 || page > GetNumPages() - 1)
			return false;

		tbUILayout *uil = GetTabLayout();
		if (uil)
		{
			tbUIWidget* mytab = NULL;
			int nn = 0;
			for (tbUIWidget *child = uil->GetFirstChild(); child; child = child->GetNext())
				if (nn++ == page)
					mytab = child;
			if (mytab)
			{
				mytab->UnsubscribeFromAllEvents();
				uil->RemoveChild(mytab, true);
			}
		}

		tbUIWidget *pages = GetContentRoot();
		if (pages)
		{
			tbUIWidget* mypage = NULL;
			int nn = 0;
			for (tbUIWidget *child = pages->GetFirstChild(); child; child = child->GetNext())
				if (nn++ == page)
					mypage = child;
			if (mypage)
			{
				mypage->UnsubscribeFromAllEvents();
				pages->RemoveChild(mypage, true);
			}
		}

		Invalidate();

		// tab container "feature", can not set it to the page number that was removed.
		int num = 0;
		if (page - 1 > 0) num = page - 1;
		SetCurrentPage(num);

		return true;
	}

	/// adds a tab + page from layout file
	void tbUITabContainer::AddTabPageFile(const String &tabname, const String &layoutFile, bool selecttab)
	{
		tbUIButton* button = new tbUIButton(context_);
		button->SetText(tabname);
		button->SetId(tabname);
		tbUILayout *uil = GetTabLayout();
		if (uil && button)
			uil->AddChild(button);
		tbUILayout* layout = new tbUILayout(context_);
		layout->SetAxis(UI_AXIS_Y);
		layout->SetLayoutSize(UI_LAYOUT_SIZE_AVAILABLE);
		layout->SetLayoutPosition(UI_LAYOUT_POSITION_GRAVITY);
		layout->SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION_AVAILABLE);
		layout->Load(layoutFile);
		tbUIWidget *pages = GetContentRoot();
		if (pages && layout)
			pages->AddChild(layout);

		Invalidate();

		if (selecttab)
			SetCurrentPage(GetNumPages() - 1);
	}

	/// adds a tab + page widget(s)
	void tbUITabContainer::AddTabPageWidget(const String &tabname, tbUIWidget *widget, bool selecttab)
	{
		tbUIButton* button = new tbUIButton(context_);
		button->SetText(tabname);
		button->SetId(tabname);
		tbUILayout *uil = GetTabLayout();
		if (uil && button)
			uil->AddChild(button);
		tbUILayout* layout = new tbUILayout(context_);
		layout->SetAxis(UI_AXIS_Y);
		layout->SetLayoutSize(UI_LAYOUT_SIZE_AVAILABLE);
		layout->SetLayoutPosition(UI_LAYOUT_POSITION_GRAVITY);
		layout->SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION_AVAILABLE);
		layout->AddChild(widget);
		tbUIWidget *pages = GetContentRoot();
		if (pages && layout)
			pages->AddChild(layout);

		Invalidate();

		if (selecttab)
			SetCurrentPage(GetNumPages() - 1);
	}

	/// undocks the page into a window with the tab name, and removes the tab
	void tbUITabContainer::UndockPage(int page)
	{
		if (!widget_)
			return;

		((TBTabContainer*)widget_)->UndockPage(page);

		// tab container "feature", can not set it to the page number that was removed.
		int num = 0;
		if (page - 1 > 0) num = page - 1;
		SetCurrentPage(num);

	}

	/// docks content from a UIDockWindow with specified title
	bool tbUITabContainer::DockWindow(String windowTitle)
	{
		if (!widget_)
			return false;
		bool done = ((TBTabContainer*)widget_)->DockFromWindow(windowTitle.CString());
		if (done)
			SetCurrentPage(GetNumPages() - 1);

		return done;
	}
}
