#include <arena/arena.h>

#include <stdio.h>
#include <stdint.h>

typedef struct {
    int id;
    float x, y;
    const char *context;
} Task;

int main(void)
{
    Arena *arena;

    arena = arena_create(64 * 1024, "alignement test.");

    Task *t1 = arena_aligned_malloc(arena, sizeof(Task), _Alignof(Task));
    Task *t2 = arena_aligned_malloc(arena, sizeof(Task), _Alignof(Task));

    printf("the address of t1 is: %p\n", (void *)t1);
    printf("the address of t2 is: %p\n", (void *)t2);

    /* Perfectly aligned */
    printf("The difference between the pointers is: %lu\n",
           (uintptr_t)t2 - (uintptr_t)t1);

    arena_destroy(arena);
    return 0;
}
