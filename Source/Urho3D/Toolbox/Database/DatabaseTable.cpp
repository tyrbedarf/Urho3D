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

#include "DatabaseTable.h"

namespace Urho3D
{
	DatabaseTable::DatabaseTable(Context* context, const TypeInfo* type) :
		Object(context),
		primaryKey_(nullptr)
	{
		if (type == nullptr)
		{
			URHO3D_LOGERROR("TypeInfo cannot be null while trying to automate database serialization.");
			return;
		}

		auto attributes = context_->GetAttributes(type->GetTypeName());
		if (attributes == nullptr || attributes->Size() < 1)
		{
			URHO3D_LOGWARNING( type->GetTypeName() + " has no attributes. Writing to database will not work for this type.");
			return;
		}

		for (int i = 0; i < attributes->Size(); i++)
		{
			auto attribute = attributes->At(i);
			auto column = SharedPtr<DatabaseColumn>(new DatabaseColumn(context_, attribute));
			if (column->IsPrimaryKey())
			{
				primaryKey_ = column;
			}

			columms_.Insert(Pair<StringHash, SharedPtr<DatabaseColumn>>(column->GetColumnName(), column));
		}

		tableName_ = InitializeTableName(type);
	}

	// TODO: Find a way to alow users to use an attribute to set the table name.
	String DatabaseTable::InitializeTableName(const TypeInfo* type)
	{
		String result = type->GetTypeName();

		return result;
	}

	DatabaseTable::~DatabaseTable()
	{

	}
}
#endif