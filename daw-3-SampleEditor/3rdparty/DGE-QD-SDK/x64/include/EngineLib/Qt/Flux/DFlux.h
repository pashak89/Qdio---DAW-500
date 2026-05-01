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
#include <QtCore/QObject>

#include <Qt/QtModuleDef.h>
namespace DGE
{
	namespace QtWrapper
	{
		namespace Flux
		{
			/// \brief Base class for implement Flux layer in the Engine
			class DGE_QtAPI DFlux : public QObject
			{
				Q_OBJECT
			public:
				DFlux();

				/// \brief Process the main task of this class and return result of processing the
				/// main task
				///
				/// \param inputParam required input parameter for process the main task of this
				/// class.
				///
				/// \return result of processing the main task
				virtual QVariant process(QVariant inputParam);
			};
		} // namespace Flux
	}	  // namespace QtWrapper
} // namespace DGE
