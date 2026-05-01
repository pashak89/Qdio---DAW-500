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
#include "../Core.h"
using namespace std;

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Use for storing version number and use it
			class DGE_CoreAPI Version
			{
				uint8 m_version;
				uint8 m_major;
				uint8 m_minor;

			public:
				Version(uint32 v = 0, uint32 ma = 0, uint32 mi = 0);
                Version(const string &versionStr);
#ifdef Q_OBJECT
                Version(const QString &versionStr);
#endif
                /// \brief Get string of version data that later can convert to Version with
                /// Version::fromString(string)
                string	toString(const string& delimiter = ".");
#ifdef Q_OBJECT
                QString toQString(const QString &delimiter = ".");
#endif
                void	fromString(string versionStr);

				bool operator>(Version v);
				bool operator<(Version v);
				bool operator==(Version obj);
				bool operator!=(Version obj);

				/// \brief Generate build number base on version, major, minor data.
				uint32 getBuildNumber();
			};
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
