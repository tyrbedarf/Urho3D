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
#include "../../IO/FileSystem.h"

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIWindow.h"
#include "tbUIEditField.h"
#include "tbUISelectList.h"
#include "tbUIPromptWindow.h"

using namespace tb;

namespace Urho3D
{

tbUIPromptWindow::tbUIPromptWindow(Context* context, tbUIWidget* target, const String& id, bool createWidget) : tbUIWindow(context, false)
{
    if (createWidget)
    {
        widget_ = new TBPromptWindow(target ? target->GetInternalWidget() : 0, TBIDC(id.CString()));
        widget_->SetDelegate(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

tbUIPromptWindow::~tbUIPromptWindow()
{
}

void tbUIPromptWindow::Show(const String &title, const String &message, const String &preset, int dimmer, int width, int height)
{
    if (!widget_)
        return;

    ((TBPromptWindow*)widget_)->Show(title.CString(), message.CString(), preset.CString(), dimmer, width, height);

}

bool tbUIPromptWindow::OnEvent(const tb::TBWidgetEvent &ev)
{

    if ( ev.type == EVENT_TYPE_CLICK
        && ( ev.ref_id == TBID("TBPromptWindow.ok") || ev.ref_id == TBID("TBPromptWindow.cancel") ))
    {
        tbUI* ui = GetSubsystem<tbUI>();
        VariantMap eventData;

        String title = "PromptWindow";
        TBStr tbtext;
        if(  widget_ && (TBWindow*)widget_->GetText(tbtext) )
            title = tbtext.CStr();

        eventData[UIPromptComplete::P_TITLE] = title;
        eventData[UIPromptComplete::P_SELECTED] = "";
        eventData[UIPromptComplete::P_REASON] = "CANCEL";

        if ( ev.ref_id == TBID("TBPromptWindow.ok") )
        {
            eventData[UIPromptComplete::P_REASON] = "OK";
            tbUIWidget *ewidget = GetEditWidget();
            if( ewidget)
                eventData[UIPromptComplete::P_SELECTED] = ewidget->GetText();
        }
        ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
        SendEvent(E_UIPROMPTCOMPLETE, eventData);

        if (eventData[WidgetEvent::P_HANDLED].GetBool())
           return true;
    }

    return tbUIWindow::OnEvent(ev);
}

tbUIWidget* tbUIPromptWindow::GetEditWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBEditField>(UIPROMPTEDITID);
    if (!child)
        return 0;
    tbUI* ui = GetSubsystem<tbUI>();
    return ui->WrapWidget(child);
}

}
