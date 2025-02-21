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

#define IGNORE_DIAGNOSTIC(name, statement) \
    XPRAGMA(GCC diagnostic push);          \
    XPRAGMA(GCC diagnostic ignored name);  \
    statement;                             \
    XPRAGMA(GCC diagnostic pop);

#ifndef PI
#  define PI (3.1415926534f)
#endif

#endif
