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

#include "JsonSerializer.h"
#include "../../IO/Log.h"
#include "../../IO/FileSystem.h"

namespace Urho3D
{
	JsonSerializer::JsonSerializer(Context* context) : Object(context)
	{

	}

	bool JsonSerializer::Save(Serializable* o, const String& absolute_path, bool overwrite)
	{
		if (!o)
		{
			URHO3D_LOGERROR("Could not serialize object. The reference was null.");
			return false;
		}

		if (absolute_path.Length() < 1)
		{
			URHO3D_LOGERROR("Could not serialize object. The file path was empty.");
			return false;
		}

		if (!overwrite && GetSubsystem<FileSystem>()->FileExists(absolute_path))
		{
			URHO3D_LOGERROR("Could not serialize object. The '" + absolute_path + "' already exists and should not be overwritten.");
			return false;
		}

		SharedPtr<JSONFile> json(new JSONFile(context_));

		JSONValue& rootElem = json->GetRoot();
		if (!o->SaveJSON(rootElem))
		{
			URHO3D_LOGERROR("Could not serialize object.");
			return false;
		}

		File f(context_, absolute_path, FILE_WRITE);
		return json->Save(f, "\t");
	}
}