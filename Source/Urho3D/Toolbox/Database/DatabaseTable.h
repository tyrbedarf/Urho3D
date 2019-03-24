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
#include "../../Core/Context.h"
#include "../../Container/Vector.h"
#include "../../IO/Log.h"
#include "../../Database/DbResult.h"

#include "DatabaseColumn.h"

namespace Urho3D
{
	class URHO3D_API DatabaseTable : public Object
	{
		URHO3D_OBJECT(DatabaseTable, Object);

	public:
		DatabaseTable(Context* context, const TypeInfo* type);
		~DatabaseTable();

		const String& GetTableName() const { return tableName_; }
		const HashMap<StringHash, SharedPtr<DatabaseColumn>>& GetColumns() const { return columms_; }
		const DatabaseColumn* GetPrimaryKey() const
		{
			return primaryKey_;
		}

		template<class T>
		void Select(const DbResult& result, T* data, int row)
		{
			for (unsigned int i = 0; i < result.GetColumns().Size(); i++)
			{
				SharedPtr<DatabaseColumn> column;
				if (!columms_.TryGetValue(result.GetColumns().At(i), column))
				{
					URHO3D_LOGWARNING(GetTableName() + " has no column named " + result.GetColumns().At(i));
					continue;
				}

				column->Set(data, result.GetRows().At(row).At(i));
			}
		}

	private:
		String InitializeTableName(const TypeInfo* type);
		String tableName_;

		HashMap<StringHash, SharedPtr<DatabaseColumn>> columms_;
		SharedPtr<DatabaseColumn> primaryKey_;
	};
}
#endif