#ifndef VERONOI_SHADER
#define VERONOI_SHADER

#include "shader.h"
#include "blend.h"
#include "include/GPoint.h"
#include "include/GPixel.h"
#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GMatrix.h"

#include <cmath>
#include <vector>
#include <iostream>

class VeronoiShader : public GShader {

public:

    /* constructor */
    VeronoiShader(const GPoint* pointArgs, const GColor* colorArgs, int count) : numColors(count) {
        opaque = true;
        inv = GMatrix();

        for (int i = 0; i < count; i++) {
            points.push_back(pointArgs[i]);
            colors.push_back(colorArgs[i]);

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
        // if invertible
        if (ctm.invert().has_value()) {
            inv = ctm.invert().value();
            return true;
        }

        return false;
    }

    /* shadeRow() */
    void shadeRow(int x, int y, int count, GPixel row[]) {
        GPoint mapped[count];
        mapRow(x, y, count, mapped);

        for (int i = 0; i < count; i++) {
            GColor closest = closestColor(mapped[i]);
            row[i] = convertColor2Pixel(closest);
        }
    }

    /* closestColor() */
    GColor closestColor(GPoint p) {
        float minDist = distance(p, points[0]);
        int index = 0;

        for (int i = 1; i < numColors; i++) {
            float dist = distance(p, points[i]);
            if (dist < minDist) {
                minDist = dist;
                index = i;
            }   
        }

        return colors[index];
    }

    float distance(GPoint p0, GPoint p1) {
        float dx = p0.x - p1.x;
        float dy = p0.y - p1.y;
        return sqrt((dx * dx) + (dy * dy));
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

GMatrix inv;

int numColors;
std::vector<GPoint> points;
std::vector<GColor> colors;


};

# endif