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
 * CPU class is using for accessing properties of CPU
 */

#pragma once
#include <Core/Config/EngineConfigurator.h>

extern DGE_CoreExtern void cpuid(int CPUInfo[4], int InfoType);

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			namespace CPU
			{

				/// \brief A utility class for detect feature of CPU
				class DGE_CoreAPI CPU_Base
				{
					/**
					 * Function to detect SSE availability in CPU.
					 */
					void detectCPUInfo();

					/**
					 * Function to detect SSE availability in operating system.
					 */
					// bool OSSupportsSSE()
					//{
					//	// try SSE instruction and look for crash
					//	__try
					//	{
					//		_asm xorps xmm0, xmm0
					//	}
					//	__except (EXCEPTION_EXECUTE_HANDLER)
					//	{
					//		if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
					//			return false;  // sse not supported by os
					//		return false;     // unknown exception occured
					//	}

					//	return true;
					//}

					void getCPUInfo();
					/*----------------------------------------------------------------*/

				public:
					bool	x64;
					bool	MMX;   // MMX support
					bool	MMXEX; // MMX (AMD specific extensions)
					bool	SSE;   // Streaming SIMD Extensions
					bool	SSE2;  // Streaming SIMD Extensions 2
					bool SSE3;
					bool SSSE3;
					bool SSE41;
					bool SSE42;
					bool SSE4a;
					bool AVX;
					bool XOP;
					bool FMA3;
					bool FMA4;
					bool	bEXT;	  // extended features available
					bool	b3DNOW;	  // 3DNow! (vendor independent)
					bool	b3DNOWEX; // 3DNow! (AMD specific extensions)
					char	name[48]; // cpu name
					char	vendor[13]; // vendor name

					CPU_Base()
					{
						x64 = MMX = SSE = SSE2 = SSE3 = SSSE3 = SSE41 = SSE42 = SSE4a = AVX = XOP
							= FMA3 = FMA4 = false;
						getCPUInfo();
					}
				};
			} // namespace CPU
		}	  // namespace BaseType
	}		  // namespace Core
} // namespace DGE
