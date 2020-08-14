#ifndef MEM_LEAKER_H
#define MEM_LEAKER_H

#ifdef __DEBUG_MEM
void *mallocDebugInternal(unsigned bytes, const char* file, unsigned line);
void *reallocDebugInternal(void *ptr, unsigned bytes, const char* file, unsigned line);
void freeDebugInternal(void *old_ptr, const char* file, unsigned line);
void printDebugMemoryInternal();

#define malloc(s) mallocDebugInternal(s, __FILE__, __LINE__)
#define realloc(p, s) reallocDebugInternal(p, s, __FILE__, __LINE__)
#define free(s) freeDebugInternal(s, __FILE__, __LINE__)
#define printMemUsage() printDebugMemoryInternal()
#else
#include <malloc.h>
#define printMemUsage()
#endif // __DEBUG_MEM

#endif // MEM_LEAKER_H
