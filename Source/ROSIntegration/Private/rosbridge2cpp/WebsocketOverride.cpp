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

#include "WebsocketOverride.h"
#include <random>
#include <vector>

#include "Animation/SkeletalMeshActor.h"
#include "Async/Async.h"
#include "Containers/Queue.h"
#include <set>
#include "UROSSharedState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"



#include "ROSIntegrationCore.h"




// create queue for brackets
// when you encounter {, add to queue
// when you encounter }, pop from queue
//		if queue is empty after pop, add string to vector of strings
//		set string = ""
//		set bracket mode = false and purge until you reach another {


TArray<FString> TestTrimString(FString& Str)
{
	UE_LOG(LogROS, Display, TEXT(" string incoming to trim string: %s"), *Str);
	TArray<FString> Result;
	TQueue<int8> Queue;
	FString s = "";
	int i = 0;
	bool bAddToString = false;
	int8 Popped = 0;
	while (i < Str.Len())
	{
		TCHAR Let = Str[i];
		FString LetterString = FString(1, &Let); // Construct a 1-character string

		if (bAddToString)
		{
			s += Str[i];
			if (Str[i] == '}')
			{
				Queue.Dequeue(Popped);
				// pop from queue
				if (Queue.IsEmpty())
				{
					// if queue is empty
					bAddToString = false;
					//	add string to vector, reset string
					Result.Add(s);
					s = "";
				}
			} else if (Str[i] == '{')
			{
				Queue.Enqueue(1);
			}
		} else
		{
			if (Str[i] == '{')
			{
				bAddToString = true;
				s += Str[i];
				Queue.Enqueue(1);
				// add to queue
			}
		}
		i++;
	}
	return Result;
}


void AssignToMap(FString Message, TMap<FName, float>& CommandMap)
{
	UE_LOG(LogROS, Display, TEXT("Message being parsed in assign to map: %s"), *Message);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
	// convert string into json object
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		UE_LOG(LogROS, Display, TEXT("Json successfully parsed"));
		// from json object get the field you want
		TSharedPtr<FJsonObject> MsgObj = JsonObject->GetObjectField("msg");
		if (MsgObj.IsValid())
		{
			FString DataString = MsgObj->GetStringField("data");
			UE_LOG(LogROS, Display, TEXT("Data string: %s"), *DataString);
			// turn data string into an object,
			
			// create json reader from string
			TSharedRef<TJsonReader<>> DataReader = TJsonReaderFactory<>::Create(DataString);
			// convert string into json object
			TSharedPtr<FJsonObject> DataReaderObject;
			if (FJsonSerializer::Deserialize(DataReader, DataReaderObject) && DataReaderObject.IsValid())
			{
				UE_LOG(LogROS, Display, TEXT("Successfully deserialized"));
				// create json object from your json object field
				TSharedPtr<FJsonObject> MsgObj2 = DataReaderObject->GetObjectField("data");
				if (MsgObj2.IsValid())
				{
					for (int i = 0; i <= 6; i++)
					{
						FString JointKey = FString::Printf(TEXT("angle%d"), i);
						if (MsgObj2->HasField(JointKey))
						{
							float Value = MsgObj2->GetNumberField(JointKey);
							UE_LOG(LogROS, Display, TEXT("Key: %s"), *JointKey);
							UE_LOG(LogROS, Display, TEXT("Angle: %f"), Value);
							if (CommandMap.Contains(FName(*JointKey)))
							{
								UE_LOG(LogROS, Display, TEXT("Key does exist, adding to value"));
								CommandMap[FName(*JointKey)] += ( Value);
							} else
							{
								UE_LOG(LogROS, Display, TEXT("Key does not exist yet, adding"));
								CommandMap.Add(FName(*JointKey), ( Value));
							}
							
						}
					}

				} else
				{
					UE_LOG(LogROS, Warning, TEXT("Inner object is not valid"));
				}
				
				
			} else
			{
				UE_LOG(LogROS, Error, TEXT("UnSuccessfully deserialized"));

			}


			
		}

	}
	
}





// Received json message:
// {"msg":{"data":"{\"seq\":4,\"address\":1,\"funcode\":2,\"data\":{\"mode\":1,\"angle0\":0.528898,\"angle1\":0.739695,\"angle2\":0.790155,\"angle3\":0.178930,\"angle4\":0.077253,\"angle5\":0.985718,\"angle6\":0.154807}}"}}

