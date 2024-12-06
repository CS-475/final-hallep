#include "include/GPoint.h"
#include "include/GMatrix.h"
#include <iostream>

/* GMatrix()
 * returns identity matrix:
 *  [1 0 0]
 *  [0 1 0]
 *  [0 0 1] <-- (implied)
 */
GMatrix::GMatrix() {
    fMat[0] = 1;
    fMat[1] = 0;
    fMat[2] = 0;
    fMat[3] = 1;
    fMat[4] = 0;
    fMat[5] = 0;
}

/* Translate()
 * [1 0 tx]
 * [0 1 ty]
 */
GMatrix GMatrix::Translate(float tx, float ty) {
    return GMatrix(1,0,tx,0,1,ty);
}

/* Scale()
 * [sx 0  0]
 * [0  sy 0]
 */
GMatrix GMatrix::Scale(float sx, float sy) {
    return GMatrix(sx,0,0,0,sy,0);
}

/* Rotate()
 * [cos -sin 0]
 * [sin  cos 0]
 */
GMatrix GMatrix::Rotate(float radians) {
    float cosT = cos(radians);
    float sinT = sin(radians);
    return GMatrix(cosT,-sinT,0,sinT,cosT,0);
}

/* Concat()
 * return (a * b)
 */
GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    float fa = (a.fMat[0] * b.fMat[0]) + (a.fMat[2] * b.fMat[1]);
    float fb = (a.fMat[1] * b.fMat[0]) + (a.fMat[3] * b.fMat[1]);
    float fc = (a.fMat[0] * b.fMat[2]) + (a.fMat[2] * b.fMat[3]);
    float fd = (a.fMat[1] * b.fMat[2]) + (a.fMat[3] * b.fMat[3]);
    float fe = (a.fMat[0] * b.fMat[4]) + (a.fMat[2] * b.fMat[5]) + a.fMat[4];
    float ff = (a.fMat[1] * b.fMat[4]) + (a.fMat[3] * b.fMat[5]) + a.fMat[5];

    return GMatrix(fa,fc,fe,fb,fd,ff);
}

/* invert() */
nonstd::optional<GMatrix> GMatrix::invert() const {
    float det = (fMat[0] * fMat[3]) - (fMat[2] * fMat[1]);

    // if not invertible
    if (det == 0) {
        return {};
    }

    float det_inv = 1/det;

    float a = fMat[3] * det_inv;
    float b = -fMat[1] * det_inv;
    float c = -fMat[2] * det_inv;
    float d = fMat[0] * det_inv;
    float e = ((fMat[2] * fMat[5]) - (fMat[3] * fMat[4])) * det_inv;
    float f = (-(fMat[0] * fMat[5]) + (fMat[1] * fMat[4])) * det_inv;

    return GMatrix(a,c,e,b,d,f);
}

/* mapPoints()
 * x' = ax + cy + e
 * y' = bx + dy + f
 */
void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; i++) {
        float x = (fMat[0] * src[i].x) + (fMat[2] * src[i].y) + fMat[4];
        float y = (fMat[1] * src[i].x) + (fMat[3] * src[i].y) + fMat[5];

        dst[i] = {x,y};
    }
}
