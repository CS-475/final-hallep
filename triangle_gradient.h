#ifndef TRIANGLE_GRADIENT
#define TRIANGLE_GRADIENT

#include "shader.h"
#include "blend.h"
#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GMatrix.h"

class TriangleGradient : public GShader {

public:
    /* constructor */
    TriangleGradient(const GPoint triPoints[3], const GColor triColors[3]) {
        // determine opacity
        opaque = (triColors[0].a == 1) && (triColors[1].a == 1) && (triColors[2].a == 1);

        // store first color
        c0 = triColors[0];

        // compute color differences
        dc1 = triColors[1] - triColors[0];
        dc2 = triColors[2] - triColors[0];

        // create matrix
        GPoint u = triPoints[1] - triPoints[0];
        GPoint v = triPoints[2] - triPoints[0];
        mx = GMatrix(u.x, v.x, triPoints[0].x, u.y, v.y, triPoints[0].y);
    }

    /* isOpaque() */
    bool isOpaque() {
        return opaque;
    }

    /* setContext() */
    bool setContext(const GMatrix& ctm) {
        GMatrix to_invert = ctm * mx;

        // if invertible
        if (to_invert.invert().has_value()) {
            inv = to_invert.invert().value();
            
            // set dc (change in color)
            GVector dp = inv.e0();
            dc = (dp.x * dc1) + (dp.y * dc2);
            
            return true;
        }

        return false;
    }

    /* shadeRow() */
    void shadeRow(int x, int y, int count, GPixel row[]) {
        // first point + color
        GPoint p = mapPoint(x, y);
        GColor c = (p.x * dc1) + (p.y * dc2) + c0;
        
        // for each pixel in row
        for (int i = 0; i < count; i++) {
            row[i] = convertColor2Pixel(c);
            c += dc;
        }
    }

    /* mapPoint() */
    GPoint mapPoint(int x, int y) {
        GVector c1 = inv.e0();
        GVector c2 = inv.e1();
        GVector c3 = inv.origin();

        float px = (c1.x * (x + 0.5f)) + (c2.x * (y + 0.5f)) + c3.x;
        float py = (c1.y * (x + 0.5f)) + (c2.y * (y + 0.5f)) + c3.y;

        return {px, py};
    }

private:
    GMatrix mx;
    GMatrix inv;

    bool opaque;

    GColor c0;
    GColor dc1;
    GColor dc2;

    GColor dc;
};

# endif