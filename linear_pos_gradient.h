#ifndef LINEAR_POS_GRADIENT
#define LINEAR_POS_GRADIENT

#include "shader.h"
#include "blend.h"
#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GMatrix.h"

#include <vector>
#include <iostream>

class LinearPosGradient : public GShader {

public:

    /* constructor*/
    LinearPosGradient(GPoint p0, GPoint p1, const GColor* colorArgs, const float* pointArgs, int count) : p0(p0), p1(p1), numColors(count) {
        opaque = true;

        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        mx = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);

        for (int i = 0; i < count; i++) {
            colors.push_back(colorArgs[i]);
            stops.push_back(pointArgs[i]);
            
            if (colorArgs[i].a < 1) {
                opaque = false;
            }
        }
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
            return true;
        }

        return false;
    }

    /* shadeRow() */
    void shadeRow(int x, int y, int count, GPixel row[]) {
        GPoint mapped[count];
        mapRow(x, y, count, mapped);

        for (int i = 0; i < count; i++) {
            float mX = mapped[i].x;
            int index = lowerIndex(mX);

            float range = stops[index+1] - stops[index];
            float t = (mX - stops[index]) / range;

            GColor thisColor = (colors[index] * (1 - t)) + (colors[index+1] * t);
            row[i] = convertColor2Pixel(thisColor);
        }
    }

    /* lowerIndex() */
    int lowerIndex(float x) {
        // clamp below 0
        if (x < 0) {
            return 0;
        }
        
        // clamp above 1
        if (x > 1) {
            return numColors-2;
        }

        for (int i = 0; i < numColors-1; i++) {
            if ((x >= stops[i]) && (x < stops[i+1])) {
                return i;
            }
        }

        return numColors-2;
    }

    /* mapRow() */
    void mapRow(int x, int y, int count, GPoint row[]) {
        GVector c1 = inv.e0();
        GVector c2 = inv.e1();
        GVector c3 = inv.origin();
        
        float px = (c1.x * (x + 0.5f)) + (c2.x * (y + 0.5f)) + c3.x;
        float py = (c1.y * (x + 0.5f)) + (c2.y * (y + 0.5f)) + c3.y;
        
        for (int i = 0; i < count; i++) {
            row[i] = {px,py};
            px += c1.x;
            py += c1.y;
        }
    }

private:
    bool opaque;

    GMatrix mx;
    GMatrix inv;

    GPoint p0;
    GPoint p1;

    std::vector<GColor> colors;
    std::vector<float> stops;
    int numColors;
};

#endif