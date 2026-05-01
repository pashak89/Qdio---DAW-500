#pragma once

#include "UIElement.h"

namespace DGE
{
	namespace UI
	{
		namespace UICore
		{
			/// \brief Basic UI element for show an image
			class DGE_UIAPI UIImage : public DGE::UI::UICore::UIElement
			{
				D_OBJECT(UIImage)
			public:
				UIImage(DGE::Core::System::ContextRef context
						= DGE::Core::System::Context::getInstance());

				D_Destructor(UIImage);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

				// IUpdate interface
				virtual bool update(GFX::GCore::InputParameters &input) override;

				// IRender interface
				virtual bool render(GFX::GCore::InputParameters &input) override;

				// IPreProcessRender interface
				virtual bool preProcessRender(GFX::GCore::InputParameters &input) override;

				// IPostProcessRender interface
				virtual bool postProcessRender(GFX::GCore::InputParameters &input) override;

			protected:
			};

		} // namespace UICore
	}	  // namespace UI
} // namespace DGE
