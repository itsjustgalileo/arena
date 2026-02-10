#include <arena/arena.h>

typedef struct {
    int id;
    const char *context;
} Task;

int main(void)
{
    Arena *arena;

    arena = arena_create(64 * 1024, NULL);

    Task *t1 = (Task *)arena_malloc(arena, sizeof(Task));
    Task *t2 = (Task *)arena_malloc(arena, sizeof(Task));
    Task *t3 = (Task *)arena_malloc(arena, sizeof(Task));

    (void)t1;
    (void)t2;
    (void)t3;

    arena_destroy(arena);

    return 0;
}
