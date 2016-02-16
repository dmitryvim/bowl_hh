#ifndef __UTIL_H__
#define __UTIL_H__

#define define_err_s(rettype, alias) \
    typedef struct { \
        rettype value; \
        char const *error; \
    } alias##_err_s;


#endif /* __UTIL_H__ */
