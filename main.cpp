#include <eekernel.h>
#include <libgraph.h>
#include <libdma.h>
#include <stdlib.h>

#include "dmac.h"
#include "starfield.h"

#define MAX_STARS 200

void InitializeGsContext();
void ClearFrameBuffer(u32 dwcolor);
void SwapBuffers();

int main() {
    InitializeGsContext();
    StarField stars(MAX_STARS);

    while (true) {
        DelayThread(1); // Give a time to ps2link process remote commands
        
        ClearFrameBuffer(0x00000080);
        stars.DrawPath1();
        SwapBuffers();
    }

    return 0;
}

sceGsDBuff db;
sceDmaChan* gif;

void InitializeGsContext() {
    sceDmaReset(1);
    gif = sceDmaGetChan(SCE_DMA_GIF);

    // This demo is meant to run on a NSTC output system
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FIELD);

    sceGsSetDefDBuff(
        &db,
        SCE_GS_PSMCT32, 
        DISPLAY_W, 
        DISPLAY_H,
        SCE_GS_DEPTH_GREATER,
        SCE_GS_PSMZ24,
        SCE_GS_NOCLEAR  // Disable libgraph clear (see below)
    );

    // Wait till next frame to align fields
    while (!sceGsSyncV(0));
}

// Implementing our own clear routine..
// SCE libgraph only clears when swapping the buffers, so you would swap first to get the right order
// By offloading the clear operation you have the flexibility of right order and better render state control

#define MAX_PKT_SIZE 8

u32 qwc;    // offset
Qword pkt[MAX_PKT_SIZE];

const s32 wdiv2 = DISPLAY_W >> 1;
const s32 hdiv2 = DISPLAY_H >> 1;

void ClearFrameBuffer(u32 dwcolor) {
    u8 r, g, b, a;
    DWCOLOR_RGBA(dwcolor, r, g, b, a);
    
    const s32 x = -wdiv2;
    const s32 y = -hdiv2;
    const s32 w = wdiv2;
    const s32 h = hdiv2;

    // Feed data to the GIF, packed mode, PATH3
    qwc = 0;
    pkt[qwc++] = Qword(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1));

    pkt[qwc++] = Qword(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_DEPTH_ALWAYS));
    pkt[qwc++] = Qword(SCE_GS_PRIM,   SCE_GS_PRIM_SPRITE);
    pkt[qwc++] = Qword(SCE_GS_RGBAQ,  SCE_GS_SET_RGBAQ(r, g, b, a, 0x3F800000));
    pkt[qwc++] = Qword(SCE_GS_XYZ2,   SCE_GS_SET_XYZ(FTOI4(x), FTOI4(y), 0));
    pkt[qwc++] = Qword(SCE_GS_XYZ2,   SCE_GS_SET_XYZ(FTOI4(w), FTOI4(h), 0));
    
    // This depth func should take the depth func before clear
    // Not doing this right now...
    pkt[qwc++] = Qword(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_DEPTH_GREATER));

    pkt[0].ul64[0] |= qwc - 1;  // NLOOP

    FlushCache(WRITEBACK_DCACHE);

    sceDmaSendN(gif, pkt, qwc);
    sceDmaSync(gif, 0, 0);
}

u32 frame = 0;
void SwapBuffers() {
    // Wait until the next vsync and swap
    sceGsSetHalfOffset(frame & 0x1 ? &db.draw1 : &db.draw0, 2048, 2048, !sceGsSyncV(0));
    sceGsSwapDBuff(&db, frame++);
}
