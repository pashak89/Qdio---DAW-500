/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 *
 */
#pragma once
#include <Core/BaseTypes/DMacro.h>
#include <Core/System/Event.h>

/// \brief This macro can define the events
#define D_Event(EventName, ReturnType, ...)                         \
public:                                                             \
	Core::System::Event<ReturnType, __VA_ARGS__>& EventName() const \
	{                                                               \
		return m_##EventName;                                       \
	}                                                               \
                                                                    \
protected:                                                          \
	mutable Core::System::Event<ReturnType, __VA_ARGS__> m_##EventName;

#define __D_CONNECT_0() D_ERROR_MACRO("Must At-least give 2 parameters")

#define __D_CONNECT(objectPointer, EventName, ...)                                               \
                                                                                                 \
	objectPointer->EventName() += static_cast<std::conditional<                                  \
		Core::BaseType::is_static_castable<                                                      \
			decltype(D_PFUNC(__VA_ARGS__)),                                                      \
   std::remove_reference<decltype(objectPointer->EventName())>::type>::value,           \
		std::remove_reference<decltype(objectPointer->EventName())>::type,                       \
		Core::System::Event<Core::BaseType::result_of<                                                      \
			std::remove_reference<decltype(objectPointer->EventName())>::type()>::type>>::type>( \
		D_PFUNC(__VA_ARGS__));

#define __D_CONNECT_2(EventName, ...) __D_CONNECT(this, EventName, __VA_ARGS__)
#define __D_CONNECT_3(objectPointer, EventName, ...) \
	__D_CONNECT(objectPointer, EventName, __VA_ARGS__)
#define __D_CONNECT_4(objectPointer, EventName, ...) \
	__D_CONNECT_3(objectPointer, EventName, __VA_ARGS__)

/// \brief DCONNECT definition section
#define __D_CONNECT_CHOOSE_FROM_ARG_COUNT(...)                                                  \
	FUNC_RECOMPOSER_9((__VA_ARGS__, __D_CONNECT_8, __D_CONNECT_7, __D_CONNECT_6, __D_CONNECT_5, \
					   __D_CONNECT_4, __D_CONNECT_3, __D_CONNECT_2, __D_CONNECT_1, ))

#define __D_CONNECT_NO_ARG_EXPANDER() , , , , , , , , __D_CONNECT_0
#define __D_CONNECT_MACRO_CHOOSER(...) \
	__D_CONNECT_CHOOSE_FROM_ARG_COUNT(__D_CONNECT_NO_ARG_EXPANDER __VA_ARGS__())

#define DConnect(...) __D_CONNECT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
