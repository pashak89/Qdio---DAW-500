#include "mixhelpers.h"
namespace MixHelpers {
static bool s_NaNHandler;

bool useNaNHandler()
{
    return s_NaNHandler;
}

void setNaNHandler(bool use)
{
    s_NaNHandler = use;
}
/*! \brief Function for sanitizing a buffer of infs/nans - returns true if those are found */
bool sanitize(sampleFrame* src, int frames)
{
    if (!useNaNHandler()) {
        return false;
    }

    bool found = false;
    for (int f = 0; f < frames; ++f) {
        for (int c = 0; c < src->getNumChannels(); ++c) {
            if (std::isinf(src->getSample(c, f)) || std::isnan(src->getSample(c, f))) {

                for (int f = 0; f < frames; ++f) {
                    for (int c = 0; c < 2; ++c) {
                        src->setSample(c, f, 0.0f);
                    }
                }
                found = true;
                return found;
            } else {

                src->setSample(c, f, qBound(-1000.0f, src->getSample(c, f), 1000.0f));
            }
        }
    }
    return found;
}
}

// namespace MixHelpers {

// struct AddOp {
//     void operator()(sampleFrame& dst, const sampleFrame& src) const
//     {
//         for (int i = 0; i < dst.channels(); i++) {
//            // dst.set(i, 0, dst.get(i, 0) + src.get(i, 0));
//         }
//     }
// };

///*! \brief Function for applying MIXOP on all sample frames */
// template <typename MIXOP>
// static inline void run(sampleFrame* dst, const sampleFrame* src, int frames, const MIXOP& OP)
//{
//     for (int i = 0; i < frames; ++i) {
//         OP(dst[i], src[i]);
//     }
// }

// void add(sampleFrame* dst, const sampleFrame* src, int frames)
//{
//      //run<>(dst, src, frames, AddOp());

//    for (int i = 0; i < frames; ++i) {
//        for (int j = 0; j < dst->channels(); j++) {

//            float value =  dst->get(j, i) + src->get(j, i);
//            dst->set(j, i, value);
//        }
//    }
//}
//}
