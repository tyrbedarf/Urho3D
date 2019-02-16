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
#include <TurboBadger/tb_atomic_widgets.h>

#include "../../IO/Log.h"
#include "../../Graphics/Texture.h"

#include "tbUIEvents.h"
#include "tbUI.h"
#include "tbUIColorWidget.h"

using namespace tb;

namespace Urho3D
{

tbUIColorWidget::tbUIColorWidget(Context* context, bool createWidget) : tbUIWidget(context, false)
{
    tbUI* ui = GetSubsystem<tbUI>();

    if (createWidget)
    {
        widget_ = new TBColorWidget();
        widget_->SetDelegate(this);
        ui->WrapWidget(this, widget_);
    }

}

tbUIColorWidget::~tbUIColorWidget()
{
}

bool tbUIColorWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    return tbUIWidget::OnEvent(ev);
}

void tbUIColorWidget::SetColorString( const String &name )
{
     if (!widget_)
        return;
    ((TBColorWidget*) widget_)->SetColor(name.CString());
}

void tbUIColorWidget::SetColor(const Color& color)
{
    if (!widget_)
        return;

    ((TBColorWidget*)widget_)->SetColor(color.r_ * 255.0f, color.g_ * 255.0f, color.b_ * 255.0f, color.a_ * 255.0f);
}

Color tbUIColorWidget::GetColor() const
{
    if (!widget_)
        return Color::BLACK;

    const TBColor& color = ((TBColorWidget*)widget_)->GetColor();
    float alpha = ((TBColorWidget*)widget_)->GetAlpha();

    return Color(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, alpha / 255.0f);

}

void tbUIColorWidget::SetAlpha ( float value )
{
     if (!widget_)
        return;
    ((TBColorWidget*) widget_)->SetAlpha(value);
}

}
