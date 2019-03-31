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
#include "../../Database/DbConnection.h"
#include "../../Database/Database.h"

#include "DatabaseTable.h"
#include "SqliteSerializer.h"

namespace Urho3D
{
	class URHO3D_API DatabaseContext : public Object
	{
		URHO3D_OBJECT(DatabaseContext, Object);

	public:
		DatabaseContext(Context* context, String db_file);
		~DatabaseContext();

		void AddTable(const TypeInfo* t);
		void CreateDatabase();

		bool Open();
		void Close();

		void Update(Serializable* item);

		template<class T>
		Vector<SharedPtr<T>> SelectQuery(const String& query)
		{
			Vector<SharedPtr<T>> result;
			auto type = T::GetTypeInfoStatic();
			auto table = GetTable(type);
			if (!table)
			{
				return result;
			}

			Open();
			auto dbResult = connection_->Execute(query, false);
			Close();

			if (dbResult.GetRows().Size() < 1)
			{
				return result;
			}

			for (int i = 0; i < dbResult.GetRows().Size(); i++)
			{
				auto item = context_->CreateObject(T::GetTypeNameStatic());
				if (!item)
				{
					URHO3D_LOGERROR("Could not create object of type " + T::GetTypeNameStatic() + ". Did you forget to register a factory with the context?");
					return result;
				}

				if (!item->GetTypeInfo()->IsTypeOf<Serializable>())
				{
					URHO3D_LOGERROR("In order to use Urho3D ORM every object must inherit from Serializable.");
					return result;
				}

				auto deserialized = static_cast<T*>(item.Get());
				table->Select(dbResult, deserialized, i);

				result.Push(SharedPtr<T>(deserialized));
			}

			return result;
		}

		template<class T>
		Vector<SharedPtr<T>> Select(String whereClause)
		{
			auto table = GetTable(T::GetTypeInfoStatic());
			if (!table)
			{
				return Vector<SharedPtr<T>>();
			}

			return SelectQuery<T>(serializer_->GetSelect(whereClause, table));
		}

	private:
		String connectionString;
		HashMap<StringHash, SharedPtr<DatabaseTable>> tables_;
		SharedPtr<AbstractDatabaseSerializer> serializer_;

		SharedPtr<DbConnection> connection_;

		DatabaseTable* GetTable(const TypeInfo* t);
	};
}

#endif