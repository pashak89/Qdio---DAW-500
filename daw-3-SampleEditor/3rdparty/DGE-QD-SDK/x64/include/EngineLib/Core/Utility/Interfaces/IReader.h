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
 * Reader interface header file is using for inheriting and adding ability of reading data
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
				/// \brief Interface of Reader class or object that can do reading operations with
				/// read functions
				class DGE_CoreAPI IReader
				{
				public:
					/// \brief Read amount of data and return them
					///
					/// \param size	Maximum size that you want to read.
					///
					/// \return Read data buffer
					virtual DByteArray read(uint64 size) = 0;

					/// \brief Read amount of data and return size of them
					///
					/// \param data	The buffer is used for store read data.
					/// \param size	Maximum size that you want to read.
					///
					/// \return Read data buffer
					virtual uint64 read(char* data, uint64 maxSize) = 0;

					/// \brief Read amount of data and return them
					virtual const DByteArray readAll() = 0;
				};
			} // namespace Interfaces
		}	  // namespace Utility
	}		  // namespace Core
} // namespace DGE
