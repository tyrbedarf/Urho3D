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
#ifdef URHO3D_DATABASE

#pragma once

#include "../../Scene/Serializable.h"
#include "../../Core/Object.h"

#include "DatabaseTable.h"
#include "SqliteSerializer.h"

#include "../../UI/tbUI/tbUIWidget.h"
#include "../../UI/tbUI/tbUITextField.h"
#include "../../UI/tbUI/tbUILayout.h"
#include "../../UI/tbUI/tbUIEditField.h"

namespace Urho3D
{
	class URHO3D_API DatatableViewWidget : public tbUIWidget
	{
		URHO3D_OBJECT(DatatableViewWidget, tbUIWidget);

	protected:
		SharedPtr<DatabaseTable> table_;
		SharedPtr<tbUILayout> widget_root_;

		Vector<SharedPtr<Serializable>> data_;

		void GenerateColumnHeader();

		int GetColumnWidth(const DatabaseColumn* column);

	public:
		DatatableViewWidget(Context* context, const TypeInfo* type);
		~DatatableViewWidget();

		const Vector<SharedPtr<Serializable>>& GetData() const { return data_; }

		template<class T>
		void SetData(Vector<SharedPtr<T>>& elements)
		{
			data_.Clear();

			int min_width = 0;
			auto columns = table_->GetColumns();
			for (auto it : elements)
			{
				SharedPtr<Serializable> item = it;
				auto layout = SharedPtr<tbUILayout>(new tbUILayout(context_));
				data_.Push(SharedPtr<Serializable>(item));

				for (auto col : columns)
				{
					auto edit = SharedPtr<tbUIEditField>(new tbUIEditField(context_));

					switch (col.second_->GetDatabaseType())
					{
					case VAR_STRING: edit->SetText(col.second_->Get(item).GetString()); break;
					case VAR_INT: edit->SetText(String(col.second_->Get(item).GetInt())); break;
					case VAR_FLOAT: edit->SetText(String(col.second_->Get(item).GetFloat())); break;
					case VAR_DOUBLE: edit->SetText(String(col.second_->Get(item).GetDouble())); break;
					case VAR_BOOL: edit->SetText(String(col.second_->Get(item).GetBool())); break;

					default:
						break;
					}

					edit->SetSerializable(item, col.second_->GetAttributeInfo().name_);

					int width = GetColumnWidth(col.second_);
					if (width < 1)
					{
						width = 100;
					}

					min_width += width;

					edit->SetSize(width, 30);
					layout->AddChild(edit);
				}

				widget_root_->AddChild(layout);
			}

			widget_root_->SetLayoutMinWidth(min_width);
		}
	};
}

#endif