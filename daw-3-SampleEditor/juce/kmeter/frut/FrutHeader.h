/* ----------------------------------------------------------------------------

   FrutJUCE
   ========
   Common classes for use with the JUCE library

   Copyright (c) 2010-2020 Martin Zuther (http://www.mzuther.de/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Thank you for using free software!

---------------------------------------------------------------------------- */

#ifndef FRUT_HEADER_H
#define FRUT_HEADER_H

// external includes
#include "JuceLibraryCode/JuceHeader.h"
#define M_PI 3.14159265358979323846 /* pi */
using namespace juce;

#define FRUT_DSP_USE_FFTW 1

#ifndef FRUT_DSP_USE_FFTW
#define FRUT_DSP_USE_FFTW 0
#endif

namespace frut {

/// Simple helper class.
///
class Frut {
public:
    /// Get version number of Frut common classes.
    ///
    /// @return version number
    ///
    static const juce::String getVersion()
    {
        return "2.0.4";
    }

    static void printVersionNumbers()
    {

        String simd = "SIMD ";

        if (juce::SystemStats::hasSSE()) {
            simd += "SSE ";
        }

        if (juce::SystemStats::hasSSE2()) {
            simd += "SSE2 ";
        }

        if (juce::SystemStats::hasAVX()) {
            simd += "AVX ";
        }

        if (juce::SystemStats::hasAVX2()) {
            simd += "AVX2 ";
        }
    }
};
}

// normal includes
#include "amalgamated/include_frut_audio.h"
#include "amalgamated/include_frut_dsp.h"
#include "amalgamated/include_frut_math.h"
#include "amalgamated/include_frut_parameters.h"
#include "amalgamated/include_frut_widgets.h"

// post includes
#include "amalgamated/include_frut_skin.h"

#endif // FRUT_HEADER_H
