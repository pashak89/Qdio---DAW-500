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
 * Writer interface header file is using for inheriting and adding ability of writing data
 */
#pragma once

#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			namespace Interfaces
			{
				/// \brief Interface of Writer class or object that can do writing operations with
				/// write functions
				class DGE_CoreAPI IWriter
				{
				public:
					/// \brief Write amount of data and size of written data
					///
					/// \return	Actual size that is written.
					virtual bool   writeAll(DString data)				= 0;
					virtual uint64 write(DByteArray byteArray)			= 0;
					virtual uint64 write(char character)				= 0;
					virtual uint64 write(DString str)					= 0;
					virtual uint64 write(int16 number)					= 0;
					virtual uint64 write(int32 number)					= 0;
					virtual uint64 write(int64 number)					= 0;
					virtual uint64 write(uint8 number)					= 0;
					virtual uint64 write(uint16 number)					= 0;
					virtual uint64 write(uint32 number)					= 0;
					virtual uint64 write(uint64 number)					= 0;
					virtual uint64 write(double number)					= 0;
					virtual uint64 write(float number)					= 0;
					virtual uint64 write(const char* data, uint64 size) = 0;
				};
			} // namespace Interfaces
		}	  // namespace Utility
	}		  // namespace Core
} // namespace DGE
