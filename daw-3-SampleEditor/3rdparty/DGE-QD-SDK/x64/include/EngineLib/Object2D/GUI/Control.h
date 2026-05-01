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
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>

#include <Engine/Core/IO/Events.h>
#include <Engine/Object2D/2DObject.h>

namespace DGE
{
	using namespace DGE::Math;
	using namespace DGE::IO;
	using namespace DGE::GFX;

	namespace Objects2D
	{
		class _2DObjectsManager;
		namespace GUI
		{
#if defined(DGE_UseGraphicAPI)
			//Base Class Of GUI Controls
			class DGE_API Control :public _2DObject
			{
				friend class Objects2D::_2DObjectsManager;
			protected:

				vector<EventType> m_vEventTypes;//Event Types.
				vector<Core::System::Function<void, Control*, EventArgs*>*>
					m_vFunctors; // Event Functions.

			public:
				
				virtual ~Control(){}

				//Connect a Event To Control.
				virtual void Connect(EventType											 eventtype,
									 Core::System::Function<void, Control*, EventArgs*>* Function);
			};
#endif
		}
	}
}
