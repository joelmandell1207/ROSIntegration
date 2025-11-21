// MIT License

// Copyright (c) 2025 YKK.xTechLab.Engineering

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ros_tf_broadcaster.h"

namespace rosbridge2cpp {
	void ROSTFBroadcaster::SendTransform(json &geometry_msgs_transformstamped_msg)
	{
		assert(geometry_msgs_transformstamped_msg.IsObject());

		rapidjson::Document transform_array;
		transform_array.SetArray();
		transform_array.PushBack(geometry_msgs_transformstamped_msg, transform_array.GetAllocator());

		SendTransforms(transform_array);
	}

	void ROSTFBroadcaster::SendTransforms(json &geometry_msgs_transformstamped_array_msg)
	{
		assert(geometry_msgs_transformstamped_array_msg.IsArray());

		rapidjson::Document tf_message;
		tf_message.SetObject();

		tf_message.AddMember("transforms", geometry_msgs_transformstamped_array_msg, tf_message.GetAllocator());

		tf_topic_.Publish(tf_message);
	}

	void ROSTFBroadcaster::SendTransform(bson_t &bson)
	{
		tf_topic_.Publish(&bson);
	}
}
