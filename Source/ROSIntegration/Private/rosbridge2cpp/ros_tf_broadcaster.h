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

#pragma once

#include "rapidjson/document.h"
#include <bson.h>

#include "ros_bridge.h"
#include "ros_topic.h"
#include "helper.h"

using json = rapidjson::Document;

namespace rosbridge2cpp {
	class ROSTFBroadcaster {
	public:
		ROSTFBroadcaster(ROSBridge &ros) : ros_(ros) {};

		// Send a single transform to /tf in JSON mode
		void SendTransform(json &geometry_msgs_transformstamped_msg);
		// Send Transform in BSON mode
		void SendTransform(bson_t &bson);

		// Accepts an json document (where .IsArray() is true) that contains
		// an array of geometry_msgs_transformstamped messages.
		// Only to be used in json mode
		void SendTransforms(json &geometry_msgs_transformstamped_array_msg);

		~ROSTFBroadcaster() = default;

	private:
		ROSBridge &ros_;
		ROSTopic tf_topic_{ ros_,"/tf","tf/tfMessage" };
	};
}
