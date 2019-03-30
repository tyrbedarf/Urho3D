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
		bool IsUnique() const					{ return unique; }

		Variant Get(const Serializable* data) const
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to get a value.");
				return Variant(0);
			}

			Variant result;
			attribute_.accessor_->Get(data, result);

			return result;
		}

		void Set(Serializable* data, int value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			if (attribute_.type_ != VAR_INT)
			{
				URHO3D_LOGERROR("Attribute has the wrong data type.");
				return;
			}

			Variant var(value);
			attribute_.accessor_->Set(data, var);
		}

		void Set(Serializable* data, float value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			if (attribute_.type_ != VAR_FLOAT)
			{
				URHO3D_LOGERROR("Attribute has the wrong data type.");
				return;
			}

			Variant var(value);
			attribute_.accessor_->Set(data, var);
		}

		void Set(Serializable* data, double value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			if (attribute_.type_ != VAR_DOUBLE)
			{
				URHO3D_LOGERROR("Attribute has the wrong data type.");
				return;
			}

			Variant var(value);
			attribute_.accessor_->Set(data, var);
		}

		void Set(Serializable* data, String value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			if (attribute_.type_ != VAR_STRING)
			{
				URHO3D_LOGERROR("Attribute has the wrong data type.");
				return;
			}

			Variant var(value);
			attribute_.accessor_->Set(data, var);
		}

		void Set(Serializable* data, bool value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			if (attribute_.type_ != VAR_BOOL)
			{
				URHO3D_LOGERROR("Attribute has the wrong data type.");
				return;
			}

			Variant var(value);
			attribute_.accessor_->Set(data, var);
		}

		void Set(Serializable* data, const Variant& value)
		{
			if (!data)
			{
				URHO3D_LOGERROR("Serializable cannot be null, in order to set a value.");
				return;
			}

			switch (attribute_.type_)
			{
			case VAR_BOOL: Set(data, value.GetBool()); break;
			case VAR_FLOAT: Set(data, value.GetFloat()); break;
			case VAR_DOUBLE: Set(data, value.GetDouble()); break;
			case VAR_INT: Set(data, value.GetInt()); break;
			case VAR_STRING: Set(data, value.GetString()); break;

			default:
				URHO3D_LOGERROR("Unhandled variant type.");
				break;
			}
		}

		/// Return the attribute info found while constructing this type.
		const AttributeInfo& GetAttributeInfo() const { return attribute_; }

	private:
		String InitializeColumnName();
		bool InitializePrimaryKey();
		bool InitializeNotNull();
		bool InitializeUnique();

		AttributeInfo attribute_;
		String columnName_;

		bool primaryKey;
		bool notNull;
		bool unique;
	};
}
#endif