#ifndef ARENA_H_
#define ARENA_H_

#include <stdlib.h>

typedef struct Arena Arena;
typedef size_t ArenaMarker;

Arena *arena_init(size_t capacity, const char *name);
void arena_free(Arena *arena);
void arena_reset(Arena *arena);

void *arena_malloc(Arena *arena, size_t size);
void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment);

size_t arena_get_size(const Arena *arena);
size_t arena_get_available(const Arena *arena);
size_t arena_get_capacity(const Arena *arena);
void *arena_top(const Arena *arena);
#ifndef NDEBUG
const char *arena_get_name(const Arena *arena);
void arena_print(const Arena *arena);
#endif /* NDEBUG */
    
ArenaMarker arena_mark(Arena *arena);
void arena_rewind(Arena *arena, ArenaMarker marker);

Arena *arena_create_subarena(Arena *parent, size_t capacity, const char *name);

#endif /* ARENA_H_ */
