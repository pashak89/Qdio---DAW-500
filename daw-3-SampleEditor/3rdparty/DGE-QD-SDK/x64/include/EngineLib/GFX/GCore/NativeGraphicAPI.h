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
#include <Core/Config/EngineConfigurator.h>

#define GL_GLEXT_PROTOTYPES
#if DGE_Platform != DGE_Mac_Platform && DGE_Platform != DGE_Android_Platform
#if (!defined(DGE_USE_QT))
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#if defined(DGE_USE_QT)
#include <GFX/GCore/Qt/QtRenderer.h>
// #include <QOpenGLExtraFunctions>
#endif

// #include <GL/glext.h>
// #include <GLES3/gl3.h>
// #include <GLES3/gl3ext.h>
#elif DGE_Platform != DGE_Android_Platform
#if (!defined(DGE_USE_QT))
#include <GL/glew.h>
#else
#include <QOpenGLExtraFunctions>
#endif
#else
#if defined(DGE_USE_QT)
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#endif

// Check GL
#if (defined(_DEBUG) || !defined(NDEBUG)) && !defined(EMSCRIPTEN)
#define GLCHECKER(caller)                                                                         \
	{                                                                                             \
		caller;                                                                                   \
		int error = glGetError();                                                                 \
		if (error != GL_NO_ERROR)                                                                 \
		{                                                                                         \
			std::cout << "GL Error: " << std::hex << error << " FUNCTION: " << #caller            \
					  << " LINE: " << std::dec << __LINE__ << " FILE: " << __FILE__ << std::endl; \
			BRK;                                                                                  \
		}                                                                                         \
	}
#else
#define GLCHECKER(caller) \
	{                     \
		caller;           \
	}
#endif
