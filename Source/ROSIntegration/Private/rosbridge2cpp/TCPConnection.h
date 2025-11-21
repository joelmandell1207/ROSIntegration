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

#include <iostream>
//#include <stdio.h>
//#include <string>
//#include <chrono>
#include <thread>

#include <functional> // std::function

// #include "json.hpp"
#include <CoreMinimal.h>
//#include <ThreadingBase.h>
#include <Sockets.h>
#include <SocketSubsystem.h>
//#include <Networking.h> // Unreal networking


#include "itransport_layer.h"
#include "types.h"
//

#include "rapidjson/document.h"
using json = rapidjson::Document;

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#pragma warning(disable:4265)
class TCPConnection : public rosbridge2cpp::ITransportLayer {
public:
	TCPConnection() {
	}
	~TCPConnection() {
		run_receiver_thread = false;
		if (receiverThreadSetUp) {
			receiverThread.join(); // Wait for the receiver thread to finish
		}
		if (_sock != nullptr) {
			_sock->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(_sock);
		}
	}

	bool Init(std::string ip_addr, int port);
	bool SendMessage(std::string data);
	bool SendMessage(const uint8_t *data, unsigned int length);
	uint16_t Fletcher16(const uint8_t *data, int count);
	int ReceiverThreadFunction();
	void RegisterIncomingMessageCallback(std::function<void(json&)> fun);
	void RegisterIncomingMessageCallback(std::function<void(bson_t&)> fun);
	void RegisterErrorCallback(std::function<void(rosbridge2cpp::TransportError)> fun);
	void ReportError(rosbridge2cpp::TransportError err);
	void SetTransportMode(rosbridge2cpp::ITransportLayer::TransportMode);

	bool IsHealthy() const;

private:
	std::string _ip_addr;
	int _port;

	FSocket *_sock = nullptr;
	// int sock = socket(AF_INET , SOCK_STREAM , 0);
	// struct sockaddr_in connect_to;
	std::thread receiverThread;
	bool run_receiver_thread = true;
	bool receiverThreadSetUp = false;
	bool _callback_function_defined = false;
	bool bson_only_mode_ = false;
	std::function<void(json&)> _incoming_message_callback;
	std::function<void(bson_t&)> incoming_message_callback_bson_;
	std::function<void(rosbridge2cpp::TransportError)> _error_callback;
};
#pragma warning(default:4265)
