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



#include "UROSSharedState.h"

static UROSSharedState* SingletonInstance = nullptr;

/**
 * Gives you the shared state for your project so you can access its variables
 * @return returns the RosShared state being stored
 */
UROSSharedState* UROSSharedState::Get()
{
	if (!SingletonInstance)
	{
		SingletonInstance = NewObject<UROSSharedState>();
		SingletonInstance->AddToRoot(); // prevents garbage collection
	}
	return SingletonInstance;
}

/**
 * 
 * @return Returns base joint
 */
float UROSSharedState::GetBase()
{
	return RaBase;
}

/**
 * 
 * @return Returns first pivot joint
 */
float UROSSharedState::GetPiv1()
{
	return RaPiv1;
}

/**
 * 
 * @return Returns second pivot joint
 */
float UROSSharedState::GetPiv2()
{
	return RaPiv2;
}

/**
 * 
 * @return Returns third pivot joint
 */
float UROSSharedState::GetPiv3()
{
	return RaPiv3;
}

/**
 * 
 * @return returns fourth pivot joint
 */
float UROSSharedState::GetPiv4()
{
	return RaPiv4;
}

/**
 * 
 * @return returns fifth pivot joint
 */
float UROSSharedState::GetPiv5()
{
	return RaPiv5;
}

/**
 * 
 * @return returns sixth pivot joint
 */
float UROSSharedState::GetPiv6()
{
	return RaHand1;
}

/**
 * 
 * @param x value setting base to
 */
void UROSSharedState::SetRaBase(float x)
{
	RaBase = x;
}

void UROSSharedState::SetRaPiv1(float x)
{
	RaPiv1 = x;
}
void UROSSharedState::SetRaPiv2(float x)
{
	RaPiv2 = x;
}
void UROSSharedState::SetRaPiv3(float x)
{
	RaPiv3 = x;
}
void UROSSharedState::SetRaPiv4(float x)
{
	RaPiv4 = x;
}
void UROSSharedState::SetRaPiv5(float x)
{
	RaPiv5 = x;
}
void UROSSharedState::SetRaPiv6(float x)
{
	RaHand1 = x;
}

void UROSSharedState::SetRaPiv7(float x)
{
	RaHand2 = x;
}