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

#include "ros_service.h"

namespace rosbridge2cpp{

	std::string ROSService::GenerateServiceCallID() {
		std::string service_call_id = "";
		service_call_id.append("call_service:");
		service_call_id.append(service_name_);
		service_call_id.append(":");
		service_call_id.append(std::to_string(++ros_.id_counter));
		return service_call_id;
	}

	bool ROSService::CallService(rapidjson::Value &request, FunVrROSServiceResponseMsg callback) {
		if (is_advertised_)	// You can't use an advertised ROSService instance to call services.
			return false;	// Use a separate instance

		std::string service_call_id = GenerateServiceCallID();

		// Register the callback with the given call id in the ROSBridge
		ros_.RegisterServiceCallback(service_call_id, callback);

		ROSBridgeCallServiceMsg cmd(true);
		cmd.id_ = service_call_id;
		cmd.service_ = service_name_;
		cmd.args_json_ = request;

		return ros_.SendMessage(cmd);
	}

	bool ROSService::CallService(bson_t *request, FunVrROSServiceResponseMsg callback) {
		if (is_advertised_)	// You can't use an advertised ROSService instance to call services.
			return false;	// Use a separate instance

		assert(request);

		std::string service_call_id = GenerateServiceCallID();

		// Register the callback with the given call id in the ROSBridge
		ros_.RegisterServiceCallback(service_call_id, callback);

		ROSBridgeCallServiceMsg cmd(true);
		cmd.id_ = service_call_id;
		cmd.service_ = service_name_;
		cmd.args_bson_ = request;

		return ros_.SendMessage(cmd);
	}

	bool ROSService::Advertise(FunVrROSCallServiceMsgrROSServiceResponseMsgrAllocator callback) {
		if (is_advertised_)
			return true;

		// Register on ROSBridge
		ros_.RegisterServiceRequestCallback(service_name_, callback);

		ROSBridgeAdvertiseServiceMsg cmd(true);
		cmd.service_ = service_name_;
		cmd.type_ = service_type_;

		is_advertised_ = ros_.SendMessage(cmd);
		return is_advertised_;
	}

	bool ROSService::Advertise(FunVrROSCallServiceMsgrROSServiceResponseMsg callback) {
		if (is_advertised_)
			return true;

		// Register on ROSBridge
		ros_.RegisterServiceRequestCallback(service_name_, callback);

		ROSBridgeAdvertiseServiceMsg cmd(true);
		cmd.service_ = service_name_;
		cmd.type_ = service_type_;

		is_advertised_ = ros_.SendMessage(cmd);
		return is_advertised_;
	}


	// Unadvertise an advertised service
	bool ROSService::Unadvertise() {
		if (!is_advertised_)
			return true;

		ROSBridgeUnadvertiseServiceMsg cmd(true);
		cmd.service_ = service_name_;

		is_advertised_ = !ros_.SendMessage(cmd);
		return !is_advertised_;
	}
}
