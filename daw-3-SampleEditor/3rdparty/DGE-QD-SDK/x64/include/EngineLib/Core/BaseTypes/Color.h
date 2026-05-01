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
 * Color class is using for storing information about a color
 */
#pragma once
#include "Core/CoreModuleDef.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Math/Vector4D.h>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			/// \brief store data of color in this class
			struct DGE_CoreAPI Color
			{
				/// \brief red color
				/// \details this is red color that between 0-255
				uint8 r;
				/// \brief green color
				/// \details this is green color that between 0-255
				uint8 g;
				/// \brief blue color
				/// \details this is blue color that between 0-255
				uint8 b;
				/// \brief alpha color
				/// \details this is alpha color that between 0-255
				uint8 a;

                static inline const int16 s_minValue = 0;
                static inline const int16 s_maxValue = 255;

				/// \brief construct this class that can store red,green.blue and alpha with
				/// specified data type
				Color();
				template<typename Type = int16>
				Color(Type red, Type green, Type blue, Type alpha = s_maxValue)
				{
					r = std::clamp<int16>((red <= 1 && red >= 0) ? red * s_maxValue : red,
										  s_minValue, s_maxValue);
					g = std::clamp<int16>((green <= 1 && green >= 0) ? green * s_maxValue : green,
										  s_minValue, s_maxValue);
					b = std::clamp<int16>((blue <= 1 && blue >= 0) ? blue * s_maxValue : blue,
										  s_minValue, s_maxValue);
					a = std::clamp<int16>((alpha <= 1 && alpha >= 0) ? alpha * s_maxValue : alpha,
										  s_minValue, s_maxValue);
				}
				/// \brief Convert string of color code into Color structure
				Color(const DString& colorCode);
				Color(DString&& colorCode);

				bool operator==(const Color& color) const;

				Math::Vector4D toVec4(float scale = 0.00390625f) const;
				DString		   toString(bool withHead = true) const;

				Color convertToColor(DString colorCode);

				/// \brief Red color value
				static const Color s_red;
				/// \brief Green color value
				static const Color s_green;
				/// \brief Blue color value
				static const Color s_blue;

				/// \brief Cyan color value
				static const Color s_cyan;
				/// \brief Magenta color value
				static const Color s_magenta;
				/// \brief Yellow color value
				static const Color s_yellow;

				/// \brief Orange color value
				static const Color s_orange;
				/// \brief Gold color value
				static const Color s_gold;
				/// \brief Corn Flower Blue color value
				static const Color s_cornFlowerBlue;
			};
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
