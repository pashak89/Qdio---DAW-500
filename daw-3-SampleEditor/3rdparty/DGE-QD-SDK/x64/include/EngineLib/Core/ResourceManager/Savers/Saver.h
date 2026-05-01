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
 *
 */
#pragma once

#include "Core/System/DObjectMacrosDef.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Math/Math.h>
#include <Core/ResourceManager/Savers/ISave.h>
#include <Core/System/DCoreObject.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			namespace Savers
			{

				/// \brief Template base and abstract class that use for implement different type of
				/// saver object
				class DGE_CoreAPI Saver
					: public System::DCoreObject
					, public ISave
				{
					D_CORE_OBJECT(Saver)
				public:
					bool isUsingEnvironmentVariable() const;
					void setIsUsingEnvironmentVariable(bool newIsUsingEV);

					DEnV environmentVariable() const;
					void setEnvironmentVariable(const DEnV& newTemplatePath);

					DOption option() const;
					void	setOption(const DOption& newOption);

				protected:
					/// \brief Store specified data in data variable in the specified file in
					/// full path variable and
					bool storeFile(DString fullPath, DByteArray data);

					/// \brief Return value of the specified option if it is existed
					bool optionValue(DString name, DString& value) const;

				protected:
					/// \brief Indicate whether enable summarizing path of resource with using
					/// environment variable to prevent from getting
					/// error when moving the resources files in almost cases. By Default this
					/// feature is enabled.
					bool m_isUsingEnvironmentVariable;

					/// \brief Information about apply environment variable on every inner path of resources
					DEnV m_environmentVariable;

					/// \brief the options that will be used for saving the resources
					mutable DOption m_option;
				};
				D_DefRefType(Saver);
			} // namespace Savers
		}	  // namespace Resource
	}		  // namespace Core
} // namespace DGE
