#include <stdint.h>
#include <stdio.h>
#include <windows.h>

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

typedef enum
{
    FAILURE = 0,
    SUCCESS = 1
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

AllocationResult Allocate(Allocator *allocator, usize size)
{
    u8 *chunk = allocator->memory + allocator->used;
    allocator->used += size;

    AllocationResult result = {.memory = chunk, .result = SUCCESS};
    return result;
}

typedef struct
{
    Result result;
    ArrayList *list;
} ArrayListInitResult;

ArrayListInitResult ArrayList_Init(Allocator *allocator)
{
    ArrayListInitResult result;
    AllocationResult allocResult = Allocate(allocator, sizeof(ArrayList) + 10);
    if (allocResult.result == FAILURE)
    {
        // panic
        result.result = FAILURE;
        result.list = 0;
        return result;
    }

    result.list = (ArrayList *)allocResult.memory;
    result.list->capacity = 10;
    result.list->len = 0;
    result.list->items = allocResult.memory + sizeof(ArrayList);

    return result;
}

Result ArrayList_Resize(Allocator *allocator, ArrayList *al)
{
    AllocationResult allocResult = Allocate(allocator, al->capacity * 2);
    if (allocResult.result == FAILURE)
    {
        return FAILURE;
    }

    for (usize i = 0; i < al->len; i++)
    {
        allocResult.memory[i] = al->items[i];
    }

    al->items = allocResult.memory;
    al->capacity *= 2;

    return SUCCESS;
}

Result ArrayList_Insert(Allocator *allocator, ArrayList *al, u8 item, usize idx)
{
    if (al->len + 1 > al->capacity)
    {
        ArrayList_Resize(allocator, al);
    }

    al->items[idx] = item;
    al->len++;

    return SUCCESS;
}

Result ArrayList_Add(Allocator *allocator, ArrayList *al, u8 item)
{
    return ArrayList_Insert(allocator, al, item, al->len);
}

u8 ArrayList_GetItemAt(ArrayList *al, usize idx)
{
    u8 ret_item;
    ret_item = al->items[idx];

    return ret_item;
}

typedef struct
{
    usize limit;
    ArrayList *items;
} PriorityQueue;

typedef struct
{
    PriorityQueue *pq;
    Result result;
} PriorityQueueInitResult;

PriorityQueueInitResult PriorityQueue_Init(Allocator *allocator, usize limit)
{
    PriorityQueueInitResult result;
    AllocationResult allocResult = Allocate(allocator, sizeof(PriorityQueue) + limit);
    if (allocResult.result == FAILURE)
    {
        // panic
        result.result = FAILURE;
        result.pq = 0;
        return result;
    }

    result.pq = (PriorityQueue *)allocResult.memory;

    ArrayListInitResult arrayListResult = ArrayList_Init(allocator);
    if (arrayListResult.result == FAILURE)
    {
        // panic
        result.result = FAILURE;
        result.pq = 0;
        return result;
    }

    result.pq->limit = limit;
    result.pq->items = arrayListResult.list;
    // result.pq->items->items = arrayListResult.list->items;
    // result.pq->items->capacity = arrayListResult.list->capacity;
    // result.pq->items->len = arrayListResult.list->len;

    result.result = SUCCESS;
    return result;
}

Result PriorityQueue_Push(Allocator *allocator, PriorityQueue *pq, u8 item)
{
    if (pq->items->len == pq->limit)
    {
        // rank and replace or drop
        if (pq->items->items[pq->items->len] > item)
        {
            return SUCCESS;
        }
        // this "drop through" should properly shift queue so that the lowest-rated entry
        // falls past the limit and is effectively discarded-> see update in additional comment*
    }

    // sort/rank and insert
    usize current_item_idx = pq->items->len;
    while (current_item_idx > 0 && ArrayList_GetItemAt(pq->items, current_item_idx) < item)
    {
        current_item_idx--;
    }

    if (current_item_idx == pq->items->len)
    {
        if (ArrayList_GetItemAt(pq->items, current_item_idx) < item)
        {
            pq->items->items[current_item_idx] = item;
        }

        return SUCCESS;
    }

    ArrayList_Insert(allocator, pq->items, item, current_item_idx);
    pq->items->len--;

    return SUCCESS;
}

u8 PriorityQueue_Peek(PriorityQueue *pq)
{
    // bounds check
    return pq->items->items[0];
}

int main(void)
{
    u8 *buf = VirtualAlloc(0, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
    Allocator allocator = {.capacity = BUFFER_SIZE, .used = 0, .memory = buf};

    ArrayListInitResult arrayListResult = ArrayList_Init(&allocator);
    ArrayList *list;
    if (arrayListResult.result == FAILURE)
    {
        // log error
        printf("there was an error init arraylist\n");
    }
    else
    {
        list = arrayListResult.list;
    }

    ArrayList_Add(&allocator, list, 3);

    PriorityQueueInitResult pqResult = PriorityQueue_Init(&allocator, 8);
    PriorityQueue *pq;
    if (pqResult.result == FAILURE)
    {
        // log error
        printf("there was an error init priorityqueue\n");
    }
    else
    {
        pq = pqResult.pq;
    }

    PriorityQueue_Push(&allocator, pq, 3);
    PriorityQueue_Push(&allocator, pq, 5);
    PriorityQueue_Push(&allocator, pq, 8);
    PriorityQueue_Push(&allocator, pq, 1);

    printf("Hello, world!\n");
    printf("List item: %d\n", list->items[0]);
    printf("Queue item: %d\n", PriorityQueue_Peek(pq));

    if (!VirtualFree(buf, 4096, MEM_RELEASE))
    {
        // panic
        return (1);
    }
    return (0);
}
