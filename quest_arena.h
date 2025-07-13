#ifndef QUEST_ARENA_H_
#define QUEST_ARENA_H_

#include <quest/quest_common.h>

#include <quest/quest_header.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    typedef struct quest_arena_t quest_arena_t;
    typedef size_t quest_arena_marker_t;

    extern QUEST_API quest_arena_t *QUEST_DECL quest_arena_init(size_t capacity, const char *name);
    extern QUEST_API void QUEST_DECL quest_arena_free(quest_arena_t *arena);

    extern QUEST_API void *QUEST_DECL quest_arena_malloc(quest_arena_t *arena, size_t size);
    extern QUEST_API void *QUEST_DECL quest_arena_aligned_malloc(quest_arena_t *arena, size_t size, size_t alignment);

    extern QUEST_API void QUEST_DECL quest_arena_reset(quest_arena_t *arena);
    extern QUEST_API int QUEST_DECL quest_arena_get_size(quest_arena_t *arena, size_t *size);
    extern QUEST_API int QUEST_DECL quest_arena_get_available(quest_arena_t *arena, size_t *size);
    extern QUEST_API int QUEST_DECL quest_arena_get_capacity(quest_arena_t *arena, size_t *capacity);
    extern QUEST_API int QUEST_DECL quest_arena_top(quest_arena_t *arena, void **top);
#ifndef NDEBUG
    extern QUEST_API const char *QUEST_DECL quest_arena_get_name(quest_arena_t *arena);
    extern QUEST_API void QUEST_DECL quest_arena_print(quest_arena_t *arena);
#endif /* NDEBUG */
    
    extern QUEST_API quest_arena_marker_t QUEST_DECL quest_arena_mark(quest_arena_t *arena);
    extern QUEST_API void QUEST_DECL quest_arena_rewind(quest_arena_t *arena, quest_arena_marker_t marker);

    extern QUEST_API quest_arena_t *QUEST_DECL quest_arena_create_subarena(quest_arena_t *parent, size_t capacity, const char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <quest/quest_footer.h>

#endif /* QUEST_ARENA_H_ */
