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
 * BaseType header is using for defining basic type in the engine
 */
#pragma once
#include <Core/BaseTypes/CPU/CPU.h>
#include <cstdint>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			//! -----------------------------------------------------------------
			//! Base Type Definition Engine.
			//! -----------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MaxPathLength 260
#define MaxLengthName 50
#define MaxMessageBuffer 2048
#define MaxTextLength MaxMessageBuffer
#define MaxVarName MaxLengthName // max length of name of variable

#ifndef uint8
			typedef std::uint8_t uint8;
#endif

#ifndef uint16
			typedef std::uint16_t uint16;
#endif

#ifndef uint32
			typedef std::uint32_t uint32;
#endif

#ifndef uint64
			typedef std::uint64_t uint64;
#endif

#ifndef int8
			typedef std::int8_t int8;
#endif

#ifndef int16
			typedef std::int16_t int16;
#endif

#ifndef int32
			typedef std::int32_t int32;
#endif

#ifndef int64
			typedef std::int64_t int64;
#endif

			typedef void* MemPtr;

			using TypeInfoRef = std::shared_ptr<const std::type_info>;

			using DByteArray = std::vector<uint8>;
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE

#include "DString.h"

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{

#ifdef UseWideChar
			using DChar	  = wchar_t; // Dynamic Character can compile in Wide mode or Simple mode.
			using dstring = std::wstring;
			using DString = WString;
			using DWIN32_FIND_DATA = WIN32_FIND_DATAW;
			using DOPENFILENAME	   = OPENFILENAMEW;
			using DD3DXFONT_DESC   = D3DXFONT_DESCW;

#define DL(x) L##x // dynamic character converter
#define DCreateFile CreateFileW
#define DFindFirstFile FindFirstFileW
#define DGetCurrentDirectory GetCurrentDirectoryW
#define DSearchPath SearchPathW
#define DMoveFile MoveFileW
#define DDeleteFile DeleteFileW
#define DCreateDirectory CreateDirectoryW
#define DRemoveDirectory RemoveDirectoryW
#define DRename _wrename
#define DGetOpenFileName GetOpenFileNameW
#define DStrcpy wcscpy
#define DStrcmp wcscmp
#define DStrlen wcslen
#define DOpenFile _wfopen_s
#define DSprintf swprintf
#define DSprintf_s swprintf_s
#define DAccess _waccess
#define DD3DXCreateFontIndirect D3DXCreateFontIndirectW
#define DDrawText DrawTextW
#define DGE_Version_Name DGE_Version_NameW
#define DFreeImage_GetFileType FreeImage_GetFileTypeU
#define DFreeImage_Load FreeImage_LoadU
#define DST WST

#else
			using DChar	  = char; // Dynamic Character can compile in Wide mode or Simple mode.
#if DGE_Platform == DGE_Windows_Platform
			using DWIN32_FIND_DATA = WIN32_FIND_DATAA;
			using DOPENFILENAME	   = OPENFILENAMEA;
#endif
			//#if DGE_Graphic_API==DGE_Use_Directx
			//				using DD3DXFONT_DESC=D3DXFONT_DESCA;
			//#endif
#define DL(x) x // dynamic character converter
#define DCreateFile CreateFileA
#define DFindFirstFile FindFirstFileA
#define DGetCurrentDirectory GetCurrentDirectoryA
#define DSearchPath SearchPathA
#define DMoveFile MoveFileA
#define DDeleteFile DeleteFileA
#define DCreateDirectory CreateDirectoryA
#define DRemoveDirectory RemoveDirectoryA
#define DRename rename
#define DGetOpenFileName GetOpenFileNameA
#define DStrcpy strcpy
#define DStrcmp strcmp
#define DStrlen strlen
#define DOpenFile fopen
#define DSprintf sprintf
#define DSprintf_s sprintf_s
#if DGE_Platform == DGE_Windows_Platform
#define DAccess _access
#elif DGE_Platform == DGE_Linux_Platform || DGE_Platform==DGE_Android_Platform
#include <unistd.h>
#define DAccess access
#endif
#define DD3DXCreateFontIndirect D3DXCreateFontIndirectA
#define DDrawText DrawTextA
#define DGE_Version_Name DGE_Version_NameA
#define DFreeImage_GetFileType FreeImage_GetFileType
#define DFreeImage_Load FreeImage_Load
#define DST ST

#endif

		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
using namespace ::DGE::Core::BaseType;
