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
#include <TurboBadger/tb_editfield.h>

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUISkinImage.h"

using namespace tb;

namespace Urho3D
{

tbUISkinImage::tbUISkinImage(Context* context, const String& bitmapID, bool createWidget) : tbUIWidget(context, false)
{
    if (createWidget)
    {
        if (bitmapID.Length())
            widget_ = new TBSkinImage(TBIDC(bitmapID.CString()));
        else
            widget_ = new TBSkinImage();

		widget_->AddListener(this);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
    }
}

bool tbUISkinImage::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

}
