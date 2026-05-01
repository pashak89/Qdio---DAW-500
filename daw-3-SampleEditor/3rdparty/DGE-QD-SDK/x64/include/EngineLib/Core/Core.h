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
#include <Core/Array/Array.h>
#include <Core/Array/LinkList.h>
#include <Core/Array/Tree.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Config/EngineConfigurator.h>
#include <Core/CoreModule.h>
#include <Core/IO/FileSystem/FileManager.h>
#include <Core/IO/Input/InputEventDef.h>
#include <Core/IO/Input/InputEvents.h>
#include <Core/IO/Input/InputManager.h>
#include <Core/IO/Input/Mouse.h>
#include <Core/IO/LogSystem/Log.h>
#include <Core/Math/Math.h>
#include <Core/OS/OS.h>
#include <Core/Platforms/Platform.h>
#include <Core/Profiling/Fps.h>
#include <Core/Profiling/Profiler.h>
#include <Core/ResourceManager/Loaders/Loader.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/ResourceManager/ResourceLoadersManager.h>
#include <Core/Sync/BaseThread.h>
#include <Core/System/Context.hpp>
#include <Core/System/DApplicationInfo.h>
#include <Core/System/DModule.h>
#include <Core/System/DObject.h>
#include <Core/System/DSubmodule.h>
#include <Core/System/Functor.h>
#include <Core/Utility/Time.h>
#include <Core/Windows/MessageToString.h>
#include <Core/Windows/WinWindow.h>
#include <Core/Windows/Window.h>
#include <Core/XML/XmlTools.h>
