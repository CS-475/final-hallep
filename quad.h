#ifndef QUAD_DEFINED
#define QUAD_DEFINED

#include "canvas.h"
#include "include/GPoint.h"
#include "include/GColor.h"

/* quadPoint() */
GPoint quadPoint(const GPoint quadPoints[4], float u, float v) {
    return (1-u)*(1-v)*quadPoints[0] + u*(1-v)*quadPoints[1] + (1-u)*v*quadPoints[3] + u*v*quadPoints[2];
}

/* quadColor() */
GColor quadColor(const GColor quadColors[4], float u, float v) {
    return (1-u)*(1-v)*quadColors[0] + u*(1-v)*quadColors[1] + (1-u)*v*quadColors[3] + u*v*quadColors[2];
}

/* fillColorQuad() */
void getQuadColors(const GColor quadColors[4], GColor colors[4], float u1, float u2, float v1, float v2) {
    colors[0] = quadColor(quadColors, u1, v1);
    colors[1] = quadColor(quadColors, u2, v1);
    colors[2] = quadColor(quadColors, u2, v2);
    colors[3] = quadColor(quadColors, u1, v2);
}

void getQuadPoints(const GPoint quadPoints[4], GPoint points[4], float u1, float u2, float v1, float v2) {
    points[0] = quadPoint(quadPoints, u1, v1);
    points[1] = quadPoint(quadPoints, u2, v1);
    points[2] = quadPoint(quadPoints, u2, v2);
    points[3] = quadPoint(quadPoints, u1, v2);

}

#endif