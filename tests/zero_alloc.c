#include <arena/arena.h>

typedef struct {
    int id;
    const char *name;
} Task;

int main(void)
{
    Arena *arena;

    arena = arena_create(0, "Zero alloc arena");

    Task *t = arena_malloc(arena, sizeof(Task));
    /* This should and does return an error. */
    if (NULL == t) {
        return 1;
    }

    arena_destroy(arena);

    return 0;
}
