
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

#include "types.h"

/*
 * This class provides an interfaces for generic Transportlayers that can be used by the ROSBridge.
 * Since this library has been developed for different plattforms,
 * we abstract from the actual transport layer, since this varies from plattforms (and frameworks) a lot.
 *
 * Please see client/socket_tcp_connection.h for an example implementation that uses UNIX Sockets
 * to connect to a ROSBridge server
 */
namespace rosbridge2cpp {
	class ITransportLayer {
	public:
		enum TransportMode { JSON, BSON };
		virtual ~ITransportLayer() = default;
		
		// Initialize the TransportLayer by connecting to the given IP and port
		// The implementing class should have an active connection to IP:port
		// when the method has been executed completly.
		// Returns true if the connection has been successfully.
		virtual bool Init(std::string ip_addr, int port) = 0;

		// Send a string over the underlying transport mechanism to the rosbridge server
		virtual bool SendMessage(std::string data) = 0;

		// Send a string over the underlying transport mechanism to the rosbridge server
		virtual bool SendMessage(const uint8_t *data, unsigned int length) = 0;

		// Register a std::function that will be called whenever a new data packet has been received by this TransportLayer.
		virtual void RegisterIncomingMessageCallback(std::function<void(json&)>) = 0;

		// Register a std::function that will be called whenever a new data packet has been received by this TransportLayer.
		virtual void RegisterIncomingMessageCallback(std::function<void(bson_t&)>) = 0;

		// Register a std::function that will be called when errors occur.
		virtual void RegisterErrorCallback(std::function<void(TransportError)>) = 0;

		// Report an error to the registered ErrorCallback (see RegisterErrorCallback)
		virtual void ReportError(TransportError) = 0;

		// Report an error to the registered ErrorCallback (see RegisterErrorCallback)
		virtual void SetTransportMode(TransportMode) = 0;
	private:
		/* data */
	};
}
