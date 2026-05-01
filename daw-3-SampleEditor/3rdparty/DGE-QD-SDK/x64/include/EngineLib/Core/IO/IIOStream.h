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
 * IOStream interface header file is using for adding an interface of IO communication.
 */
#pragma once

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Utility/Interfaces/IStream.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			using namespace Core::Utility::Interfaces;

			/// \brief The IOStream is an abstract class that is used for reading from and
			/// writing to IO device.
			class DGE_CoreAPI IIOStream : public Utility::Interfaces::IStream
			{
			public:
				// IStream interface
				virtual bool open(OpenModes openMode = OpenMode::ReadOnly) override;

				virtual bool close() override;

				//  IReader interface
				virtual DByteArray		 read(uint64 size) override;
				virtual uint64			 read(char* data, uint64 maxSize) override;
				virtual const DByteArray readAll() override;

				// IWriter interface
				virtual bool   writeAll(DString data) override;
				virtual uint64 write(DByteArray byteArray) override;
				virtual uint64 write(char character) override;
				virtual uint64 write(DString str) override;
				virtual uint64 write(int16 number) override;
				virtual uint64 write(int32 number) override;
				virtual uint64 write(int64 number) override;
				virtual uint64 write(uint8 number) override;
				virtual uint64 write(uint16 number) override;
				virtual uint64 write(uint32 number) override;
				virtual uint64 write(uint64 number) override;
				virtual uint64 write(double number) override;
				virtual uint64 write(float number) override;
				virtual uint64 write(const char* data, uint64 size) override;

			protected:
				/// \brief do reading operation with specified maximum size and if size is zero
				/// will read all data that will be available.
				///
				/// \param maxSize Maximum size that want to be read and if be zero will be read
				/// all data
				///
				/// \return Return array of bytes that can be read.
				virtual DByteArray doRead(uint64 maxSize = 0) = 0;

				/// \brief do writing operation with specified size and data buffer
				///
				/// \param data The buffer of data that want to be written
				/// \param size Size of data that want to be written
				///
				/// \return	Actual size that is written.
				virtual uint64 doWrite(const char* data, uint64 size) = 0;

				/// \brief try to open file or close it. depend on specified mustBeOpen flag, file
				/// will be opened or closed
				///
				/// \param openMode Type of opening file
				/// \param mustBeOpen Indicate whether file must be opened or closed that if it has
				/// true value, file will be opened otherwise will be closed
				///
				/// \return If requested changed state is successfull, return true otherwise return false
				virtual bool changeState(OpenModes openMode	  = OpenMode::None,
										 bool	   mustBeOpen = false)
					= 0;
			};
		} // namespace IO
	}	  // namespace Core
} // namespace DGE
