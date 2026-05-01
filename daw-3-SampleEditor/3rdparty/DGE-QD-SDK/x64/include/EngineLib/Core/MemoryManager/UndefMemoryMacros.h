#include <Core/BaseTypes/BaseTypes.h>

#if defined(DGE_Track_Memory) || defined(DGE_UseCRTDebugMemory) 
#undef new 
#undef delete 
#endif