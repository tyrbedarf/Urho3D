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
#include "../../IO/Log.h"

namespace Urho3D
{
	class URHO3D_API DatabaseColumn : public Object
	{
		URHO3D_OBJECT(DatabaseColumn, Object);

	public:
		DatabaseColumn(Context* context, AttributeInfo& info);

		VariantType GetDatabaseType() const		{ return attribute_.type_; }
		const String& GetColumnName() const		{ return columnName_; }
		bool IsPrimaryKey() const				{ return primaryKey; }
		bool IsNotNull() const					{ return notNull; }

	private:
		String InitializeColumnName();
		bool InitializePrimaryKey();
		bool InitializeNotNull();

		AttributeInfo attribute_;
		String columnName_;

		bool primaryKey;
		bool notNull;
	};
}
#endif