#ifndef _VIF_H_
#define _VIF_H_

#include "type_util.h"

enum {
    UNPACK_S_32     = 0x60,
    UNPACK_S_16     = 0x61,
    UNPACK_S8       = 0x62,
    UNPACK_V2_32    = 0x64,
    UNPACK_V2_16    = 0x65,
    UNPACK_V2_8     = 0x66,
    UNPACK_V3_32    = 0x68,
    UNPACK_V3_16    = 0x69,
    UNPACK_V3_8     = 0x6A,
    UNPACK_V4_32    = 0x6C,
    UNPACK_V4_16    = 0x6D,
    UNPACK_V4_8     = 0x6E,
    UNPACK_V4_5     = 0x6F,
};

enum {
    VIFnop          = 0x00000000,
    VIFstcycl       = 0x01000000,
    VIFoffset       = 0x02000000,
    VIFbase         = 0x03000000,
    VIFitop         = 0x04000000,
    VIFstmod        = 0x05000000,
    VIFmskpath3     = 0x06000000,
    VIFmark         = 0x07000000,
    VIFflushe       = 0x10000000,
    VIFflush        = 0x11000000,
    VIFflusha       = 0x13000000,
    VIFmscal        = 0x14000000,
    VIFmscalf       = 0x15000000,
    VIFmscnt        = 0x17000000,
    VIFstmask       = 0x20000000,
    VIFstrow        = 0x30000000,
    VIFstcol        = 0x31000000,
    VIFmpg          = 0x4A000000,
    VIFdirect       = 0x50000000,
    VIFdirecthl     = 0x51000000,
};

#define VIFcode(cmd, num, imm) \
    (((u32)cmd) | ((u32)num << 16) | ((u32)imm))

#define VIFunpack(type, count, offset) \
    (((u32)type << 24) | ((u32)count << 16) | ((u32)offset))

#define VIFnop() \
    VIFcode(VIFnop, 0, 0)

#define VIFstcycl(wl, cl) \
    VIFcode(VIFstcycl, 0, (wl << 8 | cl))

#define VIFoffset(offset) \
    VIFcode(VIFoffset, 0, offset)

#define VIFbase(base) \
    VIFcode(VIFbase, 0, base)

#define VIFitop(offset) \
    VIFcode(VIFitop, 0, offset)

#define VIFstmod(mode) \
    VIFcode(VIFstmod, 0, mode)

#define VIFmskpath3(mask) \
    VIFcode(VIFmskpath3, 0, mask << 15)

#define VIFmark(mark) \
    VIFcode(VIFmark, 0, mark)

#define VIFflushe() \
    VIFcode(VIFflushe, 0, 0)

#define VIFflush() \
    VIFcode(VIFflush, 0, 0)

#define VIFflusha() \
    VIFcode(VIFflusha, 0, 0)

#define VIFmscal(offset) \
    VIFcode(VIFmscal, 0, offset)

#define VIFmscalf(offset) \
    VIFcode(VIFmscalf, 0, offset)

#define VIFmscnt() \
    VIFcode(VIFmscnt, 0, 0)

#define VIFstmask(mask) \
    VIFcode(VIFstmask, 0, 0)

#define VIFstrow() \
    VIFcode(VIFstrow, 0, 0)

#define VIFstcol() \
    VIFcode(VIFstcol, 0, 0)

#define VIFmpg(count, offset) \
    VIFcode(VIFmpg, count, offset)

#define VIFdirect(count) \
    VIFcode(VIFdirect, 0, count)

#define VIFdirecthl(count) \
    VIFcode(VIFdirecthl, 0, count)

void Vif0UploadMicroProgram(u64* mpg, u32 count, u8 init);
void Vif1UploadMicroProgram(u64* mpg, u32 count, u8 init);

#endif // _VIF_H_
