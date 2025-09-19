
#ifndef RV32I_TYPE_H
#define RV32I_TYPE_H

/* Common type aliases, Linux-style */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;

/* Boolean */
#ifndef __cplusplus
typedef enum { false = 0, true = 1 } bool;
#endif

#endif /* RV32I_TYPE_H */
