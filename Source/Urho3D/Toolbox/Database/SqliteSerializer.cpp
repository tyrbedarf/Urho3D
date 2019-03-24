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

#include "SqliteSerializer.h"

namespace Urho3D
{
	String SqliteSerializer::GetColumnSql(const DatabaseColumn* column)
	{
		String type = "";
		switch (column->GetDatabaseType())
		{
		case VAR_BOOL:		type = "INTEGER"; break;
		case VAR_DOUBLE:	type = "REAL"; break;
		case VAR_FLOAT:		type = "REAL"; break;
		case VAR_INT:		type = "INTEGER"; break;
		case VAR_INT64:		type = "INTEGER"; break;
		case VAR_STRING:	type = "TEXT"; break;

		default:
			URHO3D_LOGERROR("Unknown data type."); return ""; break;
		}

		String constraints = "";
		if (column->IsNotNull()) { constraints += "NOT NULL "; }
		if (column->IsPrimaryKey()) { constraints = "PRIMARY KEY"; }

		return "'" + column->GetColumnName() + "' " + type + " " + constraints;
	}

	String SqliteSerializer::GetTableSql(const DatabaseTable* table)
	{
		String result = "CREATE TABLE IF NOT EXISTS " + table->GetTableName();
		String columnDescr = "";
		auto columns = table->GetColumns();
		for (unsigned int i = 0; i < columns.Size(); i++)
		{
			auto column = columns.At(i);
			if (i > 0)
			{
				columnDescr += ", \n";
			}

			columnDescr += GetColumnSql(column);
		}

		result += "(\n" + columnDescr + "\n);";

		return result;
	}
}

#endif