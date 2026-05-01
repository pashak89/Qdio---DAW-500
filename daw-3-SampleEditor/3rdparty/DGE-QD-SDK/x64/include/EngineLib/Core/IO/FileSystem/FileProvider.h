#pragma once
#include <Core/IO/IIOStream.h>
#include <Core/System/DObjectFactory.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace FileSystem
			{
				using namespace Core::Utility::Interfaces;

				template<class StreamClass>
					requires std::is_convertible_v<StreamClass*, Core::IO::IIOStream*>
				class FileProvider : public System::DObjectFactory<StreamClass>
				{
				public:
					FileProvider()
						: System::DObjectFactory<StreamClass>()
					{
					}
				};
			} // namespace FileSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