TMap<FName, float> ParseJson(FString Message)
{
	UE_LOG(LogROS, Display, TEXT("ParseJson(message) called on: %s"), *Message);
	TMap<FName, float> CommandMap;

	// clean message
	TArray<FString> Commands = TestTrimString(Message);
	for (FString& Word : Commands)
	{
		UE_LOG(LogROS, Display, TEXT("Command in commands: %s"), *Word);
		AssignToMap(Word, CommandMap);
	}
	// convert to json
	// create json reader from Fstring
	
	return CommandMap;
}

/**
 * Parses Json, returns a dictionary of joint names to the values they should be rotated
 * @param j 
 * @return 
 */

TMap<FName, float> ParseJson(json& j)
{
	// Received json message:
	// {"msg":{"data":"{\"seq\":4,\"address\":1,\"funcode\":2,\"data\":{\"mode\":1,\"angle0\":0.528898,\"angle1\":0.739695,\"angle2\":0.790155,\"angle3\":0.178930,\"angle4\":0.077253,\"angle5\":0.985718,\"angle6\":0.154807}}"}}
	UE_LOG(LogROS, Display, TEXT("Json being parsed"));
	// Convert to FString for UE_LOG
	TMap<FName, float> Map;
	return Map;
}



WebsocketOverride::WebsocketOverride(std::string IpAddress, int PortNum) : URI(""),
bIsConnected(false),
Port(PortNum),
ThreadRun(false),
IpAddr(IpAddress),
SocketStorage(INVALID_SOCKET)
{
	Init();


	std::function<void(json&)> SubscribeCallback = [this](json& j) -> void
	{
		// do a parse check of the message and if there's joint values, activate the next part

		// 1.) check if Value is actually being received in callback
		// 2.) check if correct joint names are being set
		// 3.) Check if that variable is actually being set in the animation blueprint (probably the issue)
		// 4.) check if that value is being updated and animated in that direction
		
		
		// get actor of class, cast to skeletal mesh, get anim bp, get control rig, call it...
		UE_LOG(LogROS, Display, TEXT("Calling callback function"));
		AsyncTask(ENamedThreads::GameThread, [this, &j]()
		{

			auto* State = UROSSharedState::Get();
			// switch to a setter method
			State->SetRaBase(CurrentCommand["angle0"]);
			// State->RA_Base = CurrentCommand["angle0"];
			State->SetRaPiv1(CurrentCommand["angle1"]);
			// State->RA_Piv1 = CurrentCommand["angle1"];
			State->SetRaPiv2(CurrentCommand["angle2"]);
			// State->RA_Piv2 = CurrentCommand["angle2"];
			State->SetRaPiv3(CurrentCommand["angle3"]);
			// State->RA_Piv3 = CurrentCommand["angle3"];
			State->SetRaPiv4(CurrentCommand["angle4"]);
			// State->RA_Piv4 = CurrentCommand["angle4"];
			State->SetRaPiv5(CurrentCommand["angle5"]);
			// State->RA_Hand1 = CurrentCommand["angle5"];
			State->SetRaPiv6(CurrentCommand["angle6"]);
			// State->RA_Hand2 = CurrentCommand["angle6"];
			
			
			// call the val save function from control rig
			UE_LOG(LogROS, Display, TEXT("Calling callback function"));
			UE_LOG(LogTemp, Display, TEXT("Message Incoming from Channel"));
			});
	};
	RegisterIncomingMessageCallback(SubscribeCallback);
}

WebsocketOverride::~WebsocketOverride() {
	NewClose(1000, TEXT("Websocket Closing, Object Being Destroyed"));
};


/**
 * Called in the destructor in order to clean up the Websocket. Closes the socket if it's open and
 * joins the receive_thread (checks for published data to ws)
 * @param Code Whether or not it's being closed cleanly or via error (right now just hard coded to 1000,
 * @param Reason For UE_LOG, explains context that it's closing
 */
