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
#include <TurboBadger/tb_popup_window.h>

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIWidget.h"
#include "tbUIPopupWindow.h"

using namespace tb;

namespace Urho3D
{

tbUIPopupWindow::tbUIPopupWindow(Context* context, bool createWidget, tbUIWidget* target, const String& id) : tbUIWidget(context, false)
{
    if (createWidget)
    {
        //if we should create widget, then target shouldn't be null
        assert(target);
        widget_ = new TBPopupWindow(target->GetInternalWidget());
        widget_->SetID(TBIDC(id.CString()));
        widget_->SetDelegate(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

tbUIPopupWindow::~tbUIPopupWindow()
{
}

void tbUIPopupWindow::Show(int x, int y)
{
    if (x != -1 && y != -1)
    {
        ((TBPopupWindow*)widget_)->Show(TBPopupAlignment(TBPoint(x, y)));
    }
    else
    {
        ((TBPopupWindow*)widget_)->Show(TBPopupAlignment());
    }
}

bool tbUIPopupWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

void tbUIPopupWindow::Close()
{
    if (!widget_)
        return;

    ((TBPopupWindow*)widget_)->Close();
}

}
