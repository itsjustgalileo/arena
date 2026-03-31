#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include <arena/arena.h>

#define MAX_ARENA_COUNT 256
#define MAX_SUBARENA_COUNT (MAX_ARENA_COUNT >> 1)

static unsigned int arena_count;
static unsigned int subarena_count;

struct Arena {
    size_t capacity; /* Arena's total capacity */
    size_t offset;   /* Arena's offset size */
    void *base;      /* Arena's starting point */
    bool is_sub;     /* Helps with free */
#ifndef NDEBUG
    char debug_name[256]; /* if we want to easily track them */
#endif                    /* NDEBUG */
};

Arena *arena_create(size_t capacity, const char *name)
{
    assert(arena_count < MAX_ARENA_COUNT);
#ifndef NDEBUG
    if (NULL != name) {
        assert(strlen(name) + 1 <= 256 && "Arena's name is too long");
    }
#endif /* !NDEBUG */

    Arena *arena = (Arena *)malloc(sizeof(Arena));
    if (NULL == arena) {
        fprintf(stderr, "[arena_init]: Failed to allocate memory Arena\n");
        return NULL;
    }

    if (capacity == 0) {
        ++capacity;
    }

    arena->is_sub = false;
    arena->capacity = capacity;
    arena->offset = 0;

#ifndef NDEBUG
    if (name != NULL) {
        snprintf(arena->debug_name, sizeof(arena->debug_name), "%s", name);
    } else {
        snprintf(arena->debug_name, sizeof("unnamed_arenaXXX"), "%s%03u",
                 "unnamed_arena", arena_count);
    }
#endif

    arena->base = malloc(arena->capacity);
    if (NULL == arena->base) {
        fprintf(stderr, "[arena_init]: Failed to allocate memory for base\n");
        free(arena);
        return NULL;
    }

#ifndef NDEBUG
    memset(arena->base, 0xEF, capacity);
#endif /* NDEBUG */
    ++arena_count;
    return arena;
}

/*
 * Frees an arena and its backing memory.
 * Do not call this on subarenas created via arena_create_subarena.
 * Use arena_rewind() or arena_reset() on the parent instead.
 */
void arena_destroy(Arena *arena)
{
    assert(arena);
    if (arena->is_sub) {
        fprintf(stderr,
                "[arena_destroy]: Did you mean to call arena_rewind or "
                "arena_reset instead?\nLet's not continue, just for safety\n");
        return;
    }
    free(arena->base);
    free(arena);
    --arena_count;
    return;
}

void *arena_malloc(Arena *arena, size_t size)
{
    assert(arena);

    if (size == 0) {
        ++size;
    }

    const size_t alignment = _Alignof(max_align_t);
    uintptr_t current = (uintptr_t)arena->base + arena->offset;
    uintptr_t aligned = (current + (alignment - 1)) & ~(alignment - 1);
    size_t padding = aligned - current;

    if (arena->offset + size + padding > arena->capacity) {
        fprintf(stderr, "[arena_malloc]: Buffer overrun\n");
        return NULL;
    }

    void *ptr = (void *)aligned;
    arena->offset += size + padding;

#ifndef NDEBUG
    memset(ptr, 0xAD, size);
#endif /* NDEBUG */

    return ptr;
}

void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment)
{
    assert(arena);

    /* Making sure alignment is a power of two */
    assert((alignment & (alignment - 1)) == 0);

    if (size == 0) {
        ++size;
    }

    uintptr_t current = (uintptr_t)arena->base + arena->offset;
    uintptr_t aligned = (current + (alignment - 1)) & ~(alignment - 1);
    size_t padding = aligned - current;

    if (arena->offset + size + padding > arena->capacity) {
        fprintf(stderr, "[arena_aligned_malloc]: Buffer overrun\n");
        return NULL;
    }

    void *ptr = (void *)aligned;
    arena->offset += size + padding;

#ifndef NDEBUG
    memset(ptr, 0xAD, size);
#endif /* NDEBUG */

    return ptr;
}

void arena_reset(Arena *arena)
{
    assert(arena);

#ifndef NDEBUG
    memset(arena->base, 0xDE, arena->offset);
#endif /* NDEBUG */

    arena->offset = 0;
    return;
}

size_t arena_get_size(const Arena *arena)
{
    assert(arena);
    return arena->offset;
}

size_t arena_get_available(const Arena *arena)
{
    assert(arena);
    return arena->capacity - arena->offset;
}

size_t arena_get_capacity(const Arena *arena)
{
    assert(arena);
    return arena->capacity;
}
#ifndef NDEBUG
const char *arena_get_name(const Arena *arena)
{
    assert(arena);
    return arena->debug_name;
}

void arena_print(const Arena *arena)
{
    assert(arena);
    fprintf(stderr, "[Arena: %s] Used: %zu / %zu bytes\n", arena->debug_name,
            arena->offset, arena->capacity);
    return;
}
#endif /* NDEBUG */

void *arena_top(const Arena *arena)
{
    assert(arena);
    return (char *)arena->base + arena->offset;
}

bool arena_is_sub(const Arena *arena)
{
    assert(arena);
    return arena->is_sub;
}

ArenaMarker arena_mark(const Arena *arena)
{
    assert(arena);
    return arena->offset;
}

void arena_rewind(Arena *arena, const ArenaMarker marker)
{
    assert(arena);

    if (marker > arena->offset || marker > arena->capacity) {
        fprintf(stderr,
                "[arena_rewind]: Invalid rewind marker: out of bounds\n");
        return;
    }

    arena->offset = marker;
    return;
}

Arena *arena_create_subarena(Arena *parent, size_t capacity, const char *name)
{
    assert(parent && capacity);
    assert(subarena_count < MAX_SUBARENA_COUNT);

#ifndef NDEBUG
    if (NULL != name) {
        assert(strlen(name) + 1 <= 256 && "Subarena's name is too long");
    }
#endif /* !NDEBUG */

    if (capacity == 0) {
        ++capacity
    }

    // Keeping this to rollback if we fail to allocate the subarena's base
    ArenaMarker rollback = arena_mark(parent);

    // using _Alignof is not optimal. Maybe we should create cross-platform
    // macro.
    Arena *subarena = (Arena *)arena_aligned_malloc(parent, sizeof(Arena),
                                                    _Alignof(max_align_t));
    if (NULL == subarena) {
        fprintf(stderr, "[arena_create_subarena]: Failed to malloc subarena\n");
        return NULL;
    }
    subarena->is_sub = true;

#ifndef NDEBUG
    if (name != NULL) {
        snprintf(subarena->debug_name, sizeof(subarena->debug_name), "%s",
                 name);
    } else {
        snprintf(subarena->debug_name, sizeof("unnamed_subarenaXXX"), "%s%03u",
                 "unnamed_subarena", subarena_count);
    }
#endif /* NDEBUG */

    void *sub_base
        = arena_aligned_malloc(parent, capacity, _Alignof(max_align_t));
    if (NULL == sub_base) {
        /* rollback parent by capacity */
        arena_rewind(parent, rollback);
        fprintf(stderr, "[arena_create_subarena]: Failed to malloc sub base\n");
        return NULL;
    }

    subarena->base = sub_base;
    subarena->offset = 0;
    subarena->capacity = capacity;

    ++subarena_count;

    return subarena;
}

ArenaResult arena_try_create(size_t capacity, const char *name,
                             Arena **out_arena)
{
    return ARENA_NO_ERRORS;
}
