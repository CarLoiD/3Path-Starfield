#ifndef _STARFIELD_H_
#define _STARFIELD_H_

#include "type_util.h"

struct Star {
    Vec128 position;
    s32 dx, dy;
};

class StarField {
private:
    void Update();
    void Restart();
    void Reset(u32 index);

public:
    StarField(const u32 count);
    ~StarField();

    void DrawPath3();
    void DrawPath2();
    void DrawPath1();
    void DrawTest();

private:
    bool m_drawing;
    
    Star* m_buffer;
    Qword* m_packet;
    u32 m_count;
    u32 m_qwc;
};

#endif // _STARFIELD_H
