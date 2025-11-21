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

#include <thread>
#include <atomic>

namespace rosbridge2cpp {

	class spinlock
	{
	private:
		std::atomic_flag lock_;

	public:
		spinlock()
		{
			lock_.clear(std::memory_order_release);
		}

		bool try_lock()
		{
			return !lock_.test_and_set(std::memory_order_acquire);
		}

		void lock(const bool sleep = false)
		{
			while (!try_lock())
			{
				if (sleep)
				{
					std::this_thread::yield();
				}
			}
		}

		void unlock()
		{
			lock_.clear(std::memory_order_release);
		}

	public:

		template<bool WaitForLongTask>
		class scoped_lock
		{
		private:
			spinlock& spinlock_;
			scoped_lock(scoped_lock const &);
			scoped_lock & operator=(scoped_lock const &);

		public:
			explicit scoped_lock(spinlock& sp) : spinlock_(sp)
			{
				sp.lock(WaitForLongTask);
			}

			~scoped_lock()
			{
				spinlock_.unlock();
			}
		};

		// assume the other task may take longer, so use sleep/yield
		typedef scoped_lock<true> scoped_lock_wait_for_long_task;
		// assume the other task finishes soon, so spin wait
		typedef scoped_lock<false> scoped_lock_wait_for_short_task;
	};

} // namespace rosbridge2cpp
