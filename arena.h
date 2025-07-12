#ifndef ARENA_H_
#define ARENA_H_

#include <quest/quest_common.h>

#include <quest/quest_header.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct Arena Arena;
    typedef size_t ArenaMarker;

    Arena *arena_init(size_t capacity, const char *name);
    void arena_free(Arena *arena);

    void *arena_malloc(Arena *arena, size_t size);
    void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment);

    void arena_reset(Arena *arena);
    int arena_get_size(Arena *arena, size_t *size);
    int arena_get_available(Arena *arena, size_t *size);
    int arena_get_capacity(Arena *arena, size_t *capacity);
    int arena_top(Arena *arena, void **top);
#ifndef NDEBUG
    const char *arena_get_name(Arena *arena);
    void arena_print(Arena *arena);
#endif /* NDEBUG */
    
    ArenaMarker arena_mark(Arena *arena);
    void arena_rewind(Arena *arena, ArenaMarker marker);

    Arena *arena_create_subarena(Arena *parent, size_t capacity, const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <quest/quest_footer.h>

#endif /* ARENA_H_ */
