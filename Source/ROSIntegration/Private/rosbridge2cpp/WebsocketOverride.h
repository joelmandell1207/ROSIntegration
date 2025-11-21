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

#include <functional> // std::function

#include <CoreMinimal.h>
#include "IWebSocket.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")

//#include Websocket stuff
#include "itransport_layer.h"
#include "types.h"
#include <thread>
#include "rapidjson/document.h"
using json = rapidjson::Document;

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "IWebSocket.h"

class WebsocketOverride : public IWebSocket
{
public:
	// class meant to take the place of IWebSocket, since it sucks
	WebsocketOverride(std::string IpAddress, int PortNum);
	virtual ~WebsocketOverride() override;
	
	void Init();
	// main thing you want it to do is connect

	virtual void Connect() override;
	
	// Implement all IWebSocket pure virtual methods as functions
	virtual void Close(int32 Code, const FString& Reason) override {}
	// can probably just consolidate this into Close
	void NewClose(int32 Code, const FString& Reason);
	// check handle stored in SOCKET (may need to store as a field)
	virtual bool IsConnected() override;
	// actually sending topic to subscribe or data?
	virtual void Send(const FString& Data) override;
	virtual void Send(const void* Data, SIZE_T Size, bool bIsBinary) override;
	virtual void SetTextMessageMemoryLimit(uint64 TextMessageMemoryLimit) override {}

	virtual FWebSocketConnectedEvent& OnConnected() override {
		static FWebSocketConnectedEvent Dummy; 
		return Dummy;
	}

	virtual FWebSocketConnectionErrorEvent& OnConnectionError() override {
		static FWebSocketConnectionErrorEvent Dummy; 
		return Dummy;
	}

	virtual FWebSocketClosedEvent& OnClosed() override {
		static FWebSocketClosedEvent Dummy; 
		return Dummy;
	}

	// need to implememnt on Message and on Binary message in order to actually receive messages
	virtual FWebSocketMessageEvent& OnMessage() override {
		return MessageEvent;
	}

	virtual FWebSocketBinaryMessageEvent& OnBinaryMessage() override {
		return BinaryMessageEvent;
	}

	virtual FWebSocketRawMessageEvent& OnRawMessage() override {
		return RawMessageEvent;
	}

	virtual FWebSocketMessageSentEvent& OnMessageSent() override {
		return MessageSentEvent;
	}

	// thread that continually calls recv to check for new messages
	void ReceiveMessageLoop();

	
	bool CheckConnection() const;
	bool SendMessage(std::string Data);
	bool SendMessage(const uint8_t* Data, unsigned int Length);
	void RegisterIncomingMessageCallback(std::function<void(json&)> Fun);
	void RegisterIncomingMessageCallback(std::function<void(bson_t&)> Fun);
	void RegisterErrorCallback(std::function<void(rosbridge2cpp::TransportError)> Fun);
	void ReportError(rosbridge2cpp::TransportError Err);
	void SetTransportMode(rosbridge2cpp::ITransportLayer::TransportMode);

	bool IsHealthy() const;

private:
	void OnConnectionError(const FString& Error);
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnRawMessage(const void*, size_t Size, size_t BytesRemaining);
	void OnMessage(const FString& Msg);
	FString URI;
	std::string URIString;
	bool bIsConnected;
	SOCKET SocketStorage;
	FWebSocketMessageEvent MessageEvent;
	FWebSocketBinaryMessageEvent BinaryMessageEvent;
	FWebSocketRawMessageEvent RawMessageEvent;
	FWebSocketMessageSentEvent MessageSentEvent;

	// thread safe bool, whether of not receive check thread is running
	std::atomic<bool> ThreadRun;
	// thread that continually checks for incoming message
	std::thread ReceiveThread;

	TSharedPtr<IWebSocket> WebSocket;
	std::string IpAddr;
	int Port;

	bool bson_only_mode_ = false;
	std::function<void(json&)> _incoming_message_callback;
	std::function<void(bson_t&)> incoming_message_callback_bson_;
	std::function<void(rosbridge2cpp::TransportError)> _error_callback;

	TMap<FName, float> CurrentCommand;
	
};
