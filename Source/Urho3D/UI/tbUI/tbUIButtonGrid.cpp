//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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
#include <TurboBadger/tb_layout.h>

#include "tbUI.h"
#include "tbUIEvents.h"
#include "tbUIWidget.h"
#include "tbUILayout.h"
#include "tbUIButtonGrid.h"

using namespace tb;

namespace Urho3D
{

tbUIButtonGrid::tbUIButtonGrid(Context* context, int numRows, int numCols, int margin, bool createWidget )
    : tbUIWidget(context, false),
    rows_(numRows),
    columns_(numCols),
    margin_(margin),
    rowHeight_(0),
    columnWidth_(0),
    addCount_(0)
{
    if (createWidget)
    {
        widget_ = new TBLayout(AXIS_Y); // build upon a stock TBLayout
        widget_->SetDelegate(this);
        widget_->SetGravity(WIDGET_GRAVITY_ALL);
        GetSubsystem<tbUI>()->WrapWidget(this, widget_);
        GenerateGrid();
    }
}

tbUIButtonGrid::~tbUIButtonGrid()
{
}

void tbUIButtonGrid::SetGridText (int row, int column, String str)
{
    if (!widget_)
        return;

    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row
    if (lo0)
    {
        TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(column);  // find column
        if (b0)
        {
           b0->SetText( str.CString() );
        }
    }
}

/// add strings starting at 0,0 and filling out columns, then next row, returns count
int tbUIButtonGrid::AddGridText ( String str )
{
    int row = 0;
    int column = 0;
    if ( addCount_ > ( rows_ * columns_) ) return addCount_; // dont write past the end of the grid
    row = addCount_ / columns_;
    column = addCount_ % columns_;
    SetGridText ( row, column, str );
    addCount_++;
    return addCount_-1;
}

/// returns number of rows that were programmed
int tbUIButtonGrid::GetNumRows() const
{
    return rows_;
}

/// returns number of columns that were programmed
int tbUIButtonGrid::GetNumColumns() const
{
    return columns_;
}

/// returns current row height
int tbUIButtonGrid::GetRowHeight() const
{
    return rowHeight_;
}

/// returns current column width
int tbUIButtonGrid::GetColumnWidth() const
{
    return columnWidth_;
}

/// returns current margin value
int tbUIButtonGrid::GetMargin() const
{
    return margin_;
}

String tbUIButtonGrid::GetGridId( int row, int column )
{
    return String( 'A' + row ) + String(column); // generate spreadsheet style id
}

String tbUIButtonGrid::GetGridText(int row, int column)
{
    if (!widget_)
        return "";

    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row
    if (lo0)
    {
        TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(column);  // find column
        if (b0)
        {
            TBStr foo;
            if ( b0->GetText( foo ) )
                return foo.CStr();
        }
    }
    return "";
}

/// returns text at count
String tbUIButtonGrid::AtGridText( int count )
{
    int row = 0;
    int column = 0;
    row = count / columns_;
    column = count % columns_;
    return GetGridText( row, column );
}

tbUIWidget* tbUIButtonGrid::GetGridWidget(int row, int column)
{
    if (!widget_)
        return NULL;

    TBWidget *mywidget = NULL;
    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row); // find row
    if (lo0)
    {
        mywidget = lo0->GetChildFromIndex(column);  // find column
    }
    if ( mywidget )
    {
        tbUI* ui = GetSubsystem<tbUI>();
        return ui->WrapWidget(mywidget);
    }
   return NULL;
}

/// returns widget at count
tbUIWidget* tbUIButtonGrid::AtGridWidget( int count )
{
    int row = 0;
    int column = 0;
    row = count / columns_;
    column = count % columns_;
    return GetGridWidget ( row, column );
}

void tbUIButtonGrid::GenerateGrid()
{
    if ( widget_ && widget_->numChildren() == 0 )  // build once.
    {
        ((TBLayout *)widget_)->SetSpacing(margin_);
        for (int nn=0; nn<rows_; nn++ )
            AddGridRow( nn );
    }
}

void tbUIButtonGrid::AddGridRow( int rownum )
{
    TBLayout *lo0 = new TBLayout(); // make a new layout
    lo0->SetID( TBID (rownum) );
    lo0->SetLayoutConfig ( "XACAC" ); // do config + (spacing) margin
    lo0->SetSpacing(margin_);
    int cc = 0;
    for ( cc=0; cc<columns_; cc++) // stuff new button ( with preferred size, new id ) in.
    {
        LayoutParams *lp0 = new LayoutParams();
        lp0->SetWidth( columnWidth_ <= 0 ? 1 : columnWidth_);
        lp0->SetHeight( rowHeight_ <= 0 ? 1 : rowHeight_);
        TBButton *b0 = new TBButton();
        b0->SetLayoutParams(*lp0);
        b0->SetSqueezable(true);
        TBStr myid;
        myid.SetFormatted ( "%c%d", 'A' + rownum, cc+1 );
        b0->SetID( TBID (myid) );
        lo0->AddChild ( b0 );
    }
    widget_->AddChild ( lo0 );
}

/// will disable buttons that havent set any text, and enable those with text.
void tbUIButtonGrid::DisableEmptyButtons()
{
    int row = 0;
    int column = 0;
    for ( row=0; row<rows_; row++ )
    {
        TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row layout
        if (lo0)
        {
            for ( column=0; column<columns_; column++ )
            {
                TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(column);  // find column button
                if (b0)
                {
                    TBStr foo;
                    if ( b0->GetText( foo ) )
                    {
                        b0->SetState(WIDGET_STATE_DISABLED, foo.IsEmpty() );
                    }
                }
            }
        }
    }
}


void tbUIButtonGrid::ResizeGrid()
{
    if ( rows_ == 0 || columns_ == 0 ) return; // dont do bad maths.

    TBRect myrect = widget_->GetRect();
    rowHeight_ = (int)(  (myrect.h - (margin_ * rows_ )) / rows_ );
    columnWidth_ = (int)( (myrect.w -( margin_ * columns_ )) / columns_ );

    if ( rowHeight_ <= 1) rowHeight_ = 1;
    if ( columnWidth_ <= 1)  columnWidth_ = 1;

    int row = 0;
    int column = 0;
    for ( row=0; row<rows_; row++ )
    {
        TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row layout
        if (lo0)
        {
            for ( column=0; column<columns_; column++ )
            {
                TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(column);  // find column button
                if (b0)
                {
                    LayoutParams *lp1 = new LayoutParams(); // replace with new calced values
                    lp1->SetWidth(columnWidth_);
                    lp1->SetHeight(rowHeight_);
                    b0->SetLayoutParams(*lp1);
                }
            }
        }
    }
}

void tbUIButtonGrid::OnResized(int old_w, int old_h)
{
    ResizeGrid();
}

bool tbUIButtonGrid::OnEvent(const tb::TBWidgetEvent &ev)
{
    return tbUIWidget::OnEvent(ev);
}

}
