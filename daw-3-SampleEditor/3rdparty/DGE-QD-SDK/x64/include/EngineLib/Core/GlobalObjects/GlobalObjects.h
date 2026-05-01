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
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Windows/Window.h>

#include <Core/ResourceManager/ResourceLoader.h>

#include <Core/OS/Timer.h>

namespace DGE
{
	namespace GlobalObjects
	{

		/// \brief Global object that use for writing those logs that Logger subsystem couldn't write it.
		extern DGE_CoreExtern fstream ferror;

		// true when error occur in engine.
		extern DGE_CoreExtern atomic_bool g_ErrorOccur;

		// true when window is lost focus or minimize.
		extern DGE_CoreExtern atomic_bool g_Disable;
#ifndef _M_CEE
		// use for enable thread part engine.
		extern DGE_CoreExtern Sync::StdMutex* g_mutexInputDevice;
		extern DGE_CoreExtern Sync::StdMutex*		  g_mutexGraphicDevice;
		extern DGE_CoreExtern condition_variable_any* g_condVariableInputDevice;
#endif

	} // namespace GlobalObjects
} // namespace DGE
