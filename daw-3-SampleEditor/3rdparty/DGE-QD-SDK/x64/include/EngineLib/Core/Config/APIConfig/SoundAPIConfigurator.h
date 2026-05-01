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

#include "../EngineConfigurator.h"

#pragma region Sound API Configourator
	#ifdef DGE_UseSoundAPI
		#if DGE_Sound_API==DGE_Use_DirectMusic
			#pragma region DirectSound Library

				#undef FACILITY_DIRECTMUSIC
//#include <dmusici.h>
//#pragma comment(lib, "dxerr8.lib")
//#pragma comment(lib, "dxguid8.lib")
//#pragma comment(lib, "dsound.lib")
#if defined(DEBUG) || defined(_DEBUG)
#else
#endif

#pragma endregion
#else

#endif

#endif
#pragma endregion
