#ifndef _DMAC_H_
#define _DMAC_H_

#include "type_util.h"

enum eDmaChain { VIF0_CHAIN, VIF1_CHAIN, GIF_CHAIN };

enum {
    DMArefe     = 0x00000000,
    DMAcnt      = 0x10000000,
    DMAnext     = 0x20000000,
    DMAref      = 0x30000000,
    DMArefs     = 0x40000000,
    DMAcall     = 0x50000000,
    DMAret      = 0x60000000,
    DMAend      = 0x70000000,
};

#define DMAcnt(qwc) \
    (DMAcnt | qwc)

#define DMAend(qwc) \
    (DMAend | qwc)

#endif // _DMAC_H_
