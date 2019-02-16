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
#include <TurboBadger/tb_window.h>

#include "../../IO/Log.h"

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIDockWindow.h"

using namespace tb;

namespace Urho3D
{

/// UIDockWindow is the host for UI content which has been transferred from the main window.
tbUIDockWindow::tbUIDockWindow(Context* context, bool createWidget, const String& title, tbUIWidget *contentptr, int minwidth, int minheight ) : tbUIWindow(context, false)
{
    if (createWidget)
    {
        tb:TBWidget *contents = contentptr->GetInternalWidget();
        widget_ = new TBDockWindow( title.CString(), contents, minwidth, minheight );
        widget_->SetDelegate(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

tbUIDockWindow::~tbUIDockWindow()
{
}

/// ID of the redock widget. If specified, pressing the dock button, will move the content there.
void tbUIDockWindow::SetDockOrigin( String dockid )
{
    if (!widget_)
        return;

   ((TBDockWindow*)widget_)->SetDockOrigin(TBIDC(dockid.CString()));

}

/// This returns a pointer to the docked content.
tbUIWidget *tbUIDockWindow::GetDockContent()
{
    if (!widget_)
        return NULL;

    TBWidget* content = ((TBDockWindow*)widget_)->GetDockContent();

    if (!content)
        return 0;

    tbUI* ui = GetSubsystem<tbUI>();

    return ui->WrapWidget(content);

}

/// Returns if the UIDockWindow contains docked content.
bool tbUIDockWindow::HasDockContent()
{
    if (!widget_)
        return false;

    return ((TBDockWindow*)widget_)->HasDockContent();
}

/// Transfers the dock content to the target widget
void tbUIDockWindow::Dock( tbUIWidget *target )
{
    if (!widget_)
        return;

    if (!target)
        return;

    ((TBDockWindow*)widget_)->Dock( target->GetInternalWidget() );
}

/// Show the UIDockWindow, and optional position
void tbUIDockWindow::Show( tbUIWidget *host, int xpos, int ypos )
{
    if (!widget_)
        return;

    if (!host)
        return;

    ((TBDockWindow*)widget_)->Show( host->GetInternalWidget(), xpos, ypos );

}

bool tbUIDockWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return tbUIWidget::OnEvent(ev);
}

void tbUIDockWindow::SetAxis(UI_AXIS axis)
{
    if (!widget_)
        return;

    widget_->SetAxis((AXIS) axis);
}




}
