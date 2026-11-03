#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

typedef unsigned char u1;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint64_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef int64_t isize;

static usize BUFFER_SIZE = 4096;

typedef union {
    u1 Success;
    u1 Failure;
} Result;

typedef struct
{
    u8 *memory;
    Result result;
} AllocationResult;

typedef struct
{
    usize capacity;
    usize used;
    u8 *memory;
} Allocator;

typedef struct
{
    usize capacity;
    usize len;
    u8 *items;
} ArrayList;

AllocationResult Allocate(Allocator allocator, usize size)
{
    u8 *chunk = allocator.memory + allocator.used;
    allocator.used += size;

    AllocationResult result = {.result = {.Success = 1}, .memory = chunk};
    return result;
}

ArrayList *ArrayList_Init(Allocator allocator)
{
    AllocationResult allocResult = Allocate(allocator, sizeof(ArrayList) + 10);
    if (!allocResult.result.Success)
    {
        Result result = {.Failure = 1};
        // panic
    }

    ArrayList *arrayList = (ArrayList *)allocResult.memory;
    arrayList->capacity = 10;
    arrayList->len = 0;
    arrayList->items = allocResult.memory + sizeof(ArrayList);

    return arrayList;
}

Result ArrayList_Resize(Allocator allocator, ArrayList *al)
{
    AllocationResult allocResult = Allocate(allocator, al->capacity * 2);
    if (!allocResult.result.Success)
    {
        Result result = {.Failure = 1};
        return result;
    }

    for (usize i = 0; i < al->len; i++)
    {
        allocResult.memory[i] = al->items[i];
    }

    al->items = allocResult.memory;
    al->capacity *= 2;

    Result result = {.Success = 1};
    return result;
}

Result ArrayList_Add(Allocator allocator, ArrayList *al, u8 item)
{
    if (al->len + 1 > al->capacity)
    {
        ArrayList_Resize(allocator, al);
    }

    al->items[al->len] = item;
    al->len++;

    Result result = {.Success = 1};
    return result;
}

typedef struct
{
    usize limit;
    ArrayList items;
} PriorityQueue;

Result PriorityQueue_Push(Allocator allocator, PriorityQueue *pq, u8 item)
{
    if (pq->items.len == pq->limit)
    {
        if (pq->items.items[pq->items.len] < item)
        {
        }
        // rank and replace or drop
    }

    // sort/rank and insert

    Result result = {.Success = 1};
    return result;
}

int main(void)
{
    u8 *buf = VirtualAlloc(0, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
    Allocator allocator = {.capacity = BUFFER_SIZE, .used = 0, .memory = buf};

    ArrayList *arrayList = ArrayList_Init(allocator);

    ArrayList_Add(allocator, arrayList, 3);

    printf("Hello, world!\n");
    printf("%d\n", arrayList->items[0]);

    if (!VirtualFree(buf, 4096, MEM_RELEASE))
    {
        // panic
        return (1);
    }
    return (0);
}
