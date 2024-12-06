#ifndef LINEAR_GRADIENT
#define LINEAR_GRADIENT

#include "shader.h"
#include "blend.h"
#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GMatrix.h"

#include <vector>
#include <iostream>

class LinearGradient : public GShader {

public:
    /* constructor */
    LinearGradient(GPoint p0, GPoint p1, const GColor* colors, int count, GTileMode tilemode) : tilemode(tilemode) {
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            mx = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);

            LinearGradient::p0 = p0;
            LinearGradient::p1 = p1;

            // fill in colorStops
            for (int i = 0; i < count; i++) {
                colorStops.push_back(colors[i]);
            }

            // fill in colorDiffs
            for (int i = 0; i < count - 1; i++) {
                colorDiffs.push_back(colors[i+1] - colors[i]);
            }

            numColors = count;
        }

    /* isOpaque() */
    bool isOpaque() {
        // for each color...
        for (int i = 0; i < numColors; i++) {
            
            // if any color is transparent (a < 1)...
            if (colorStops[i].a < 1) {
                return false;
            }
        }
        return true;
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

        // only 1 color
        if (numColors == 1) {
            for (int i = 0; i < count; i++) {
                row[i] = convertColor2Pixel(colorStops[0]);
            }
        }
        
        // multiple colors
        else {

            // build src
            GPoint src[count];
            for (int i = 0; i < count; i++) {
                GPoint thisPoint;
                thisPoint.x = x + i;
                thisPoint.y = y;
                src[i] = thisPoint;
            }

            // map points
            GPoint dst[count];
            mapRow(x, y, count, dst);
            // inv.mapPoints(dst, src, count);

            for (int i = 0; i < count; i++) {
                float tiledX;

                // tile
                switch (tilemode) {
                    // kClamp
                    case GTileMode::kClamp:
                        tiledX = clamp(dst[i].x);
                        break;

                    // kRepeat
                    case GTileMode::kRepeat:
                        tiledX = repeat(dst[i].x);
                        break;

                    // kMirror
                    case GTileMode::kMirror:
                        tiledX = mirror(dst[i].x);
                        break;
                }

                // 2 colors
                if (numColors == 2) {
                    const GColor thisColor = colorStops[0] + (tiledX * colorDiffs[0]);
                    row[i] = convertColor2Pixel(thisColor);
                }

                // 3+ colors
                else {
                    float thisX = tiledX * (numColors - 1);
                    int index = int(floor(thisX));
                    float t = thisX - index;
                    
                    const GColor thisColor = colorStops[index] + (t * colorDiffs[index]);
                    row[i] = convertColor2Pixel(thisColor);
                }   
            }
        }
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
    GMatrix mx;
    GMatrix inv;
    
    GPoint p0;
    GPoint p1;
    std::vector<GColor> colorStops;
    std::vector<GColor> colorDiffs;
    int numColors;

    GTileMode tilemode;
};

/* GCreateLinearGradient() */
std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor* colors, int count, GTileMode tilemode) {
    return std::shared_ptr<GShader>(new LinearGradient(p0,p1,colors,count,tilemode));
}

#endif