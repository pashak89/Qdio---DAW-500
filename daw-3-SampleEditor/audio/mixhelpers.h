#ifndef MIXHELPERS_H
#define MIXHELPERS_H
#include "audio_global.h"
namespace MixHelpers {

///*! \brief Add samples from src to dst */
// void add(sampleFrame* dst, const sampleFrame* src, int frames);

//}
bool sanitize(sampleFrame* src, int frames);
bool useNaNHandler();

void setNaNHandler(bool use);
}
#endif // MIXHELPERS_H
