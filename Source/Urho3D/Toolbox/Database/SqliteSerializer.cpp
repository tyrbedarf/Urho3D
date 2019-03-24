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
		if (column->IsNotNull())
		{
			constraints += "NOT NULL";
		}

		if (column->IsUnique())
		{
			constraints += constraints.Length() > 0 ? " " : "";
			constraints += "UNIQUE";
		}

		if (column->IsPrimaryKey())
		{
			constraints = "PRIMARY KEY";
		}

		auto r = "'" + column->GetColumnName() + "' " + type;
		if (constraints.Length() > 0)
		{
			r += " " + constraints;
		}

		return r;
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

			columnDescr += "\t" + GetColumnSql(column);
		}

		result += "(\n" + columnDescr + "\n);";

		return result;
	}

	String SqliteSerializer::GetUpdateSql(
		const DatabaseTable* table,
		const Serializable* data)
	{
		auto pk = table->GetPrimaryKey();
		String stmt = "UPDATE '" + table->GetTableName() + "' SET \n";
		String whereClause = " WHERE ";
		if (pk)
		{
			auto id = pk->Get(data).GetInt();
			whereClause += pk->GetColumnName() + " = " + String(id) + ";";
		}

		auto columns = table->GetColumns();
		String values = "";
		for (auto it = columns.Begin(); it != columns.End(); it++)
		{
			auto column = *(it);
			if (column->IsPrimaryKey())
			{
				continue;
			}

			if (values.Length() > 0)
			{
				values += ", \n";
			}

			switch (column->GetDatabaseType())
			{
			case VAR_INT: values += "\t'" + column->GetColumnName() + "' = " + String(column->Get(data).GetInt()); break;
			case VAR_FLOAT: values += "\t'" + column->GetColumnName() + "' = " + String(column->Get(data).GetFloat()); break;
			case VAR_DOUBLE: values += "\t'" + column->GetColumnName() + "' = " + String(column->Get(data).GetDouble()); break;
			case VAR_BOOL: values += "\t'" + column->GetColumnName() + "' = " + String(column->Get(data).GetBool()); break;
			case VAR_STRING: values += "\t'" + column->GetColumnName() + "' = '" + column->Get(data).GetString() + "'"; break;
			default:
				break;
			}
		}

		return stmt + "(" + values + "\n) " + whereClause;
	}

	String SqliteSerializer::GetInsertSql(
		const DatabaseTable* table,
		const Serializable* data)
	{
		auto pk = table->GetPrimaryKey();
		String stmt = "INSERT INTO '" + table->GetTableName() + "'";
		String columnNames = "";
		String values = "";

		auto columns = table->GetColumns();
		for (auto it = columns.Begin(); it != columns.End(); it++)
		{
			auto column = *(it);
			if (column->IsPrimaryKey())
			{
				continue;
			}

			if (columnNames.Length() > 0)
			{
				columnNames += ", ";
				values += ", ";
			}

			columnNames += "'" + column->GetColumnName() + "'";

			switch (column->GetDatabaseType())
			{
			case VAR_INT: values += String(column->Get(data).GetInt()); break;
			case VAR_FLOAT: values += String(column->Get(data).GetFloat()); break;
			case VAR_DOUBLE: values += String(column->Get(data).GetDouble()); break;
			case VAR_BOOL: values += String(column->Get(data).GetBool()); break;
			case VAR_STRING: values += "'" + column->Get(data).GetString() + "'"; break;
			default:
				break;
			}
		}

		return stmt + "(" + columnNames + ") VALUES (" + values + ");";
	}
}
#endif