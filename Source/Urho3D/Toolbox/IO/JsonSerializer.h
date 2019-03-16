//
// Copyright (c) 2008-2019 the Urho3D project.
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

#pragma once

#include "../../IO/File.h"
#include "../../Core/Object.h"
#include "../../Core/Context.h"
#include "../../Resource/JSONFile.h"
#include "../../Resource/JSONValue.h"
#include "../../Container/Str.h"
#include "../../Scene/Serializable.h"
#include "../../IO/Log.h"

namespace Urho3D
{
	class URHO3D_API JsonSerializer : public Object
	{
		URHO3D_OBJECT(JsonSerializer, Object);

	public:
		JsonSerializer(Context* context);

		bool Save(Serializable* o, const String& absolute_path, bool overwrite = true);

		template<class T>
		SharedPtr<T> Load(const String& absolute_path)
		{
			if (absolute_path.Length() < 1)
			{
				URHO3D_LOGERROR("Could not deserialize object. The file path was empty.");
				return nullptr;
			}

			SharedPtr<JSONFile> json(new JSONFile(context_));
			File f(context_, absolute_path, FILE_READ);
			if (!json->Load(f))
			{
				URHO3D_LOGERROR("Could not deserialize object.");
				return nullptr;
			}

			auto result = context_->CreateObject(T::GetTypeNameStatic());
			if (!result)
			{
				URHO3D_LOGERROR("Could not create object of type " + T::GetTypeNameStatic() + ". Did you forget to register a factory with the context?");
				return nullptr;
			}

			if (!result->GetTypeInfo()->IsTypeOf<Serializable>())
			{
				URHO3D_LOGERROR("In order to use automatic deserialization every object must inherit from Serializable.");
				return nullptr;
			}

			auto deserialized = static_cast<T*>(result.Get());
			if (!deserialized->LoadJSON(json->GetRoot()))
			{
				URHO3D_LOGERROR("Could not read json file.");
				return nullptr;
			}

			return SharedPtr<T>(deserialized);
		}
	};
}
