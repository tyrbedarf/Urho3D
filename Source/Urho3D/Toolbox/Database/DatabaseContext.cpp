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

#include "DatabaseContext.h"

namespace Urho3D
{
	DatabaseContext::DatabaseContext(Context* context, String db_file) :
		Object(context),
		connection_(nullptr)
	{
		// TODO: Currently only SQLite is supported.
		serializer_ = SharedPtr<AbstractDatabaseSerializer>(new SqliteSerializer(context_));
	}

	DatabaseContext::~DatabaseContext()
	{
		Close();
	}

	void DatabaseContext::AddTable(const TypeInfo* t)
	{
		auto table = SharedPtr<DatabaseTable>(new DatabaseTable(context_, t));
		tables_.Push(table);
	}

	bool DatabaseContext::Open()
	{
		if (db_file.Empty())
		{
			URHO3D_LOGERROR("The connection string cannot be empty in order to open a database connection.");
			return false;
		}

		if (connection_)
		{
			URHO3D_LOGWARNING("A database connection has already been established.");
			return true;
		}

		connection_ = GetSubsystem<Database>()->Connect(db_file);
		return connection_ != nullptr;
	}
	void DatabaseContext::Close()
	{
		if (!connection_)
		{
			URHO3D_LOGWARNING("A database connection has not been established.");
			return;
		}

		GetSubsystem<Database>()->Disconnect(connection_);
		connection_ = nullptr;
	}

	void DatabaseContext::CreateDatabase()
	{
		Open();

		URHO3D_LOGDEBUG("Creating database:");
		for (unsigned int i = 0; i < tables_.Size(); i++)
		{
			auto table = tables_.At(i);
			String sql = serializer_->GetTableSql(table);

			URHO3D_LOGDEBUG(sql);
		}

		Close();
	}
}

#endif