#ifndef SHADER_DEFINED
#define SHADER_DEFINED

#include <cmath>
#include <algorithm> 

/* clampX() */
inline float clamp(float x) {
    if (x < 0) {
        return 0;
    }

    if (x > 1) {
        return 1;
    }

    return x;
}

/* repeatX() */
inline float repeat(float x) {
    return x - float(floor(x));
}

/* mirrorX() */
inline float mirror(float x) {
    
    // forward section
    if (int(floor(x)) % 2 == 0) {
        return x - float(floor(x));
    }

    // mirrored section
    else {
        return 1 - x + float(floor(x));
    }
}

#endif