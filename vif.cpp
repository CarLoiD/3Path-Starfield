#include "vif.h"

#include <eekernel.h>
#include <libdma.h>
#include <libgraph.h>
#include <string.h>

#include "dmac.h"

// TODO: SIZE > 255
// Probably can implement it by just keeping a pointer to the actual head of the mpg
// And divide the actual size by 255 until it goes to zero

static void VifUploadMicroProgram(u8 channel, u64* mpg, u32 size, u8 start) {
    Qword pkt[512];
    memset(pkt, 0x0, sizeof(pkt));

    u32 codes = start ? 2 : 1;  // MPG/MSCAL
    u32 qwc = codes + size >> 1;    // 2 dwords = 1 qword

    pkt[0] = Qword(VIFmpg(size, 0), VIFnop, 0, DMAend(qwc));

    u64* upkt = (u64*)(pkt + 1);
    for (u32 index = 0; index < size; ++index) {
        *upkt++ = mpg[index];
    }
    
    // MSCAL start is also a TODO

    FlushCache(WRITEBACK_DCACHE);

    sceDmaChan* dch = sceDmaGetChan(channel);
    sceDmaSend(dch, pkt);
    
    sceGsSyncPath(0, 0);
}

void Vif0UploadMicroProgram(u64* mpg, u32 size, u8 start) {
    VifUploadMicroProgram(0, mpg, size, start);
}

void Vif1UploadMicroProgram(u64* mpg, u32 size, u8 start) {
    VifUploadMicroProgram(1, mpg, size, start);
}
