#ifndef TRIANGLE_BITMAP
#define TRIANGLE_BITMAP

#include "shader.h"
#include "blend.h"
#include "bitmap_shader.h"

#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class TriangleBitmap : public GShader {

public:
    /* constructor */
    TriangleBitmap(GShader* bmshader, const GPoint pVerts[3], const GPoint tVerts[3]) : bitmapshader(bmshader) {
        // create p matrix
        GPoint u = pVerts[1] - pVerts[0];
        GPoint v = pVerts[2] - pVerts[0];
        GMatrix p = GMatrix(u.x, v.x, pVerts[0].x, u.y, v.y, pVerts[0].y);

        // create t matrix
        u = tVerts[1] - tVerts[0];
        v = tVerts[2] - tVerts[0];
        GMatrix t = GMatrix(u.x, v.x, tVerts[0].x, u.y, v.y, tVerts[0].y);

        // set p * t^-1
        ptinv = p * t.invert().value();
    }

    /* isOpaque() */
    bool isOpaque() {
        return bitmapshader->isOpaque();
    }

    /* setContext () */
    bool setContext(const GMatrix& ctm) {
        return bitmapshader->setContext(ctm * ptinv);
    }

    /* shadeRow() */
    void shadeRow(int x, int y, int count, GPixel row[]) {
        bitmapshader->shadeRow(x, y, count, row);
    }

private:
    GShader* bitmapshader;
    GMatrix ptinv;
};

#endif