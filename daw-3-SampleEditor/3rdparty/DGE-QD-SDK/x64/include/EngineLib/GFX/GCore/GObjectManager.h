#pragma once

#include <Core/System/DSubsystem.h>
#include <GFX/GCore/Interfaces/IGpuBuffer.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Graphic objects manager subsystem that is managing graphic objects
			class GObjectManager
				: public DGE::Core::System::DSubsystem
				, public Core::Utility::Singleton<GObjectManager>
			{
				D_OBJECT_SINGLETON(GObjectManager)
				GObjectManager(DGE::Core::System::ContextRef context);

			public:
				D_Destructor(GResourceManager);

				// IInitialize interface
				using Core::System::DSubsystem::initialize;

				/// \brief Initialize itself with getting current Renderer subsystem
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				/// \brief Add graphic object with specified id to manage duplicate graphic objects
				template<typename GObject>
				bool addGObject(DString id, std::shared_ptr<GObject> object)
				{
					return registerGObject(id, object);
				}

				/// \brief Add graphic object with specified id to manage duplicate graphic objects
				template<typename GObject>
				bool addGObject(GObject object)
				{
					auto&  typeInfo = typeid(GObject);
					DString typeName = typeInfo.name();
					return addGObject(typeName, object);
				}

				/// \brief Retrieve graphic object with specified id
				template<typename GObject>
				std::shared_ptr<GObject> graphicObject(DString id)
				{
					return graphicObject(id);
				}

			protected:
				bool registerGObject(DString id, Interfaces::IGpuBufferRef object);
				Interfaces::IGpuBufferRef graphicObject(DString id);

				// DSubsystem interface
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			protected:
				std::map<DString, Interfaces::IGpuBufferRef> m_graphicObjectsList;
			};

		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
