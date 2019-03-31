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
#include <TurboBadger/tb_toggle_container.h>

#include "../../IO/Log.h"

#include "tbUIEvents.h"
#include "tbUI.h"
#include "tbUISection.h"

using namespace tb;

namespace Urho3D
{

tbUISection::tbUISection(Context* context, bool createWidget) : tbUIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSection();
		widget_->SetDelegate(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

bool tbUISection::OnEvent(const tb::TBWidgetEvent &ev)
{
    return tbUIWidget::OnEvent(ev);
}

void tbUISection::AddChild(tbUIWidget* child)
{
    if (!widget_ || !child || !child->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChild(child->GetInternalWidget());
}

void tbUISection::AddChildAfter(tbUIWidget* child, tbUIWidget* otherChild)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !otherChild || !otherChild->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), tb::WIDGET_Z_REL_AFTER, otherChild->GetInternalWidget());
}

void tbUISection::AddChildBefore(tbUIWidget* child, tbUIWidget* otherChild)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !otherChild || !otherChild->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), tb::WIDGET_Z_REL_BEFORE, otherChild->GetInternalWidget());
}

void tbUISection::AddChildRelative(tbUIWidget* child, UI_WIDGET_Z_REL z, tbUIWidget* reference)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !reference || !reference->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), (WIDGET_Z_REL)z, reference->GetInternalWidget());
}

void tbUISection::RemoveChild(tbUIWidget* child, bool cleanup)
{
    if (!widget_ || !child)
        return;

    TBWidget* childw = child->GetInternalWidget();

    if (!childw)
        return;

    TBSafeCast<TBSection>(widget_)->RemoveToggleChild(childw);
}

void tbUISection::DeleteAllChildren()
{
    if (!widget_)
        return;

    TBSafeCast<TBSection>(widget_)->DeleteAllChildren();
}

tbUIWidget* tbUISection::GetFirstChild()
{
    if (!widget_)
        return NULL;

    return GetSubsystem<tbUI>()->WrapWidget(TBSafeCast<TBSection>(widget_)->GetFirstToggleChild());
}

tbUIWidget* tbUISection::GetWidget(const String& id)
{
    if (!widget_)
        return 0;

    TBWidget* child = TBSafeCast<TBSection>(widget_)->GetToggleWidgetById(TBID(id.CString()));

    if (!child)
        return 0;

    tbUI* ui = GetSubsystem<tbUI>();
    return ui->WrapWidget(child);
}
}
