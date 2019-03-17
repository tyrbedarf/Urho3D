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


#include "../../IO/Log.h"

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIMessageWindow.h"

using namespace tb;

namespace Urho3D
{

tbUIMessageWindow::tbUIMessageWindow(Context* context, tbUIWidget* target, const String& id, bool createWidget) : tbUIWindow(context, false)
{
    if (createWidget)
    {
        widget_ = new TBMessageWindow(target ? target->GetInternalWidget() : 0, TBIDC(id.CString()));
		widget_->AddListener(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

void tbUIMessageWindow::Show(const String &title, const String &message, UI_MESSAGEWINDOW_SETTINGS settings, bool dimmer, int width, int height)
{
    if (!widget_)
        return;

    TBMessageWindowSettings tbsettings;
    tbsettings.msg = (TB_MSG) settings;
    tbsettings.styling = true;
    tbsettings.dimmer = dimmer;

    ((TBMessageWindow*)widget_)->Show(title.CString(), message.CString(), &tbsettings, width, height);

}

bool tbUIMessageWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return tbUIWindow::OnEvent(ev);
}

}
