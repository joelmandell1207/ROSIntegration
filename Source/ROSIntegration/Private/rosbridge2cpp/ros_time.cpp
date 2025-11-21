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

#include "ros_time.h"

namespace rosbridge2cpp {

	bool ROSTime::use_sim_time = false;
	ROSTime ROSTime::sim_time(0, 0);

	static std::chrono::high_resolution_clock::duration ComputeHRC1970EpocDelta()
	{
		const std::chrono::high_resolution_clock::time_point hr_time_since_epoch = std::chrono::high_resolution_clock::now();
		const std::chrono::system_clock::time_point time_since_epoch = std::chrono::system_clock::now();
		return time_since_epoch.time_since_epoch() - hr_time_since_epoch.time_since_epoch();
	}

	const std::chrono::high_resolution_clock::duration ROSTime::HRCEpocOffset = ComputeHRC1970EpocDelta();

	ROSTime ROSTime::now() {
		if (use_sim_time) {
			return sim_time;
		}

		const std::chrono::high_resolution_clock::duration time_since_epoch = HRCEpocOffset + std::chrono::high_resolution_clock::now().time_since_epoch();
		unsigned long seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
		unsigned long long nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch).count();
		unsigned long nanosecond_difference = nanoseconds_since_epoch - (seconds_since_epoch * 1000000000ul);
		return ROSTime(seconds_since_epoch, nanosecond_difference);
	}
}
