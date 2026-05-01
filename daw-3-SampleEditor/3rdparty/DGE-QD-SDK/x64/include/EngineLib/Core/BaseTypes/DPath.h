#pragma once

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/DEnv.h>
#include <Core/CoreModuleDef.h>
#include <regex>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			class DGE_CoreAPI DPath
			{
			public:
				DPath(DString path = "");
				explicit DPath(const DString &filename, const DString &path);

				operator DString() const;

				static const DPath toFullPathFileName(const DString &filename, const DString &path);
				static const DString toFullPath(const DString &filename, const DString &path);

				/// \brief Normalize path depend on OS and specified path and return it.
				///
				/// \param path It's path that want to be normalized
				///
				/// \return Return normalized path
				static const DString normalizePath(const DString &path);

				const DString toString() const;

				/// \brief Return file name in this path string
				const DString fileName() const;
				/// \brief Return only path to file that doesn't include filename
				const DString path() const;

				bool operator==(const DPath &) const;

			public:
				static const DString		s_sepratorPath;
				static const std::regex s_regexNormalize;

				DString m_dataString;
			};
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
