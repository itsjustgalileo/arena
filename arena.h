/**
 * @file arena.h
 * @author itsjustgalileo
 * @version 1.2
 * @brief Basic bump allocator in C.
 */
#ifndef ARENA_H_
#define ARENA_H_

#include <stdlib.h>
#include <stdbool.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief An opaque type representing the arena type.
     */
    typedef struct Arena Arena;

    /**
     * @brief An unsigned integer to hold the return
     * pseudo-address for arena_rewind(). Remains valid
     * only as long as the arena's state remains unchanged.
     */
    typedef size_t ArenaMarker;

    /**
     * @brief Creates an arena with a given non-zero capacity.
     *
     * If capacity == 0, a minimum allocation of 1 byte is performed.
     *
     * @param capacity The size of the chunk of memory to allocate for the entire pool.
     * @param name (optional) A name for easier debug tracking.
     *
     * @return A pointer to the arena object if success, or NULL if failure.
     */
    extern Arena *arena_create(size_t capacity, const char *name);

    /**
     * @brief Frees an entire arena. Never call this on a subarena.
     *
     * @param arena A pointer to the arena to free.
     */
    extern void arena_destroy(Arena *arena);

    /**
     * @brief Rewinds an arena back to the base. This does not free
     * any allocated memory.
     *
     * All previously returned pointers become invalid.
     */
    extern void arena_reset(Arena *arena);

    /**
     * @brief Returns a pointer to the allocated memory and bumps
     * the allocator's offset by the requested non-zero size, aligned to
     * the alignment of max_align_t.
     *
     * If size == 0, a minimum allocation of 1 byte is performed.
     *
     * @param arena The arena pool in which to allocate.
     * @param size The size of the desired memory chunk.
     * 
     * @return A pointer to the allocated memory chunk.
     */
    extern void *arena_malloc(Arena *arena, size_t size);

    /**
     * @brief Returns a pointer to the allocated memory and bumps
     * the allocator's offset by the requested size, aligned to
     * the requested size.
     *
     * If size == 0, a minimum allocation of 1 byte is performed.
     *
     * @param arena A pointer to the arena in which to allocate.
     * @param size The size of the desired memory chunk.
     * @param alignment The size of memory alignment. Must be > 0 and a power of 2.
     *
     * @return A pointer to the allocated memory chunk.
     */
    extern void *arena_aligned_malloc(Arena *arena, size_t size, size_t alignment);

    /**
     * @brief Get the size of allocated memory in a given arena.
     *
     * @param arena A pointer to the arena object.
     *
     * @return The offset of the arena.
     */
    extern size_t arena_get_size(const Arena *arena);

    /**
     * @brief Get the remaining memory in bytes in a given arena
     *
     * @param arena A pointer to the arena object.
     *
     * @return The available size in the arena.
     */
    extern size_t arena_get_available(const Arena *arena);
    
    /**
     * @brief Get the total capacity of a given arena.
     *
     * @param arena A pointer to the arena object.
     *
     * @return The capacity of the arena.
     */
    extern size_t arena_get_capacity(const Arena *arena);

    /**
     * @brief Return the new base of the arena after allocations.
     *
     * @param arena A pointer to the arena object.
     *
     * @return A pointer to the next available memory address.
     */
    extern void *arena_top(const Arena *arena);

#ifndef NDEBUG
    extern const char *arena_get_name(const Arena *arena);
    extern void arena_print(const Arena *arena);
#endif /* NDEBUG */

    /**
     * @brief Check if a given arena is a subarena or a parent.
     *
     * @param arena The arena to check.
     *
     * @return true if the arena is a subarena, false if not.
     */
    extern bool arena_is_sub(const Arena *arena);

    /**
     * @brief Set a checkpoint in memory.
     *
     * @param arena The arena in which to set the marker.
     *
     * return An integer (pseudo-address) that acts as a marker.
     */
    extern ArenaMarker arena_mark(const Arena *arena);

    /**
     * @brief Rewinds an arena back to a given marker.
     *
     * @param arena The arena to rewind.
     * @param marker The marker to jump back to.
     */
    extern void arena_rewind(Arena *arena, const ArenaMarker marker);

    /**
     * @brief Creates a secondary arena that branches off of an already existing one.
     *
     * Subarenas do not own memory and are invalidated if the parent is rewound past their creation point.
     *
     * If capacity == 0, a minimum bump of one byte is performed.
     *
     * @param parent The arena to branch from.
     * @param capacity The total size of the arena.
     * @param name (optional) debug name.
     */
    extern Arena *arena_create_subarena(Arena *parent, size_t capacity, const char *name);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* ARENA_H_ */
