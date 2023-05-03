#ifndef _TYPE_UTIL_H_
#define _TYPE_UTIL_H_

#include "macros.h"

#ifdef _PS2DEV_LINKED_
#include <dev/tamtypes.h>
#else

typedef int s128 __attribute__ ((mode (TI)));
typedef unsigned int u128 __attribute__ ((mode (TI)));

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef signed char  s8;
typedef signed short s16;
typedef signed int   s32;
typedef signed long  s64;

#endif // _PS2DEV_LINKED_

typedef unsigned long ulong;
typedef float f32;

typedef union Qword {
    // Arrays the memory goes ascending order
    // lo - hi

    u64     ul64[2];
    u32     ul32[4];
    f32     sf32[4];
    u128    ul128;

    // And on these function helpers, memory goes from descending order
    // hi - lo

    Qword() {}

    // Helper for GIF packets
    Qword(u64 hi, u64 lo) { 
        ul64[0] = lo; 
        ul64[1] = hi; 
    }
    
    // Helper for VIF packets
    Qword(u32 hhi, u32 hlo, u32 lhi, u32 llo) {
        ul64[0] = (((u64)lhi) << 32) | ((u64)llo);
        ul64[1] = (((u64)hhi) << 32) | ((u64)hlo);
    }
} Qword ALIGN_STRUCT(16);

struct ALIGN_STRUCT(16) Vec128 {
    f32 x, y, z, w;

    Vec128() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 1.0f;
    }
    
    Vec128(f32 scalar) {
        this->x = scalar;
        this->y = scalar;
        this->z = scalar;
        this->w = 1.0f;
    }

    Vec128(f32 x, f32 y, f32 z, f32 w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
};

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#endif // _TYPE_UTIL_H_
