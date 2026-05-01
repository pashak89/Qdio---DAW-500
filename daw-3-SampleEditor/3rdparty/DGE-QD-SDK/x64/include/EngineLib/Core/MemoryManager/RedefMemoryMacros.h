#include <Core/BaseTypes/BaseTypes.h>
#include <Core/MemoryManager/MemoryManager.h>

#ifdef DGE_Track_Memory 
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo) 
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo) 
#elif defined(DGE_UseCRTDebugMemory) 
#define _New DBG_NEW 
#define new DBG_NEW 
#endif