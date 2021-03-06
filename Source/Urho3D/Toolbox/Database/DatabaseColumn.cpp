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

#include "DatabaseColumn.h"
#include "DatabaseConstants.h"

namespace Urho3D
{
	DatabaseColumn::DatabaseColumn(Context* context, AttributeInfo& info) :
		Object(context),
		attribute_(info)
	{
		columnName_ = InitializeColumnName();
		primaryKey = InitializePrimaryKey();
		notNull = InitializeNotNull();
		unique = InitializeUnique();
	}

	String DatabaseColumn::InitializeColumnName()
	{
		auto result = attribute_.name_;
		auto metaData = attribute_.GetMetadata(DatabaseConstants::META_COLUMN_NAME);
		if (!metaData.IsEmpty())
		{
			if (metaData.GetType() != VAR_STRING)
			{
				URHO3D_LOGWARNING(
					"Found column name metadata on " + attribute_.name_ +
					" but it was not of type VAR_STRING. The provided value will be ignored");
			}
			else
			{
				result = metaData.GetString();
			}
		}

		return result;
	}

	bool DatabaseColumn::InitializePrimaryKey()
	{
		auto result = false;
		auto metaData = attribute_.GetMetadata(DatabaseConstants::META_PRIMARY_KEY);
		if (!metaData.IsEmpty())
		{
			if (metaData.GetType() != VAR_BOOL)
			{
				URHO3D_LOGWARNING(
					"Found primnary key metadata on " + attribute_.name_ +
					" but it was not of type VAR_BOOL. The provided value will be ignored");
			}
			else
			{
				result = metaData.GetBool();
			}
		}

		return result;
	}

	bool DatabaseColumn::InitializeNotNull()
	{
		auto result = false;
		auto metaData = attribute_.GetMetadata(DatabaseConstants::META_NOT_NULL);
		if (!metaData.IsEmpty())
		{
			if (metaData.GetType() != VAR_BOOL)
			{
				URHO3D_LOGWARNING(
					"Found not null metadata on " + attribute_.name_ +
					" but it was not of type VAR_BOOL. The provided value will be ignored");
			}
			else
			{
				result = metaData.GetBool();
			}
		}

		return result;
	}

	bool DatabaseColumn::InitializeUnique()
	{
		auto result = false;
		auto metaData = attribute_.GetMetadata(DatabaseConstants::META_UNIQUE);
		if (!metaData.IsEmpty())
		{
			if (metaData.GetType() != VAR_BOOL)
			{
				URHO3D_LOGWARNING(
					"Found unique metadata on " + attribute_.name_ +
					" but it was not of type VAR_BOOL. The provided value will be ignored");
			}
			else
			{
				result = metaData.GetBool();
			}
		}

		return result;
	}
}
#endif