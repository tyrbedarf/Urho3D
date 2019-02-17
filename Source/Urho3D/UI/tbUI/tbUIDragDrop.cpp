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

#include "../../IO/Log.h"
#include "../../Input/Input.h"
#include "../../Input/InputEvents.h"

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIWidget.h"
#include "tbUILayout.h"
#include "tbUIFontDescription.h"
#include "tbUITextField.h"
#include "tbUIImageWidget.h"
#include "tbUISelectList.h"
#include "tbUIDragDrop.h"
#include "tbUIDragObject.h"

#ifdef ATOMIC_PLATFORM_OSX
#include "UIDragDropMac.h"
#elif ATOMIC_PLATFORM_WINDOWS
#include "tbUIDragDropWindows.h"
#else
namespace Urho3D { void InitDragAndDrop(tbUIDragDrop *dragAndDrop) {} }
#endif

using namespace tb;

namespace Urho3D
{

tbUIDragDrop::tbUIDragDrop(Context* context) : Object(context)
{
    SubscribeToEvent(E_UIUPDATE, URHO3D_HANDLER(tbUIDragDrop, HandleUIUpdate));
    SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(tbUIDragDrop,HandleMouseMove));
    SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(tbUIDragDrop,HandleMouseUp));

    SharedPtr<tbUIFontDescription> fd(new tbUIFontDescription(context));
    fd->SetId("Vera");
    fd->SetSize(12);

    dragText_ = new tbUITextField(context);
    dragText_->SetFontDescription(fd);
    dragText_->SetGravity(UI_GRAVITY_TOP);

    dragLayout_ = new tbUILayout(context);
    dragLayout_->AddChild(dragText_);

    dragLayout_->SetLayoutSize(UI_LAYOUT_SIZE_PREFERRED);
    dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_GONE);

    // put into hierarchy so we aren't pruned
    TBWidget* root = GetSubsystem<tbUI>()->GetRootWidget();
    root->AddChild(dragLayout_->GetInternalWidget());

    InitDragAndDrop(this);

}

tbUIDragDrop::~tbUIDragDrop()
{

}

void tbUIDragDrop::DragEnd()
{
    SharedPtr<tbUIDragObject> dragObject = dragObject_;
    SharedPtr<tbUIWidget> currentTargetWidget(currentTargetWidget_);
    SharedPtr<tbUIWidget> dragSourceWidget(dragSourceWidget_);


    // clean up
    currentTargetWidget_ = 0;
    dragObject_ = 0;
    dragSourceWidget_ = 0;
    dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_GONE);

    if (currentTargetWidget.Null())
    {
        return;
    }

    VariantMap dropData;
    dropData[DragEnded::P_TARGET] = currentTargetWidget;
    dropData[DragEnded::P_DRAGOBJECT] = dragObject;
    currentTargetWidget->SendEvent(E_DRAGENDED, dropData);
}

void tbUIDragDrop::HandleUIUpdate(StringHash eventType, VariantMap& eventData)
{
    Input* input = GetSubsystem<Input>();

    if (dragSourceWidget_.NotNull() || !input->IsMouseVisible() || !input->GetMouseButtonDown(MOUSEB_LEFT))
    {
        return;
    }

    if (TBWidget::hovered_widget)
    {
        // see if we have a widget with a drag object

        TBWidget* tbw = TBWidget::hovered_widget;
        tbUIWidget* widget = nullptr;

        while(tbw)
        {
            if (tbw->GetDelegate())
            {
                widget = (tbUIWidget*) tbw->GetDelegate();

                if (widget->GetDragObject())
                {
                    // TODO: check if we're in widget bounds
                    // this is going to need to be updated for drag/drop multiselect
                    break;
                }

                widget = nullptr;
            }

            tbw = tbw->GetParent();
        }

        if (!widget)
            return;

        currentTargetWidget_ = widget;
        dragSourceWidget_ = widget;
        mouseDownPosition_ = input->GetMousePosition();

    }

}

void tbUIDragDrop::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseButtonUp;

    if (dragObject_.Null())
    {
        dragSourceWidget_ = 0;
        currentTargetWidget_ = 0;
        return;
    }

    if (!(eventData[P_BUTTON].GetInt() ==  MOUSEB_LEFT))
        return;

    DragEnd();

}

void tbUIDragDrop::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    if (dragObject_.Null() && dragSourceWidget_.Null())
        return;

    if (dragObject_.Null())
    {
        dragObject_ = dragSourceWidget_->GetDragObject();

        if (dragObject_.Null())
        {
            dragSourceWidget_ = 0;
            return;
        }

    }

    using namespace MouseMove;

    int x = eventData[P_X].GetInt();
    int y = eventData[P_Y].GetInt();

    // tolerance to 8 pixels to start drag/drop operation
    IntVector2 mousePos(x, y);
    mousePos -= mouseDownPosition_;
    if (Abs(mousePos.x_) < 8 && Abs(mousePos.y_) < 8)
        return;

    // initialize if necessary
    if (dragLayout_->GetVisibility() == UI_WIDGET_VISIBILITY_GONE)
    {
        dragLayout_->GetInternalWidget()->SetZ(WIDGET_Z_TOP);
        dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_VISIBLE);
        dragText_->SetText(dragObject_->GetText());

        tbUIPreferredSize* sz = dragLayout_->GetPreferredSize();
        dragLayout_->SetRect(IntRect(0, 0, sz->GetMinWidth(), sz->GetMinHeight()));
    }

    // see if we have a widget
    TBWidget* tbw = TBWidget::hovered_widget;

    while(tbw && (!tbw->GetDelegate() || tbw->IsOfType<TBLayout>()))
    {
        tbw = tbw->GetParent();
    }

    if (!tbw || !tbw->GetParent())
        return;

    tbUIWidget* hoverWidget = (tbUIWidget*) tbw->GetDelegate();

    if (!hoverWidget->GetInternalWidget())
        return;

    if (hoverWidget != currentTargetWidget_)
    {
        if (currentTargetWidget_)
        {
            VariantMap exitData;
            exitData[DragExitWidget::P_WIDGET] = currentTargetWidget_;
            exitData[DragExitWidget::P_DRAGOBJECT] = dragObject_;
            currentTargetWidget_->SendEvent(E_DRAGEXITWIDGET, exitData);
        }

        currentTargetWidget_ = hoverWidget;

        VariantMap enterData;
        enterData[DragEnterWidget::P_WIDGET] = currentTargetWidget_;
        enterData[DragEnterWidget::P_DRAGOBJECT] = dragObject_;
        currentTargetWidget_->SendEvent(E_DRAGENTERWIDGET, enterData);

    }

    dragLayout_->SetPosition(x, y - 20);

}

void tbUIDragDrop::FileDragEntered()
{
    dragObject_ = new tbUIDragObject(context_);
}

void tbUIDragDrop::FileDragAddFile(const String& filename)
{
    dragObject_->AddFilename(filename);
}

void tbUIDragDrop::FileDragConclude()
{
    DragEnd();
}


}
