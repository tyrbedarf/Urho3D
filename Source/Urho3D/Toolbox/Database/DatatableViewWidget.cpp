//
// Copyright (c) 2019-2019, the Urho3D project.
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
#include "DatatableViewWidget.h"
#include "DatabaseConstants.h"

namespace Urho3D
{
	DatatableViewWidget::DatatableViewWidget(Context* context, const TypeInfo* type) :
		tbUIWidget(context)
	{
		table_ = SharedPtr<DatabaseTable>(new DatabaseTable(context_, type));
		widget_root_ = SharedPtr<tbUILayout>(new tbUILayout(context_));
		widget_root_->SetAxis(UI_AXIS_Y);

		AddChild(widget_root_);

		GenerateColumnHeader();

	}

	DatatableViewWidget::~DatatableViewWidget()
	{

	}

	void DatatableViewWidget::GenerateColumnHeader()
	{
		if (table_ == nullptr)
		{
			URHO3D_LOGERROR("Table has not been set up.");
			return;
		}

		auto columns = table_->GetColumns();
		auto table_name = table_->GetTableName();
		auto layout = SharedPtr<tbUILayout>(new tbUILayout(context_));
		layout->SetAxis(UI_AXIS_X);
		int layout_width = 0;
		for (auto it : columns)
		{
			auto header = SharedPtr<tbUITextField>(new tbUITextField(context_));
			header->SetText(it.second_->GetColumnName());
			header->SetTextAlign(UI_TEXT_ALIGN_CENTER);

			int width = GetColumnWidth(it.second_);
			if (width < 1)
			{
				width = 100;
			}

			layout_width += width;

			header->SetSize(width, 30);
			header->SetLayoutMinWidth(width);
			header->SetId(table_name + "_" + it.second_->GetColumnName() + "_header");
			layout->AddChild(header);
		}

		layout->SetSize(layout_width, 30);
		layout->SetLayoutMinWidth(layout_width);
		widget_root_->AddChild(layout);
		widget_root_->SetWidth(layout_width);
	}

	int DatatableViewWidget::GetColumnWidth(const DatabaseColumn* column)
	{
		auto attribs = column->GetAttributeInfo();
		if (!attribs.metadata_.Contains(DatabaseConstants::META_WIDGET_MIN_WIDTH))
		{
			return -1;
		}

		auto result = attribs.GetMetadata(DatabaseConstants::META_WIDGET_MIN_WIDTH);
		return result.GetInt();
	}

	SharedPtr<tbUIWidget> DatatableViewWidget::GetWidget(const DatabaseColumn* column, Serializable* item) const
	{
		SharedPtr<tbUIWidget> result = nullptr;

		switch (column->GetDatabaseType())
		{
		case VAR_STRING:
		{
			auto edit = SharedPtr<tbUIEditField>(new tbUIEditField(context_));
			edit->SetText(column->Get(item).GetString());
			edit->SetTextAlign(UI_TEXT_ALIGN_LEFT);
			result = edit;
			break;
		}
		case VAR_INT:
		{
			if (column->IsPrimaryKey())
			{
				auto edit = SharedPtr<tbUITextField>(new tbUITextField(context_));
				edit->SetText(String(column->Get(item).GetInt()));
				edit->SetTextAlign(UI_TEXT_ALIGN_RIGHT);
				result = edit;
			}
			else
			{
				auto edit = SharedPtr<tbUIEditField>(new tbUIEditField(context_));
				edit->SetText(String(column->Get(item).GetInt()));
				edit->SetTextAlign(UI_TEXT_ALIGN_RIGHT);
				edit->SetEditType(UI_EDIT_TYPE_NUMBER);
				result = edit;
			}

			break;
		}

		case VAR_FLOAT:
		{
			auto edit = SharedPtr<tbUIEditField>(new tbUIEditField(context_));
			edit->SetText(String(column->Get(item).GetFloat()));
			edit->SetTextAlign(UI_TEXT_ALIGN_RIGHT);
			edit->SetEditType(UI_EDIT_TYPE_NUMBER);
			result = edit;
			break;
		}

		case VAR_DOUBLE:
		{
			auto edit = SharedPtr<tbUIEditField>(new tbUIEditField(context_));
			edit->SetText(String(column->Get(item).GetDouble()));
			edit->SetTextAlign(UI_TEXT_ALIGN_RIGHT);
			edit->SetEditType(UI_EDIT_TYPE_NUMBER);
			result = edit;
			break;
		}

		case VAR_BOOL:
		{
			auto edit = SharedPtr<tbUICheckBox>(new tbUICheckBox(context_));
			edit->SetValue(column->Get(item).GetBool() ? 1 : 0);

			break;
		}

		default:
			URHO3D_LOGWARNING("Cannot handle variant type for column " + column->GetColumnName() + ".");
			auto edit = SharedPtr<tbUITextField>(new tbUITextField(context_));
			edit->SetText("Unsupported variant type");
			result = edit;
			break;
		}

		result->SetSerializable(item, column->GetAttributeInfo().name_);

		return result;
	}
}