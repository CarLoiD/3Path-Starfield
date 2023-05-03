#include "starfield.h"

#include <string.h>
#include <stdlib.h>
#include <libgraph.h>
#include <libdma.h>
#include <libprintf.h>

#include "dmac.h"
#include "vif.h"

// Define MPG info headers
// This macro helper will define the extern reference to the vu symbols
// It also declare a static variable containing the size (instruction count) of the program

VU_MPG_HEADER(vu1_starfield);

static sceDmaChan* gif;
static sceDmaChan* vif1;

const f32 zSpeed = 0.03f;

void StarField::Update() {
    for (u32 index = 0; index < m_count; ++index) {
        Star* ptr = &m_buffer[index];

        // Update the "z" star coordinate, getting closer to the screen
        ptr->position.z -= zSpeed;
        if (ptr->position.z <= 0) {  // cull
            Reset(index);
        }

        // Convert to screen coordinates and zoom double
        ptr->dx = (s32)(ptr->position.x / ptr->position.z);
        ptr->dy = (s32)(ptr->position.y / ptr->position.z);

        // Is it out of screen bounds?
        if (ptr->dx < -HALF_W || ptr->dx >= HALF_W || 
            ptr->dy < -HALF_H || ptr->dy >= HALF_H
        ) {
            Reset(index);
        }
    }
}

void StarField::Restart() {
    memset(m_buffer, 0x0, m_count * sizeof(Star));
}

void StarField::Reset(u32 index) {
    Star* ptr = &m_buffer[index]; 

    // Randomize starting position and depth

    ptr->position.x = (f32)(rand() % DISPLAY_W) - FHALF_W;
    ptr->position.y = (f32)(rand() % DISPLAY_H) - FHALF_H;
    ptr->position.z = 0.001f + (rand() % 4);   // Offset a little to not spawn directly on camera
}

StarField::StarField(const u32 count) {
    gif  = sceDmaGetChan(SCE_DMA_GIF);

    // VIF1 will transfer DMA tag inside packets
    vif1 = sceDmaGetChan(SCE_DMA_VIF1);
    vif1->chcr.TTE = true;

    Vif1UploadMicroProgram(&vu1_starfield_begin, vu1_starfield_size, 0);

    m_buffer = (Star*) malloc((count * 2) * sizeof(Star));
    m_count = count;

    // Allocate enough memory to send data on all paths
    // This also take on account room for DMA tags and VIF codes

    m_packet = (Qword*) malloc((count * 2 + 64) * sizeof(Qword));
    m_qwc = 0;

    Restart();
}

StarField::~StarField() {
    if (m_count) {
        free(m_buffer);
        free(m_packet);
    }
}

// When not using chain mode, you pretty much just specify your primitives and render states
// PACKED mode already makes each qword be 128 aligned, while on REGLIST, every primitive is sent
// on a 64bit word, following the order of registers in the HI word of the GIF tag
// Since on REGLIST every entry on the list is a 4bit id, it cannot reference the whole range of GS registers
// A+D address this fact by the primitive aligning to 128bit boundary and it's HI word determine the GS register

void StarField::DrawPath3() {
    Update();

    m_qwc = 0;  // Reset packet offset

    m_packet[m_qwc++] = Qword(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(m_count + 3, 1, 0, 0, SCE_GIF_PACKED, 1));
    m_packet[m_qwc++] = Qword(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_DEPTH_ALWAYS));
    m_packet[m_qwc++] = Qword(SCE_GS_PRIM,   SCE_GS_PRIM_POINT);
    m_packet[m_qwc++] = Qword(SCE_GS_RGBAQ,  SCE_GS_SET_RGBAQ(0xA0, 0xA0, 0xA0, 0x80, 0x3F800000));

    for (u32 index = 0; index < m_count; ++index) {
        Star* ptr = &m_buffer[index];
        
        const s32 x = (s32)ptr->dx;
        const s32 y = (s32)ptr->dy;

        m_packet[m_qwc++] = Qword(SCE_GS_XYZ2, SCE_GS_SET_XYZ(FTOI4(x), FTOI4(y), 0));
    }

    FlushCache(WRITEBACK_DCACHE);
    
    sceDmaSendN(gif, m_packet, m_qwc);
    sceDmaSync(gif, 0, 0);
}

// Pretty much the same as PATH3, but we use DMA tags and VIF codes to specify the operation
// VIF1 code DIRECT send our qwords to the VIF FIFO, and then it's sent to the GIF directly
// Hence the name...

void StarField::DrawPath2() {
    Update();

    const u32 qwc2 = m_count + 4;

    m_qwc = 0;
    m_packet[m_qwc++] = Qword(VIFdirect(qwc2), VIFnop, 0, DMAend(qwc2));

    m_packet[m_qwc++] = Qword(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(qwc2 - 1, 1, 0, 0, SCE_GIF_PACKED, 1));
    m_packet[m_qwc++] = Qword(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_DEPTH_ALWAYS));
    m_packet[m_qwc++] = Qword(SCE_GS_PRIM,   SCE_GS_PRIM_POINT);
    m_packet[m_qwc++] = Qword(SCE_GS_RGBAQ,  SCE_GS_SET_RGBAQ(0xA0, 0xA0, 0xA0, 0x80, 0x3F800000));

    for (u32 index = 0; index < m_count; ++index) {
        Star* ptr = &m_buffer[index];

        const s32 x = (s32)ptr->dx;
        const s32 y = (s32)ptr->dy;

        m_packet[m_qwc++] = Qword(SCE_GS_XYZ2, SCE_GS_SET_XYZ(FTOI4(x), FTOI4(y), 0));
    }

    FlushCache(WRITEBACK_DCACHE);

    sceDmaSend(vif1, m_packet);
    sceGsSyncPath(0, 0);    // Notice the sync of all paths, not just polling DMA channel STR
}

// Quite simple for this demo
// We send data to VU1 using the VIF unpack functions instead of MMU IO (through VU1_MEM)

void StarField::DrawPath1() {
    Update();

    const u32 qwc1 = m_count + 4;
    
    m_qwc = 0;
    m_packet[m_qwc++] = Qword(VIFunpack(UNPACK_V4_32, qwc1, 0), VIFstcycl(4, 4), 0, DMAcnt(qwc1));
    m_packet[m_qwc++] = Qword(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(qwc1 - 1, 1, 0, 0, SCE_GIF_PACKED, 1));
    m_packet[m_qwc++] = Qword(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, SCE_GS_DEPTH_ALWAYS));
    m_packet[m_qwc++] = Qword(SCE_GS_PRIM,   SCE_GS_PRIM_POINT);
    m_packet[m_qwc++] = Qword(SCE_GS_RGBAQ,  SCE_GS_SET_RGBAQ(0xA0, 0xA0, 0xA0, 0x80, 0x3F800000));

    for (u32 index = 0; index < m_count; ++index) {
        Star* ptr = &m_buffer[index];

        const s32 x = (s32)ptr->dx;
        const s32 y = (s32)ptr->dy;

        m_packet[m_qwc++] = Qword(SCE_GS_XYZ2, SCE_GS_SET_XYZ(FTOI4(x), FTOI4(y), 0));
    }

    m_packet[m_qwc++] = Qword(VIFmscal(0), VIFflushe, 0, DMAend(0));
    FlushCache(WRITEBACK_DCACHE);

    sceDmaSend(vif1, m_packet);
    sceGsSyncPath(0, 0);
}
