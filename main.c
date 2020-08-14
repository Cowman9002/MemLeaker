#include <malloc.h>
#include <stdio.h>

typedef struct
{
    unsigned line;      // line memory was allocated on
    unsigned bytes;     // size in bytes of memory allocated
    const char *file;   // file memory was allocated in
    void *ptr;          // the memory that was allocated
}MemoryData;

static unsigned s_allocated = 0;                // counter for bytes currently allocated
static MemoryData *s_allocated_bytes = NULL;    // array of memory data allocated
static unsigned s_num_allocations = 0;          // number of allocations, size counter for s_allocated_bytes

void *mallocDebugInternal(unsigned bytes, const char* file, unsigned line)
{
    s_allocated += bytes;
    s_num_allocations++;

    void *ptr = malloc(bytes);

    // increase array size to take in more data
    s_allocated_bytes = realloc(s_allocated_bytes, s_num_allocations * sizeof(*s_allocated_bytes));

    // fill in new struct
    MemoryData new_memdata;
    new_memdata.bytes = bytes;
    new_memdata.file = file;
    new_memdata.line = line;
    new_memdata.ptr = ptr;

    // keep track of new malloc
    s_allocated_bytes[s_num_allocations - 1] = new_memdata;

    printf("+Add %u bytes %p at file %s, line %u\n\n", bytes, ptr, file, line);

    return ptr;
}

void *reallocDebugInternal(void *old_ptr, unsigned bytes, const char* file, unsigned line)
{
    void *ptr = NULL;

    // look to see if ptr has been allocated
    for(int i = 0; i < s_num_allocations; i++)
    {
        if(s_allocated_bytes[i].ptr == old_ptr) // found the memory data
        {
            // change allocation to fit true allocation
            s_allocated += bytes - s_allocated_bytes[i].bytes;

            ptr = realloc(old_ptr, bytes);

            printf("+Rek %u bytes %p at file %s, line %u\n", bytes, ptr, file, line);
            printf("\tAllocated in file %s, line %u\n\n", s_allocated_bytes[i].file, s_allocated_bytes[i].line);

            // change array index to fit reallocation
            s_allocated_bytes[i].bytes = bytes;
            s_allocated_bytes[i].file = file;
            s_allocated_bytes[i].line = line;
            s_allocated_bytes[i].ptr = ptr;

            return ptr;
        }
    }

    // preform the same actions as malloc
    s_allocated += bytes;
    s_num_allocations++;

    ptr = realloc(old_ptr, bytes);

    s_allocated_bytes = realloc(s_allocated_bytes, s_num_allocations * sizeof(*s_allocated_bytes));

    MemoryData new_memdata;
    new_memdata.bytes = bytes;
    new_memdata.file = file;
    new_memdata.line = line;
    new_memdata.ptr = ptr;

    s_allocated_bytes[s_num_allocations - 1] = new_memdata;

    printf("+Rek %u bytes %p at file %s, line %u\n\n", bytes, ptr, file, line);

    return ptr;
}

void freeDebugInternal(void *ptr, const char* file, unsigned line)
{
    // look for allocated memeory
    for(int i = 0; i < s_num_allocations; i++)
    {
        if(s_allocated_bytes[i].ptr == ptr) // found the memory data
        {
            s_allocated -= s_allocated_bytes[i].bytes;
            printf("-Sub %u bytes %p at file %s, line %u\n",
                   s_allocated_bytes[i].bytes,
                   s_allocated_bytes[i].ptr,
                   file,
                   line);
            printf("\tAllocated in file %s, line %u\n\n", s_allocated_bytes[i].file, s_allocated_bytes[i].line);

            s_num_allocations--; // this must be done before switching but must happen always
            if(s_num_allocations > 0) // make sure there is more than one element in the array
            {
                /* change the memory to be removed with the last memory on the
                    array, removing it and shortening the array at the same time*/
                s_allocated_bytes[i] = s_allocated_bytes[s_num_allocations];
            }
            break;
        }
    }

    free(ptr);
}

void printDebugMemoryInternal()
{
    if(s_allocated)
    {
        printf("%u bytes not freed!\n", s_allocated);
        for(int i = 0; i < s_num_allocations; i++)
        {
            printf("\t%u bytes of memory %p from file %s, line %u not freed!\n",
                   s_allocated_bytes[i].bytes,
                   s_allocated_bytes[i].ptr,
                   s_allocated_bytes[i].file,
                   s_allocated_bytes[i].line);
        }
    }
    else
        printf("All allocated bytes freed.\n");
}
