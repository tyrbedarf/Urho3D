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

#pragma once

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_select_item.h>

#include "../../Core/Object.h"
#include "../../Container/List.h"

namespace Urho3D
{

class tbUISelectItemSource;

class URHO3D_API tbUISelectItem : public Object
{
    URHO3D_OBJECT(tbUISelectItem, Object)

public:

    tbUISelectItem(Context* context, const String& str = String::EMPTY, const String& id = String::EMPTY, const String& skinImage = String::EMPTY);
    virtual ~tbUISelectItem();

    void SetString(const String& str) { str_ = str; }
    void SetID(const String& id);
    const String& GetStr() { return str_; }
    tb::TBID GetID() { return id_; }
    void SetSkinImage(const String& skinImage);
    void SetSubSource(tbUISelectItemSource *subSource);

    virtual tb::TBGenericStringItem* GetTBItem();

protected:

    String str_;

    // TBID
    tb::TBID id_;
    // TBID
    tb::TBID skinImage_;

    SharedPtr<tbUISelectItemSource> subSource_;

};

class URHO3D_API tbUISelectItemSource : public Object
{
    URHO3D_OBJECT(tbUISelectItemSource, Object)

public:

    tbUISelectItemSource(Context* context);
    virtual ~tbUISelectItemSource();

    void AddItem(tbUISelectItem* item) { items_.Push(SharedPtr<tbUISelectItem>(item)); }
    void RemoveItemWithId(const String& id);
    void RemoveItemWithStr(const String& str);
    int GetItemCount() { return items_.Size(); }

    void Clear() { items_.Clear(); }

    /// Returns item string for the index. Returns empty string for invalid indexes.
    const String& GetItemStr(int index);

    // caller's responsibility to clean up
    virtual tb::TBSelectItemSource* GetTBItemSource();

protected:

    List<SharedPtr<tbUISelectItem>> items_;

};

}
