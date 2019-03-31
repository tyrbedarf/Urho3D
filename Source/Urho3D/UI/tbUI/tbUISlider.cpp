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

#include "tbUIEvents.h"
#include "tbUI.h"
#include "tbUILayout.h"
#include "tbUISlider.h"

using namespace tb;

namespace Urho3D
{

tbUISlider::tbUISlider(Context* context, bool createWidget) : tbUIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSlider();
		widget_->SetDelegate(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

void tbUISlider::SetLimits(double minimum, double maximum)
{
    if (!widget_)
        return;
    ((TBSlider*) widget_)->SetLimits(minimum, maximum);

}

double tbUISlider::GetMinValue() const
{
    if (!widget_)
        return 0.0;

   return ((tbUISlider*) widget_)->GetMinValue();

}

double tbUISlider::GetMaxValue() const
{
    if (!widget_)
        return 0.0;

   return ((tbUISlider*) widget_)->GetMaxValue();

}

bool tbUISlider::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CUSTOM && ev.ref_id == TBIDC("edit_complete"))
    {
        VariantMap eventData;
        eventData[UIWidgetEditComplete::P_WIDGET] = this;
        SendEvent(E_UIWIDGETEDITCOMPLETE, eventData);

        return true;
    }
    return tbUIWidget::OnEvent(ev);
}

}
