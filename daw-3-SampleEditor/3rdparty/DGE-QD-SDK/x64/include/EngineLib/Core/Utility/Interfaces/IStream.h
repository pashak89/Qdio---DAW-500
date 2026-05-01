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
 * Stream interface header file is using for inheriting and adding ability of streaming data
 */
#pragma once

#include "IReader.h"
#include "IWriter.h"
#include <Core/BaseTypes/Flags.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			namespace Interfaces
			{
				/// \enum DGE::Core::Interfaces::OpenMode
				/// \brief Use this enumeration to set that how open stream.
				/// \var OpenMode::ReadOnly open stream in read only mode.
				/// \var OpenMode::WriteOnly try to open stream in write only mode.
				/// \var OpenMode::ReadWrite try to open stream in read and write mode.
				/// \var OpenMode::CreateOnFail create file if opening is failed.
				enum class OpenMode
				{
					None		 = 0x000,
					ReadOnly	 = 0x001,
					WriteOnly	 = 0x002,
					ReadWrite	 = ReadOnly | WriteOnly,
					Binary		 = 0x010,
					CreateOnFail = 0x020
				};

				D_EnumFlags(OpenMode);

				/// \brief Interface of Strean class or object that can do streaming operations with
				/// read & write functions
				class IStream
					: public DGE::Core::Utility::Interfaces::IReader
					, public DGE::Core::Utility::Interfaces::IWriter
				{
				public:
					/// \brief try to open stream with specified open mode.
					///
					/// \param openMode is used for type of opening stream.
					virtual bool open(OpenModes openMode = OpenMode::ReadOnly) = 0;

					/// \brief Close stream
					virtual bool close() = 0;

					/// \brief Indicate whether file has been opened or not.
					///
					/// \return return true if file had been opened.
					virtual bool hasOpened() = 0;
				};

			} // namespace Interfaces
		}	  // namespace Utility
	}		  // namespace Core
} // namespace DGE
