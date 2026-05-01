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
#include "IUpdate.h"
#include <GFX/GCore/Interfaces/IPostProcessRender.h>
#include <GFX/GCore/Interfaces/IPreProcessRender.h>
#include <GFX/GCore/Interfaces/IRender.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief Present minimum function required for render pipeline that call in every frame
				class DGE_GFXAPI IRenderPipeline
					: public IPreProcessRender
					, public IRender
					, public IPostProcessRender
					, public IUpdate
				{
				public:

					// IUpdate interface
					/// \brief Call when engine is updated
					using GFX::GCore::Interfaces::IUpdate::update;

					// IPreRender interface
					/// \brief call this function before main function of rendering render(const
					/// std::any &input)
					using GFX::GCore::Interfaces::IPreProcessRender::preProcessRender;

					// IRender interface
					/// \brief call when process of rendering started and must this object
					/// complete own rendering process
					using GFX::GCore::Interfaces::IRender::render;

					// IPostRender interface
					/// \brief call this function after main function of rendering render(const
					/// std::any &input)
					using GFX::GCore::Interfaces::IPostProcessRender::postProcessRender;
				};

			} // namespace Interfaces
		}	  // namespace Utility
	}		  // namespace GFX
} // namespace DGE
