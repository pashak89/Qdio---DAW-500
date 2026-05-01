#pragma once

#include <Core/Utility/Singleton/SingletonMacrosDef.h>
#include <QFile>
#include <Qt/QtModuleDef.h>

#include <Core/IO/FileSystem/AbstractFile.h>

namespace DGE
{
	namespace Qt
	{
		namespace QtDCore
		{
			namespace IO
			{
				/// \brief Qt File reader wrapper is used for loading "qrc" resources.
				class DGE_QtAPI QtDFile : public DGE::Core::IO::FileSystem::AbstractFile
				{
					D_OBJECT(QtDFile)
				public:
					QtDFile(DGE::Core::System::ContextRef context
							= DGE::Core::System::Context::getInstance());
					QtDFile(DString&&					  fileName,
							DGE::Core::System::ContextRef context
							= DGE::Core::System::Context::getInstance());
					QtDFile(const DString&				  fileName,
							DGE::Core::System::ContextRef context
							= DGE::Core::System::Context::getInstance());
					D_Destructor(QtDFile);

					static QIODevice::OpenMode toQOpenModeFlag(DGE::Core::IO::OpenModes openMode);

					// IStream interface
				public:
					virtual bool hasOpened() override;

					// IIOStream interface
				protected:
					virtual DByteArray doRead(uint64 maxSize) override;
					virtual uint64	   doWrite(const char* data, uint64 size) override;
					virtual bool	   changeState(Core::Utility::Interfaces::OpenModes openMode,
												   bool mustBeOpen) override;

				protected:
					QFile m_file;
				};

				using QtDFileRef = std::shared_ptr<QtDFile>;
			} // namespace IO
		}	  // namespace QtDCore
	}		  // namespace Qt
} // namespace DGE
