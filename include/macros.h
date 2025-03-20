#ifndef MACROS_H
#define MACROS_H


#ifndef UNUSED
#  define UNUSED(x) {(void)x;}
#endif

#ifndef XSTR
#  define XSTR(x) #x
#endif
#ifndef STR
#  define STR(x) XSTR(x)
#endif

#ifndef XJOIN2
#  define XJOIN2(a, b) a##b
#endif
#ifndef JOIN2
#  define JOIN2(a, b) XJOIN2(a, b)
#endif
#ifndef XJOIN3
#  define XJOIN3(a, b, c) a##b##c
#endif
#ifndef JOIN3
#  define JOIN3(a, b, c) XJOIN3(a, b, c)
#endif

#ifndef XPRAGMA
#  define XPRAGMA(x) _Pragma(#x)
#endif

#ifndef IGNORE_DIAGNOSTIC
#  define IGNORE_DIAGNOSTIC(name, statement) \
    XPRAGMA(GCC diagnostic push);            \
    XPRAGMA(GCC diagnostic ignored name);    \
    statement;                               \
    XPRAGMA(GCC diagnostic pop);
#endif

#ifndef PI
#  define PI (3.1415926534f)
#endif

#ifndef LIST_INIT_CAP
#  define LIST_INIT_CAP (256)
#endif

#ifndef list
#define list(type, name) \
    struct {             \
        type *items;     \
        size_t count;    \
        size_t capacity; \
    } name;
#endif

#ifndef list_init
#define list_init(list)                                                                  \
    do {                                                                                 \
        (list)->count = 0;                                                               \
        (list)->capacity = LIST_INIT_CAP;                                                \
        (list)->items = NULL;                                                            \
        (list)->items = realloc((list)->items, (list)->capacity*sizeof(*(list)->items)); \
        assert((list)->items != NULL && "Buy more RAM lol");                             \
    } while (0)
#endif

#ifndef list_append
#define list_append(list, item)                                                              \
    do {                                                                                     \
        if ((list)->count >= (list)->capacity) {                                             \
            (list)->capacity = (list)->capacity == 0 ? LIST_INIT_CAP : (list)->capacity*2;   \
            (list)->items = realloc((list)->items, (list)->capacity*sizeof(*(list)->items)); \
            assert((list)->items != NULL && "Buy more RAM lol");                             \
        }                                                                                    \
                                                                                             \
        (list)->items[(list)->count++] = (item);                                             \
    } while (0)
#endif

#ifndef list_free
#  define list_free(list) free((list).items)
#endif

#ifndef list_append_many
#define list_append_many(list, new_items, new_items_count)                                            \
    do {                                                                                              \
        if ((list)->count + (new_items_count) > (list)->capacity) {                                   \
            if ((list)->capacity == 0) {                                                              \
                (list)->capacity = LIST_INIT_CAP;                                                     \
            }                                                                                         \
            while ((list)->count + (new_items_count) > (list)->capacity) {                            \
                (list)->capacity *= 2;                                                                \
            }                                                                                         \
            (list)->items = realloc((list)->items, (list)->capacity*sizeof(*(list)->items));          \
            assert((list)->items != NULL && "Buy more RAM lol");                                      \
        }                                                                                             \
        memcpy((list)->items + (list)->count, (new_items), (new_items_count)*sizeof(*(list)->items)); \
        (list)->count += (new_items_count);                                                           \
    } while (0)
#endif

#ifndef list_resize
#define list_resize(list, new_size)                                                            \
    do {                                                                                       \
        if ((new_size) > (list)->capacity) {                                                   \
            (list)->capacity = (new_size);                                                     \
            (list)->items = realloc((list)->items, (list)->capacity * sizeof(*(list)->items)); \
            assert((list)->items != NULL && "Buy more RAM lol");                               \
        }                                                                                      \
        (list)->count = (new_size);                                                            \
    } while (0)
#endif

#endif
