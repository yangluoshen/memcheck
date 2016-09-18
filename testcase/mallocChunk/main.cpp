#include <stdio.h>
#include <cstring>
#include <malloc.h>

const size_t SIZE_SZ = sizeof(size_t);
struct malloc_chunk
{
    size_t prev_size;
    size_t size; 
    struct malloc_chunk* fd;
    struct malloc_chunk* bk;

    struct malloc_chunk* fd_nextsize;
    struct malloc_chunk* bk_nextsize;
};

typedef struct malloc_chunk* mchunk_ptr;

#define mem2chunk(mem) ((mchunk_ptr) ((char*)(mem) - 2 * SIZE_SZ))
#define chunk2mem(p) ((void*)((char*)(p) + 2 * SIZE_SZ))

int main(void)
{
    //printf("sizeof(size_t) = %lu\n", sizeof(size_t));
    //printf("sizeof(chunk) = %lu\n", sizeof(struct malloc_chunk));

    size_t* mem_front = (size_t*) malloc(SIZE_SZ * 5);
    size_t* mem_back = (size_t*) malloc(SIZE_SZ * 5);
    if (!mem_front || !mem_back)
    {
        printf("mem is null\n");
        return -1;
    }
    
    mchunk_ptr pchunk_front = mem2chunk(mem_front);
    mchunk_ptr pchunk_back = mem2chunk(mem_back);

    if (!pchunk_front || !pchunk_back)
    {
        printf("pchunk is null\n");
        return -1;
    }

    memset(mem_front, 0xff, SIZE_SZ * 5);
    
    printf("pchunk_front->size:%lu\n", pchunk_front->size);
    printf("pchunk_back->size :%lu\n", pchunk_back->size);

    printf("pchunk_back - pchunk_front = %lu\n", (size_t)pchunk_back - (size_t)pchunk_front);
    printf("pchunk_back - mem_front = %lu\n", (size_t)pchunk_back - (size_t)mem_front);

    printf("pchunk_back->prev_size=%d\n", (int)pchunk_back->prev_size);

    return 0;
}
