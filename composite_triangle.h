#ifndef COMPOSITE_TRIANGLE
#define COMPOSITE_TRIANGLE

#include "shader.h"
#include "blend.h"
#include "bitmap_shader.h"
#include "triangle_bitmap.h"
#include "triangle_gradient.h"

#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class CompositeTriangle : public GShader {
public:
    /* constructor */
    CompositeTriangle(GShader* bmshader, const GPoint pVerts[3], const GPoint tVerts[3], const GColor colors[3]) {
        triGradient = new TriangleGradient(pVerts, colors);
        triBitmap = new TriangleBitmap(bmshader, pVerts, tVerts);
    }

    /* isOpaque() */
    bool isOpaque() {
        return triBitmap->isOpaque() && triGradient->isOpaque();
    }
    
    /* setContext () */
    bool setContext(const GMatrix& ctm) {
        return triBitmap->setContext(ctm) && triGradient->setContext(ctm);
    }

    /* shadeRow() */
    void shadeRow(int x, int y, int count, GPixel row[]) {
        GPixel bm [count];
        triBitmap->shadeRow(x, y, count, bm);
        
        GPixel gr [count];
        triGradient->shadeRow(x, y, count, gr);

        for (int i = 0; i < count; i++) {
            unsigned a = div255(GPixel_GetA(bm[i]) * GPixel_GetA(gr[i]));
            unsigned r = div255(GPixel_GetR(bm[i]) * GPixel_GetR(gr[i]));
            unsigned g = div255(GPixel_GetG(bm[i]) * GPixel_GetG(gr[i]));
            unsigned b = div255(GPixel_GetB(bm[i]) * GPixel_GetB(gr[i]));

            row[i] = GPixel_PackARGB(a,r,g,b);
        }
    }

private:
    TriangleGradient* triGradient;
    TriangleBitmap* triBitmap;
};

#endif