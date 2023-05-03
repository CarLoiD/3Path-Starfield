#ifndef _MACROS_H_
#define _MACROS_H_

// Utilities

#define UNUSED_VARIABLE(var)        (void)(var)
#define ALIGN(value, alignment)     (value + alignment - 1) & (~(alignment - 1))
#define ALIGN_STRUCT(n)             __attribute__((aligned(n)))
#define PACKED_STRUCT               __attribute__((packed))
#define EE_ASM                      __asm__ __volatile__
#define UNCACHED_ADDR(addr)         ((void*)((u32)(addr) | 0x20000000))
#define IS_UNCACHED_ADDR(addr)      ((u32)(addr) & 0x20000000)
#define STRIP_UNCACHED(addr)        ((u32)(addr) & 0xFFFFFFF)
#define icast(type, name)           *(type*)name
#define U128_REG_FAST_ZEROMEM(reg)  icast(u128, &reg) = 0x0
#define U64_REG_FAST_ZEROMEM(reg)   icast(u64,  &reg) = 0x0
#define U32_REG_FAST_ZEROMEM(reg)   icast(u32,  &reg) = 0x0
#define FTOI4(x)                    ((2048 + x) << 4)
#define VU_SECTION(type, sym)       extern type sym __attribute__((section(".vudata")))

#define VU_MPG_HEADER(sym) \
    VU_SECTION(u64, sym##_begin); \
    VU_SECTION(u64, sym##_end); \
    static u32 sym##_size = (u32)(&sym##_end - &sym##_begin) \

#define DWCOLOR_RGBA(dwColor, r, g, b, a)   \
    r = (dwColor & 0xFF000000) >> 24;       \
    g = (dwColor & 0x00FF0000) >> 16;       \
    b = (dwColor & 0x0000FF00) >> 8;        \
    a = (dwColor & 0x000000FF)

#define RGBA_DWCOLOR(r, g, b, a) \
    (u32)((r << 24) | (g << 16) | (b << 8) | (a));

// Constants
            
#define DISPLAY_W           640
#define DISPLAY_H           480
#define HALF_W              (DISPLAY_W >> 1)
#define HALF_H              (DISPLAY_H >> 1)
#define FHALF_W             (DISPLAY_W * 0.5f)
#define FHALF_H             (DISPLAY_H * 0.5f)
#define GS_VRAM_MAX_TBP     0xFFFFF
#define DMA_IMAGE_MAX_QWC   0x7FFF
#define HOME_PATH           "host:"
#define CDVD_SECTOR_SIZE    2048

#endif // _MACROS_H_
