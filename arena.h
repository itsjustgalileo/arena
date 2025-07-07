#ifndef ARENA_H_
#define ARENA_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct Arena Arena;
    typedef size_t ArenaMarker;

    Arena *arena_init(size_t capacity);
    void arena_free(Arena *arena);

    void *arena_malloc(Arena *arena, size_t size);
    void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment);

    void arena_reset(Arena *arena);
    int arena_get_size(Arena *arena, size_t *size);
    int arena_get_capacity(Arena *arena, size_t *capacity);

    ArenaMarker arena_mark(Arena *arena);
    void arena_rewind(Arena *arena, ArenaMarker marker);

    Arena *arena_create_subarena(Arena *parent, size_t capacity);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARENA_H_ */
