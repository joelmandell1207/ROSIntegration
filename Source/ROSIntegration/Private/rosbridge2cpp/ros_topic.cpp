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

#include "ros_topic.h"

#include "ROSIntegrationCore.h"

namespace rosbridge2cpp {

	ROSCallbackHandle<FunVrROSPublishMsg> ROSTopic::Subscribe(FunVrROSPublishMsg callback)
	{
		++subscription_counter_;
		UE_LOG(LogTemp, Display, TEXT("Subscribe being called"));
		// Only send subscribe when this ROSTopic hasn't sent this command before
		if (subscribe_id_ == "") {
			subscribe_id_.append("subscribe:");
			subscribe_id_.append(topic_name_);
			subscribe_id_.append(":");
			subscribe_id_.append(std::to_string(++ros_.id_counter));
			UE_LOG(LogTemp, Display, TEXT("Topic being subscribed to: %s"), *FString(topic_name_.c_str()));
			UE_LOG(LogTemp, Display, TEXT("Subscribe id: %s"), *FString(subscribe_id_.c_str()));

			ROSBridgeSubscribeMsg cmd(true);
			cmd.id_ = subscribe_id_;
			cmd.topic_ = topic_name_;
			cmd.type_ = message_type_;
			cmd.compression_ = compression_;
			cmd.throttle_rate_ = throttle_rate_;
			cmd.queue_length_ = queue_size_;

			if (!ros_.SendMessage(cmd))
			{
				subscribe_id_ = "";
			}
		}

		if (subscribe_id_ != "")
		{
			// Register callback in ROSBridge
			UE_LOG(LogTemp, Display, TEXT("Subscriber id not blank"));
			ROSCallbackHandle<FunVrROSPublishMsg> handle(callback);
			ros_.RegisterTopicCallback(topic_name_, handle); // Register callback in ROSBridge
			return handle;
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Subscriber id blank"));
		}

		subscribe_id_ = "";
		return ROSCallbackHandle<FunVrROSPublishMsg>();
	}

	bool ROSTopic::Unsubscribe(const ROSCallbackHandle<FunVrROSPublishMsg>& callback_handle)
	{
		UE_LOG(LogROS, Display, TEXT("Ros Topic unsubscribe being called"));
		// We've no active subscription
		if (subscribe_id_ == "")
			return false;

		if (!ros_.UnregisterTopicCallback(topic_name_, callback_handle)) { // Unregister callback in ROSBridge
			// failed to unregister callback - maybe the method is different from already registered callbacks
			std::cerr << "[ROSTopic] Passed unknown callback to ROSTopic::unsubscribe. This callback is not registered in the ROSBridge instance. Aborting..." << std::endl;
			return false;
		}

		--subscription_counter_;

		if (subscription_counter_ > 0)
			return true;

		std::cout << "[ROSTopic] No callbacks registered anymore - unsubscribe from topic" << std::endl;
		// Handle unsubscription when no callback is registered anymore
		//rapidjson::Document cmd;
		//cmd.SetObject();

		ROSBridgeUnsubscribeMsg cmd(true);
		cmd.id_ = subscribe_id_;
		cmd.topic_ = topic_name_;

		if (ros_.SendMessage(cmd)) {
			subscribe_id_ = "";
			subscription_counter_ = 0; // shouldn't be necessary ...
			return true;
		}
		return false;
	}

	bool ROSTopic::Advertise()
	{
		if (is_advertised_)
			return true;

		advertise_id_ = "";
		advertise_id_.append("advertise:");
		advertise_id_.append(topic_name_);
		advertise_id_.append(":");
		advertise_id_.append(std::to_string(++ros_.id_counter));

		ROSBridgeAdvertiseMsg cmd(true);
		cmd.id_ = advertise_id_;
		cmd.topic_ = topic_name_;
		cmd.type_ = message_type_;
		cmd.latch_ = latch_;
		cmd.queue_size_ = queue_size_;

		if (ros_.SendMessage(cmd)) {
			is_advertised_ = true;
		}
		return is_advertised_;
	}

	bool ROSTopic::Unadvertise()
	{
		if (!is_advertised_)
			return true;

		ROSBridgeUnadvertiseMsg cmd(true);
		cmd.id_ = advertise_id_;
		cmd.topic_ = topic_name_;

		if (ros_.SendMessage(cmd)) {
			is_advertised_ = false;
		}
		return !is_advertised_;
	}

	// void ROSTopic::Publish(json &message){
	//	if(!is_advertised_)
	//	Advertise();

	//   std::string publish_id;
	//   publish_id.append("publish:");
	//   publish_id.append(topic_name_);
	//   publish_id.append(":");
	//   publish_id.append(std::to_string(++ros_.id_counter));

	//   rapidjson::Document cmd;
	//   cmd.SetObject();
	//   cmd.AddMember("op","publish", cmd.GetAllocator());
	//   cmd.AddMember("id", publish_id, cmd.GetAllocator());
	//   cmd.AddMember("topic", topic_name_, cmd.GetAllocator());
	//   cmd.AddMember("msg", message, cmd.GetAllocator());
	//   cmd.AddMember("latch", latch_, cmd.GetAllocator());

	//   std::cout << "[ROSTopic] Publishing data " << Helper::get_string_from_rapidjson(cmd);

	//   ros_.SendMessage(cmd);
	// }

	bool ROSTopic::Publish(rapidjson::Value &message)
	{
		if (!is_advertised_) {
			if (!Advertise()) {
				return false;
			}
		}

		std::string publish_id = GeneratePublishID();

		ROSBridgePublishMsg cmd(true);
		cmd.id_ = publish_id;
		cmd.topic_ = topic_name_;
		cmd.msg_json_ = message;
		cmd.latch_ = latch_;

		return ros_.QueueMessage(topic_name_, queue_size_, cmd);
	}

	bool ROSTopic::Publish(bson_t *message)
	{

		if (!is_advertised_) {
			if (!Advertise()) {
				return false;
			}
		}

		assert(message);

		std::string publish_id = GeneratePublishID();

		ROSBridgePublishMsg cmd(true);
		cmd.id_ = publish_id;
		cmd.topic_ = topic_name_;
		cmd.msg_bson_ = message;
		cmd.latch_ = latch_;

		return ros_.QueueMessage(topic_name_, queue_size_, cmd);
	}

	std::string ROSTopic::GeneratePublishID()
	{

		std::string publish_id;
		publish_id.append("publish:");
		publish_id.append(topic_name_);
		publish_id.append(":");
		publish_id.append(std::to_string(++ros_.id_counter));
		return publish_id;
	}
}