void WebsocketOverride::NewClose(int32 Code, const FString& Reason)
{
	ThreadRun = false;
	// joins thread back to main
	if (ReceiveThread.joinable())
		ReceiveThread.join();
	// closes socket if it's open
	if (bIsConnected && SocketStorage != INVALID_SOCKET)
	{
		UE_LOG(LogROS, Display, TEXT("Closing socket connection..."));
		closesocket(SocketStorage);
		SocketStorage = INVALID_SOCKET;
		bIsConnected = false;
	} else
	{
		UE_LOG(LogROS, Warning, TEXT("Websocket not closing because there isn't one open"));
	}
	UE_LOG(LogROS, Display, TEXT("%s"), *Reason);
}




/**
 * 
 * @return whether you're currently connected to the websocket
 */
bool WebsocketOverride::CheckConnection() const
{
	return bIsConnected;
}

/**
 * TODO: Simplify so you only use one of these methods (See which one is actually being called)
 * @return Calls check connection
 */
bool WebsocketOverride::IsConnected()
{
	return CheckConnection();
}

/**
 * Checks if the handshake with the websocket was succesful
 * @param sock Handle reference to the socket that you're connected to
 * @return whether the handshake connecting to the server was successful
 */
bool CheckHandshake(SOCKET& Sock)
{
	// stores response from server
	char Buffer[40960];
	// recv call receives in the header from the server
	int Bytes_received = recv(Sock, Buffer, sizeof(Buffer) - 1, 0);
	if (Bytes_received > 0)
	{
		Buffer[Bytes_received] = '\0';
		std::string Response(Buffer);
		UE_LOG(LogROS, Display, TEXT("Handshake accepted, server response: %s"), *FString(Response.c_str()));
		return true;
	} else
	{
		UE_LOG(LogROS, Error, TEXT("Handshake with server failed"));
		return false;
	}
}

// generates a random base 64 encoded key to use for handshake
/**
 * TODO: make actually generate a random key and not just a hard coded one
 * @return random key to send in ws header
 */
std::string GenerateRandomKey()
{
	return "dGhlIHNhbXBsZSBub25jZQ==";
}

/**
 * TODO: just make this a member function so you don't have to pass in member fields
 * @param IpAddress ip address of the socket you want to connect to
 * @param PortNum port number you're connecting to
 * @param UriString full combo of ip address and port number (could just create this in the function)
 * @param Sock Socket handle
 * @return Whether the connection was successful
 */
