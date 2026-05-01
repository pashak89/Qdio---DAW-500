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
#include <GFX/GCore/InputParameters.h>
#include <any>
#include <atomic>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief An abstract class and interface that use it in rendering pipeline
				class DGE_GFXAPI IPreProcessRender
				{
				public:
					/// \brief Call this function before main process rendering
					///
					/// \param input input parameter of rendering process
					///
					/// \return Return true if preprocess rendering  is successful else return false
					virtual bool preProcessRender(InputParameters &input) = 0;
				};

			} // namespace Interfaces
		}	  // namespace Utility
	}		  // namespace GFX
} // namespace DGE
