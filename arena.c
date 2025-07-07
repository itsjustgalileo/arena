#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

#include "./arena.h"

struct Arena {
    size_t capacity; /* Arena's total capacity */
    size_t used; /* Arena's used size */
    void *base; /* Arena's starting point */
};

Arena *arena_init(size_t capacity) {
    Arena *arena = (Arena *)malloc(sizeof(Arena));
    if (NULL == arena) {
        fprintf(stderr, "Failed to allocate memory Arena\n");
        return NULL;
    }

    arena->capacity = capacity;
    arena->used = 0;
    
    arena->base = malloc(capacity);
    if (NULL == arena->base) {
        fprintf(stderr, "Failed to allocate memory for base\n");
    }
    
    return arena;
}

void arena_free(Arena *arena) {
    if (NULL == arena) {
        fprintf(stderr, "Segfault\n");
        return;
    }
    free(arena->base);
    free(arena);
}

void *arena_malloc(Arena *arena, size_t size) {
    if (NULL == arena) {
        return NULL;
    }
    
    if (0 == size) {
        ++size;
    }
    
    if (arena->used + size > arena->capacity) {
        fprintf(stderr, "Buffer overrun\n");
        return NULL;
    }
    void *ptr = (char *)arena->base + arena->used;
    arena->used += size;
    memset(ptr, 0xAD, size);
    
    return ptr;
}

void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment) {
    if (NULL == arena) {
        return NULL;
    }
    
    if (0 == size) {
        ++size;
    }

    assert((alignment & (alignment - 1)) == 0);

    uintptr_t current = (uintptr_t)arena->base + arena->used;
    uintptr_t aligned = (current + (alignment - 1)) & ~(alignment - 1);
    size_t padding = aligned - current;

    if (arena->used + size + padding > arena->capacity) {
        fprintf(stderr, "Buffer overrun\n");
        return NULL;
    }
    
    void *ptr = (void *)aligned;
    arena->used += size + padding;
    return ptr;
}

void arena_reset(Arena *arena) {
    if (NULL == arena) {
        fprintf(stderr, "Segfault\n");
        return;
    }
    memset(arena->base, 0xDE, arena->used);
    arena->used = 0;
}

int arena_get_size(Arena *arena, size_t *size) {
    if (NULL == arena || NULL == size) {
        fprintf(stderr, "Segfault\n");
        return 1;
    }
    *size = arena->used;
    return 0;
}

int arena_get_capacity(Arena *arena, size_t *capacity) {
    if (NULL == arena || NULL == capacity) {
        fprintf(stderr, "Segfault\n");
        return 1;
    }
    *capacity = arena->capacity;
    return 0;
}

ArenaMarker arena_mark(Arena *arena) {
    if (NULL == arena) {
        fprintf(stderr, "Segfault\n");
        return 0x00;
    }
    return arena->used;
}

void arena_rewind(Arena *arena, ArenaMarker marker) {
    if (NULL == arena || marker > arena->used) {
        fprintf(stderr, "Segfault\n");
        return;
    }
    arena->used = marker;
}

Arena *arena_create_subarena(Arena *parent, size_t capacity) {
    if (NULL == parent) {
        fprintf(stderr, "Segfault\n");
        return NULL;
    }
    if (0 == capacity) {
        ++capacity;
    }
    Arena *subarena = (Arena *)arena_aligned_malloc(parent, sizeof(Arena), alignof(Arena));
    if (NULL == subarena) {
        fprintf(stderr, "Failed to malloc subarena\n");
        return NULL;
    }

    void *sub_base = arena_aligned_malloc(parent, capacity, __alignof__(max_align_t));
    if (NULL == sub_base) {
        fprintf(stderr, "Failed to malloc sub base\n");
        return NULL;
    }

    subarena->base = sub_base;
    subarena->used = 0;
    subarena->capacity = capacity;
    
    return subarena;
}
