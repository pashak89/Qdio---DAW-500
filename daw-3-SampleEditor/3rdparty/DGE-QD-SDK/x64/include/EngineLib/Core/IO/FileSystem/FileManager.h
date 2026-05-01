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
 * FileManager header file is using for managing accessing to the file system.
 */
#pragma once

#include <Core/IO/FileSystem/File.h>
#include <Core/IO/FileSystem/FileProvider.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <memory>
#include <type_traits>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace FileSystem
			{
				/// \brief This class is FileManager sub-system. this class use for
				/// create,remove,rename,... on file or directory.
				class DGE_CoreAPI FileManager
					: public Core::System::DSubsystem
					, public Core::Utility::Singleton<FileManager>
				{
					D_OBJECT_SINGLETON(FileManager)
					// File *m_File;

					/// \brief Constructor of FileManager sub-module
					///
					/// \details The Engine register an object of this class automatically.
					///
					/// \param context shared pointer of engine context
					FileManager(DGE::Core::System::ContextRef context
								= DGE::Core::System::Context::getInstance());

				public:
					D_Destructor(FileManager);
					// IInitialize interface
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

					// IRelease interface
					virtual bool release() override;

					AbstractFileRef getFileResource(const DString& filePath);
					AbstractFileRef getFileResource(DString&& filePath);

					template<class StreamType>
						requires std::is_convertible_v<StreamType*,
													   Core::IO::FileSystem::AbstractFile*>
					bool registerFileProvider(
						const DString prefix,
						std::shared_ptr<Core::IO::FileSystem::FileProvider<StreamType>>
							fileProvider)
					{
						if (m_fileProviders.contains(prefix) || !fileProvider)
							return false;
						m_fileProviders.insert(
							{prefix, std::static_pointer_cast<Core::System::AbstractDObjectFactory>(
										 fileProvider)});
						return true;
					}

					/// \brief Search for <FileName> in Special Path
					std::shared_ptr<File> searchFile(const DChar* FileName);

					/// \brief Search for <FileName> in <PathSearch>
					std::shared_ptr<File> searchFile(const DChar* FileName, const DChar* PathSearch);

					/// \brief Indicate that is FileName existed or not.
					static bool isExist(const DString& FileName);

					/// \brief Change Name of File form <OldName> to <NewName>
					static bool renameFile(const DChar* OldName, const DChar* NewName);

					/// \brief Change Name of File form <OldName> to <NewName>
					static bool renameFile(const DString& OldName, const DString& NewName);
#undef DeleteFile
					/// \brief Delete a file from path in <FileName>
					static bool deleteFile(const DChar* FileName);

					/// \brief Create a Directory With <DirectoryName>
					static bool createDirectory(const DChar* DirectoryName);

					/// \brief Delete a Directory from path in <DirectoryName>
					static bool deleteDirectory(const DChar* DirectoryName);

					/// \brief Change Name of Directory form <OldName> to <NewName>
					static bool renameDirectory(const DChar* OldName, const DChar* NewName);

				protected:
					virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

				protected:
					std::map<DString, std::shared_ptr<Core::System::AbstractDObjectFactory>>
						m_fileProviders;
				};
				D_DefRefType(FileManager);
			} // namespace FileSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
