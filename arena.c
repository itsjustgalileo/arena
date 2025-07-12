#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "./arena.h"

struct Arena {
    size_t capacity; /* Arena's total capacity */
    size_t used; /* Arena's used size */
    void *base; /* Arena's starting point */
    bool is_sub; /* Helps with free */
#ifndef NDEBUG
    const char *debug_name;
#endif /* NDEBUG */
};

Arena *arena_init(size_t capacity, const char *name) {
    Arena *arena = (Arena *)malloc(sizeof(Arena));
    if (NULL == arena) {
        fprintf(stderr, "[arena_init]: Failed to allocate memory Arena\n");
        return NULL;
    }

    arena->is_sub = false;
    arena->capacity = capacity;
    arena->used = 0;
#ifndef NDEBUG
    arena->debug_name = name ? name : "unnamed_arena";
#endif
    
    arena->base = malloc(capacity);
    if (NULL == arena->base) {
        fprintf(stderr, "[arena_init]: Failed to allocate memory for base\n");
        free(arena);
        return NULL;
    }
    
    return arena;
}

/*
 * Frees an arena and its backing memory.
 * Do not call this on subarenas created via arena_create_subarena.
 * Use arena_rewind() or arena_reset() on the parent instead.
 */
void arena_free(Arena *arena) {
    assert(arena);
    if (arena->is_sub) {
        fprintf(stderr, "[arena_free]: Did you mean to call arena_rewind or arena_reset instead?\nLet's not continue, just for safety\n");
        return;
    }
    free(arena->base);
    free(arena);
    return;
}

void *arena_malloc(Arena *arena, size_t size) {
    assert(arena);
    
    if (0 == size) {
        ++size;
    }
    
    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "[arena_malloc]: Buffer overrun\n");
        return NULL;
    }
    void *ptr = (char *)arena->base + arena->used;
    arena->used += size;
#ifndef NDEBUG
    memset(ptr, 0xAD, size);
#endif /* NDEBUG */
    
    return ptr;
}

void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment) {
    assert(arena);
    if (0 == size) {
        ++size;
    }

    assert((alignment & (alignment - 1)) == 0);

    uintptr_t current = (uintptr_t)arena->base + arena->used;
    uintptr_t aligned = (current + (alignment - 1)) & ~(alignment - 1);
    size_t padding = aligned - current;

    if (arena->used + size + padding > arena->capacity) {
        fprintf(stderr, "[arena_aligned_malloc]: Buffer overrun\n");
        return NULL;
    }
    
    void *ptr = (void *)aligned;
    arena->used += size + padding;
    return ptr;
}

void arena_reset(Arena *arena) {
    assert(arena);
#ifndef NDEBUG
    memset(arena->base, 0xDE, arena->used);
#endif /* NDEBUG */
    arena->used = 0;
    return;
}

int arena_get_size(Arena *arena, size_t *size) {
    assert(arena && size);
    *size = arena->used;
    return 0;
}

int arena_get_available(Arena *arena, size_t *size) {
    assert(arena && size);
    *size = arena->capacity - arena->used;
    return 0;
}

int arena_get_capacity(Arena *arena, size_t *capacity) {
    assert(arena && capacity);
    *capacity = arena->capacity;
    return 0;
}
#ifndef NDEBUG
const char *arena_get_name(Arena *arena) {
    assert(arena);
    return arena->debug_name;
}

void arena_print(Arena *arena) {
    assert(arena);
    fprintf(stderr, "[Arena: %s] Used: %zu / %zu bytes\n",
            arena->debug_name ? arena->debug_name : "(unnamed)",
            arena->used, arena->capacity);
    return;
}
#endif /* NDEBUG */

int arena_top(Arena *arena, void **top) {
    assert(arena && top);
    *top = (char *)arena->base + arena->used;
    return 0;
}

ArenaMarker arena_mark(Arena *arena) {
    assert(arena);
    return arena->used;
}

void arena_rewind(Arena *arena, ArenaMarker marker) {
    assert(arena);
    if (marker > arena->used || marker > arena->capacity) {
        fprintf(stderr, "[arena_rewind]: Invalid rewind marker: out of bounds\n");
        return;
    }
    arena->used = marker;
    return;
}

Arena *arena_create_subarena(Arena *parent, size_t capacity, const char *name) {
    assert(parent);
    if (0 == capacity) {
        ++capacity;
    }
    Arena *subarena = (Arena *)arena_aligned_malloc(parent, sizeof(Arena), __alignof__(Arena));
    if (NULL == subarena) {
        fprintf(stderr, "[arena_create_subarena]: Failed to malloc subarena\n");
        return NULL;
    }
    subarena->is_sub = true;
#ifndef NDEBUG
    subarena->debug_name = name ? name : "anonymous_subarena";
#endif /* NDEBUG */
    
    void *sub_base = arena_aligned_malloc(parent, capacity, __alignof__(MAX_ALIGN));
    if (NULL == sub_base) {
        fprintf(stderr, "[arena_create_subarena]: Failed to malloc sub base\n");
        return NULL;
    }

    subarena->base = sub_base;
    subarena->used = 0;
    subarena->capacity = capacity;
    
    return subarena;
}
