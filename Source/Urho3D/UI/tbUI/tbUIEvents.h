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

#include "../../Core/Object.h"

namespace Urho3D
{

	// UIUpdate event which is sent post engine update, ensuring all input events have been processed
	URHO3D_EVENT(E_UIUPDATE, UIUpdate)
	{

	}

	URHO3D_EVENT(E_WIDGETEVENT, WidgetEvent)
	{
		URHO3D_PARAM(P_HANDLER, Handler);           // UIWidget pointer of widget's OnEvent we are in
		URHO3D_PARAM(P_TARGET, Target);             // UIWidget pointer
		URHO3D_PARAM(P_TYPE, Type);                 // enum UI_EVENT_TYPE
		URHO3D_PARAM(P_X, X);                       // int
		URHO3D_PARAM(P_Y, Y);                       // int
		URHO3D_PARAM(P_DELTAX, DeltaX);             // int
		URHO3D_PARAM(P_DELTAY, DeltaY);             // int
		URHO3D_PARAM(P_COUNT, Count);               // int
		URHO3D_PARAM(P_KEY, Key);                   // int
		URHO3D_PARAM(P_SPECIALKEY, SpecialKey);     // enum SPECIAL_KEY
		URHO3D_PARAM(P_MODIFIERKEYS, ModifierKeys); // enum MODIFIER_KEYS
		URHO3D_PARAM(P_REFID, RefID);               // string (TBID)
		URHO3D_PARAM(P_TOUCH, Touch);               // bool

		// EventHandled can be set by event receivers to stop event bubble
		URHO3D_PARAM(P_HANDLED, Handled);               // [OUT] -> bool

	}

	URHO3D_EVENT(E_WIDGETLOADED, WidgetLoaded)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
	}

	URHO3D_EVENT(E_WIDGETFOCUSCHANGED, WidgetFocusChanged)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
		URHO3D_PARAM(P_FOCUSED, Focused);             // bool
	}


	URHO3D_EVENT(E_WIDGETDELETED, WidgetDeleted)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
	}

	URHO3D_EVENT(E_DRAGBEGIN, DragBegin)
	{
		URHO3D_PARAM(P_TARGET, Source);             // UIWidget source
		URHO3D_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
	}

	URHO3D_EVENT(E_DRAGENTERWIDGET, DragEnterWidget)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
		URHO3D_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
	}

	URHO3D_EVENT(E_DRAGEXITWIDGET, DragExitWidget)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
		URHO3D_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
	}

	URHO3D_EVENT(E_DRAGENDED, DragEnded)
	{
		URHO3D_PARAM(P_TARGET, Target);             // UIWidget pointer
		URHO3D_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
	}


	URHO3D_EVENT(E_POPUPMENUSELECT, PopupMenuSelect)
	{
		URHO3D_PARAM(P_BUTTON, Button);             // UIButton that created popup
		URHO3D_PARAM(P_REFID, RefID);             // string tbid
	}

	URHO3D_EVENT(E_UISHORTCUT, UIShortcut)
	{
		URHO3D_PARAM(P_KEY, Key);                    // int
		URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int

	}

	URHO3D_EVENT(E_WINDOWCLOSED, WindowClosed)
	{
		URHO3D_PARAM(P_WINDOW, Window);               // UIWindow
	}

	URHO3D_EVENT(E_UIWIDGETFOCUSCHANGED, UIWidgetFocusChanged)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
		URHO3D_PARAM(P_FOCUSED, Focused);             // bool
	}
	URHO3D_EVENT(E_UIWIDGETFOCUSESCAPED, UIWidgetFocusEscaped)
	{
	}

	URHO3D_EVENT(E_UIWIDGETEDITCANCELED, UIWidgetEditCanceled)
	{
		URHO3D_PARAM(P_WIDGET, Widget);         // UIWidget pointer
	}

	URHO3D_EVENT(E_UIWIDGETEDITCOMPLETE, UIWidgetEditComplete)
	{
		URHO3D_PARAM(P_WIDGET, Widget);             // UIWidget pointer
	}

	URHO3D_EVENT(E_UIUNHANDLEDSHORTCUT, UIUnhandledShortcut)
	{
		URHO3D_PARAM(P_REFID, RefID); // string tbid
	}

	URHO3D_EVENT(E_UILISTVIEWSELECTIONCHANGED, UIListViewSelectionChanged)
	{
		URHO3D_PARAM(P_REFID, RefID); // string tbid
		URHO3D_PARAM(P_SELECTED, Selected);        // bool
	}

	/// event for PromptWindow
	URHO3D_EVENT(E_UIPROMPTCOMPLETE, UIPromptComplete)
	{
		URHO3D_PARAM(P_TITLE, Title);  // string
		URHO3D_PARAM(P_REASON, Reason);  // string
		URHO3D_PARAM(P_SELECTED, Selected);  // string
	}

	/// event for FinderWindow
	URHO3D_EVENT(E_UIFINDERCOMPLETE, UIFinderComplete)
	{
		URHO3D_PARAM(P_TITLE, Title);  // string
		URHO3D_PARAM(P_REASON, Reason);  // string
		URHO3D_PARAM(P_SELECTED, Selected);  // string
	}

}