bool SocketCreate(const std::string& IpAddress, const int& PortNum, const std::string& UriString, SOCKET& Sock)
{
	// creates the socket to connect to the rosbridge server
	// gives you the handle you need
	Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Sock == INVALID_SOCKET) {
		UE_LOG(LogROS, Error, TEXT("Failed to create socket"));
	}
	// specifies your IPv4 address
	sockaddr_in ServerAddress;
	// sets port, family type, and server address
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(PortNum);
	inet_pton(AF_INET, IpAddress.c_str(), &ServerAddress.sin_addr);
	// the actual TCP connection being called 
	if (connect(Sock, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
	{
		UE_LOG(LogROS, Error, TEXT("Failed to connect to TCP server"));
		return false;
	} else
	{
		UE_LOG(LogROS, Display, TEXT("Connected to TCP server!"));
		std::string Key = GenerateRandomKey();
		
		// handshake request message, upgrading to websocket connection
		std::ostringstream Request;
		Request << "GET / HTTP/1.1\r\n";
		Request << "Host: " << UriString << "\r\n";
		Request << "Upgrade: websocket\r\n";
		Request << "Connection: Upgrade\r\n";
		Request << "Sec-WebSocket-Key: " << Key << "\r\n";
		Request << "Sec-WebSocket-Version: 13\r\n\r\n";

		std::string ReqStr = Request.str();
		// sends the actual http upgrade request
		send(Sock, ReqStr.c_str(), ReqStr.size(), 0);

		// checking whether or not the handshake (http upgrade request) was successful
		return CheckHandshake(Sock);
	}
	
}

/**
 * Helper function called in constructor, creates and saves the URI to be used in ws connection
 */
void WebsocketOverride::Init()
{
	TArray< FStringFormatArg > Args;
	Args.Add(FStringFormatArg(IpAddr.c_str()));
	Args.Add(FStringFormatArg(Port));
	const FString ServerURL = FString::Format(TEXT("ws://{0}:{1}"), Args);
	
	UE_LOG(LogROS, Display, TEXT("URI: %s"), *ServerURL);
	URI = ServerURL;
	URIString = IpAddr + ":" + std::to_string(Port);
}

/**
 * Called to connect to websocket, uses socketCreate as helper
 * Also dispatches thread to continually loop and check for messages from the connection
 */
void WebsocketOverride::Connect()
{
	bIsConnected = SocketCreate(IpAddr, Port, URIString, SocketStorage);
	if (bIsConnected)
	{
		UE_LOG(LogROS, Display, TEXT("Dispatching receive loop thread"));
		// sets non-blocking mode for the socket if recv call fails
		u_long ThreadMode = 1;
		ioctlsocket(SocketStorage, FIONBIO, &ThreadMode);
		
		ThreadRun = true;
		// dispatch thread that continually loops and checks for message updates
		ReceiveThread = std::thread(&WebsocketOverride::ReceiveMessageLoop, this);
	}
}


/**
 * Send for string data
 * @param Data String you want to send over the ws connection
 */
void WebsocketOverride::Send(const FString& Data)
{
	UE_LOG(LogROS, Display, TEXT("Calling send string from websocket override"));
	std::string Utf8 = TCHAR_TO_UTF8(*Data);
	SendMessage(Utf8);
}

// for sending binary to server
/**
 * Send for binary data (don't really use this but needed to implement in case it gets called somewhere i don't know about)
 * @param Data Data being sent over
 * @param Size size of data
 * @param bIsBinary whether data is binary (should be or this wouldn't be getting called)
 */
void WebsocketOverride::Send(const void* Data, SIZE_T Size, bool bIsBinary)
{
	UE_LOG(LogROS, Display, TEXT("Calling send binary from websocket override"));
	SendMessage(reinterpret_cast<const uint8_t*>(Data), static_cast<unsigned int>(Size));
	
}

/**
 * helper for Send function, actually sends string data over the ws connection
 * @param Data string of data to send
 * @return whether send was successful
 */
bool WebsocketOverride::SendMessage(std::string Data)
{
	UE_LOG(LogROS, Display, TEXT("Calling send message string from websocket override"));

	if (!bIsConnected || SocketStorage == INVALID_SOCKET)
	{
		UE_LOG(LogROS, Error, TEXT("Can't send message, not connected to socket"));
		return false;
	}
	UE_LOG(LogROS, Display, TEXT("Message being sent from string SendMessage to server: %hs"), Data.c_str());

	// frame to actually put encoding in so it's ready to send to the ws
	std::vector<uint8_t> Frame;
	// opcode for text
	uint8_t Opcode = 0x1;
	// marks final frame and the opcode for it
	Frame.push_back(0x80 | Opcode);

	
	size_t Length = Data.size();
	if (Length <= 125) {
		Frame.push_back(0x80 | static_cast<uint8_t>(Length));
	} else if (Length <= 65535) {
		Frame.push_back(0x80 | 126);
		Frame.push_back((Length >> 8) & 0xFF);
		Frame.push_back(Length & 0xFF);
	} else {
		Frame.push_back(0x80 | 127);
		for (int i = 7; i >= 0; i--) {
			Frame.push_back((Length >> (i * 8)) & 0xFF);
		}
	}

	// Mask key (4 random bytes)
	uint8_t MaskKey[4];
	for (int i = 0; i < 4; i++) MaskKey[i] = rand() % 256;
	Frame.insert(Frame.end(), MaskKey, MaskKey + 4);

	// Mask the payload
	for (size_t i = 0; i < Length; i++) {
		Frame.push_back(static_cast<uint8_t>(Data[i]) ^ MaskKey[i % 4]);
	}

	// Send the frame over TCP
	int TotalSent = 0;
	const char* FrameData = reinterpret_cast<const char*>(Frame.data());
	int FrameSize = static_cast<int>(Frame.size());

	while (TotalSent < FrameSize)
	{
		// not the same send call as the one in websocket class, not an infinite loop, chill
		int Sent = send(SocketStorage, FrameData + TotalSent, FrameSize - TotalSent, 0);
		if (Sent == SOCKET_ERROR)
		{
			const int LastError = WSAGetLastError();
			UE_LOG(LogROS, Error, TEXT("WebSocket Send failed, WSA error %d"), LastError);
			return false;
		}
		TotalSent += Sent;
	}

	return true;
}

/**
 * Send message but for binary data (once again not really used but just in case you need to send binary)
 * @param Data binary data to send
 * @param Length size of data
 * @return 
 */
bool WebsocketOverride::SendMessage(const uint8_t* Data, unsigned int Length)
{
	UE_LOG(LogROS, Display, TEXT("Calling send message binary from websocket override"));
	if (!bIsConnected || SocketStorage == INVALID_SOCKET)
	{
		UE_LOG(LogROS, Error, TEXT("Can't send message, not connected to socket"));
		return false;
	}

	UE_LOG(LogROS, Display, TEXT("Sending message from binary"));
	UE_LOG(LogROS, Display, TEXT("Sending length: %u Data:%hhu "), Length, *Data);

	// create websocket frame to send package in

	// frame to put data in
	std::vector<uint8_t> Frame;

	// first byte of frame header, tells you it's a binary header
	uint8_t Opcode = 0x2;
	// sets final frame marker, as well as opcode
	Frame.push_back(0x80 | Opcode);
	

	// second byte of frame header, tells how long your frame is
	// Bit 1: mask, bits 2-8 payload length
	if (Length <= 125)
	{
		// store length in 7 bits
		Frame.push_back(0x80 | static_cast<uint8_t>(Length));
	} else if (Length <= 65535) {
		Frame.push_back(0x80 | 126);
		Frame.push_back((Length >> 8) & 0xFF);
		Frame.push_back(Length & 0xFF);
	} else {
		Frame.push_back(0x80 | 127);
		for (int i = 7; i >= 0; i--) {
			Frame.push_back((Length >> (i * 8)) & 0xFF);
		}
	}

	// Masking key (4 random bytes)
	uint8_t MaskKey[4];
	for (int i = 0; i < 4; i++) MaskKey[i] = rand() % 256;
	Frame.insert(Frame.end(), MaskKey, MaskKey + 4);

	// Mask the payload
	for (unsigned int i = 0; i < Length; i++) {
		Frame.push_back(Data[i] ^ MaskKey[i % 4]);
	}

	// Send frame over socket
	int TotalSent = 0;
	const char* FrameData = reinterpret_cast<const char*>(Frame.data());
	const int FrameSize = static_cast<int>(Frame.size());

	while (TotalSent < FrameSize) {
		const int Sent = send(SocketStorage, FrameData + TotalSent, FrameSize - TotalSent, 0);

		if (Sent == SOCKET_ERROR) {
			int Err = WSAGetLastError();
			UE_LOG(LogTemp, Error, TEXT("WebSocket Send failed, WSA error: %d"), Err);

			if (Err == WSAEWOULDBLOCK) {
				continue; // try again
			}
			return false;
		}

		TotalSent += Sent;
	}

	return true;
}

void WebsocketOverride::RegisterIncomingMessageCallback(std::function<void(json&)> Fun)
{
	_incoming_message_callback = Fun;
}

void WebsocketOverride::RegisterIncomingMessageCallback(std::function<void(bson_t&)> Fun)
{
	incoming_message_callback_bson_ = Fun;
}

void WebsocketOverride::RegisterErrorCallback(std::function<void(rosbridge2cpp::TransportError)> Fun)
{
	_error_callback = Fun;
}

void WebsocketOverride::ReportError(rosbridge2cpp::TransportError Err)
{
	if (_error_callback)
		_error_callback(Err);
}

void WebsocketOverride::SetTransportMode(rosbridge2cpp::ITransportLayer::TransportMode Mode)
{
	switch (Mode) {
	case rosbridge2cpp::ITransportLayer::JSON:
		bson_only_mode_ = false;
		break;
	case rosbridge2cpp::ITransportLayer::BSON:
		bson_only_mode_ = true;
		break;
	default:
		UE_LOG(LogROS, Error, TEXT("Given TransportMode not implemented!"));
	}
}

bool WebsocketOverride::IsHealthy() const
{
	return bIsConnected;
}


/**
 * Continually polls for updates on the ws connection. If there is new changes, triggers callback function
 * Called by the receive thread
 */
void WebsocketOverride::ReceiveMessageLoop()
{
	UE_LOG(LogROS, Display, TEXT("Calling receive message loop from thread"));
	// constexpr means value known at compile time, as opposed to regular const
	// important because you're allocating size of buffer at compile time
	constexpr size_t BUFFER_SIZE = 8192;
	// stores your incoming data from the socket
	uint8_t Buffer[BUFFER_SIZE];
	
	while (bIsConnected && ThreadRun)
	{
		// checking to see if there is any messages being sent over from server
		int ReceivedBytes = recv(SocketStorage,reinterpret_cast<char*>(Buffer), BUFFER_SIZE, 0);
		if (ReceivedBytes > 0)
		{
			if (bson_only_mode_)
			{
				// binary incoming, stores data from buffer as a bson_t
				bson_t b;
				if (!bson_init_static(&b, Buffer, ReceivedBytes))
				{
					UE_LOG(LogROS, Error, TEXT("Error parsing BSON in receive thread"));
				}
				else if (incoming_message_callback_bson_)
				{
					incoming_message_callback_bson_(b);
				}
			}
			else
			{
				// json mode, registers incoming message as a json
				FString MsgStr = UTF8_TO_TCHAR(reinterpret_cast<const char*>(Buffer));
				UE_LOG(LogROS, Display, TEXT("Received json message: %s"), *MsgStr);
				OnMessage(MsgStr);
			}
		} else if (ReceivedBytes == 0)
		{
			UE_LOG(LogROS, Warning, TEXT("Socket closed by remote"));
			ThreadRun = false;
			bIsConnected = false;
			break;
		}
		else
		{
			int Err = WSAGetLastError();
			if (Err != WSAEWOULDBLOCK)
			{
				UE_LOG(LogROS, Error, TEXT("Socket recv error: %d"), Err);
				ThreadRun = false;
				bIsConnected = false;
				break;
			}
			// adjust if you want more time in between publish checks (if the game starts to lag)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		
	}
	
}


/**
 * On updated data being sent
 * @param msg new data sent
 */
void WebsocketOverride::OnMessage(const FString& Msg) {
	json j;
	UE_LOG(LogROS, Display, TEXT("Received message, calling OnMessage"));
	try {
		j.Parse(TCHAR_TO_ANSI(*Msg)); //convert to UTF-8 and then reinterpret_cast the pointer to char *
		CurrentCommand = ParseJson(Msg);
		if (_incoming_message_callback)
			_incoming_message_callback(j);
	}
	catch (...) {
		UE_LOG(LogROS, Error, TEXT("Failed to parse JSON - Ignoring message"));
	}
}


void WebsocketOverride::OnRawMessage(const void* Data, size_t Size, size_t BytesRemaining)
{
	UE_LOG(LogROS, Display, TEXT("Received raw message, calling OnRawMessage"));
	if (BytesRemaining > 0) {
		UE_LOG(LogROS, Warning, TEXT("Websocket message fragments not supported - Ignoring message"));
		return;
	}
	bson_t b;
	if (!bson_init_static(&b, reinterpret_cast<const uint8_t*>(Data), Size)) {
		UE_LOG(LogROS, Error, TEXT("Error on BSON parse - Ignoring message"));
	} else if (incoming_message_callback_bson_) {
		incoming_message_callback_bson_(b);
	}
}

//function void IWebSocket::Close(int32 Code, const FString& Reason) is pure virtual
//	function bool IWebSocket::IsConnected() is pure virtual
//	function void IWebSocket::Send(const FString& Data) is pure virtual
//	function void IWebSocket::Send(const void* Data, SIZE_T Size, bool bIsBinary) is pure virtual
//	function void IWebSocket::SetTextMessageMemoryLimit(uint64 TextMessageMemoryLimit) is pure virtual
//	function IWebSocket::FWebSocketConnectedEvent& IWebSocket::OnConnected() is pure virtual
//	function IWebSocket::FWebSocketConnectionErrorEvent& IWebSocket::OnConnectionError() is pure virtual
//	function IWebSocket::FWebSocketClosedEvent& IWebSocket::OnClosed() is pure virtual
//	function IWebSocket::FWebSocketMessageEvent& IWebSocket::OnMessage() is pure virtual
//	function IWebSocket::FWebSocketBinaryMessageEvent& IWebSocket::OnBinaryMessage() is pure virtual
//	function IWebSocket::FWebSocketRawMessageEvent& IWebSocket::OnRawMessage() is pure virtual
//	function IWebSocket::FWebSocketMessageSentEvent& IWebSocket::OnMessageSent() is pure virtual