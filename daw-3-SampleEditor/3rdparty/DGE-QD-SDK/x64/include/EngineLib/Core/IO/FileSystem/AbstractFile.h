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
 * Abstract File header file is using for accessing to a file contents.
 */
#pragma once

#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/IIOStream.h>
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
				class DGE_CoreAPI AbstractFile
					: public Core::System::DObject
					, public Core::IO::IIOStream
				{
					friend class FileManager;
					D_OBJECT(AbstractFile)

				public:
					/// \brief Create a File class that it can read from or write to file.
					/// \param fileName name of file
					AbstractFile(DGE::Core::System::ContextRef context
								 = DGE::Core::System::Context::getInstance());
					AbstractFile(DString&&					   fileName,
								 DGE::Core::System::ContextRef context
								 = DGE::Core::System::Context::getInstance());
					AbstractFile(const DString& fileName, DGE::Core::System::ContextRef context);

					D_Destructor(AbstractFile);

					using Core::IO::IIOStream::open;
					bool open(const DString& fileName, OpenModes openMode = OpenMode::ReadOnly);
					bool open(DString&& fileName, OpenModes openMode = OpenMode::ReadOnly);

					/// \brief return name of file
					const DString fileName();

					/// \brief Set full file name
					void setFileName(const DString& newFileName);

				protected:
					/// \brief name of file that must be open
					DString m_fileName;
				};
				using AbstractFileRef  = std::shared_ptr<Core::IO::FileSystem::AbstractFile>;
				using AbstractFileWRef = std::weak_ptr<Core::IO::FileSystem::AbstractFile>;
			} // namespace FileSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
