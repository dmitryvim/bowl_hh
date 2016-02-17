#ifndef __UTIL_H__
#define __UTIL_H__

// Defines wrapper for rettype with meaning:
// "This could be rettype variable or error".
//
// This can be useful as a return type for functions that would
// normally return rettype when succeed and smthing like -1, null when failed.
// Now they can explicitly return error.
#define define_err_s(rettype, alias) \
    typedef struct { \
        rettype value; \
        char const *error; \
    } alias##_err_s;

#endif /* __UTIL_H__ */
