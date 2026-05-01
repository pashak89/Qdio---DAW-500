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
 * File header file is using for accessing to a file contents.
 */
#pragma once

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/FileSystem/AbstractFile.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace FileSystem
			{
				using namespace Core::Utility::Interfaces;
				class FileManager;

				/// \brief The File class is used for reading from and writing to file.
				class DGE_CoreAPI File : public Core::IO::FileSystem::AbstractFile
				{
					friend class FileManager;
					D_OBJECT(File)

				public:
					/// \brief Create a File class that it can read from or write to file.
					/// \param fileName name of file
					File(DGE::Core::System::ContextRef context
						 = DGE::Core::System::Context::getInstance());
					File(DString&&					   fileName,
						 DGE::Core::System::ContextRef context
						 = DGE::Core::System::Context::getInstance());
					File(const DString&				   fileName,
						 DGE::Core::System::ContextRef context
						 = DGE::Core::System::Context::getInstance());

					D_Destructor(File);

					// IReader interface
					/// \brief read whole bytes from file and return them.
					///
					/// \return return all bytes that read from file.
					virtual const DByteArray readAll() override;
					virtual DByteArray		 read(uint64 size) override;
					virtual uint64			 read(char* data, uint64 maxSize) override;

					/// \brief write to file that was opened
					/// \param data the data that want to write to file
					///
					/// \return if was writing into file is successful return true else false.
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

					/// \brief Indicate whether file has been opened or not.
					///
					/// \return return true if file had been opened.
					virtual bool hasOpened() override;

					/// \brief return current size of file
					///
					/// \return if file is opened return size of file else return 0.
					uint64_t size();
					// IIOStream interface
				protected:
					virtual DByteArray doRead(uint64 maxSize = 0) override;
					virtual uint64	   doWrite(const char* data, uint64 size) override;
					virtual bool	   changeState(OpenModes openMode, bool mustBeOpen) override;

				protected:
					/// \brief C++ file strean object
					std::fstream m_hFile;

					bool m_hasSearch; // if true show Create with search method.
				};
			} // namespace FileSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
