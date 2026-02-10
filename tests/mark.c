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

    t1->id = 0;
    t1->context = "1st task";

    ArenaMarker mark = arena_mark(arena);

    Task *tmp = (Task *)arena_malloc(arena, sizeof(Task));

    for (int i = 0; i < 100; ++i) {
        tmp->id++;
    }

    arena_rewind(arena, mark);

    tmp->id++;

    arena_destroy(arena);

    return 0;
}
