#ifndef BITMAP_SHADER
#define BITMAP_SHADER

#include "shader.h"
#include "include/GPixel.h"
#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class BitmapShader : public GShader {

public:
    /* constructor */
    BitmapShader(const GBitmap& device, const GMatrix& matrix, GTileMode tilemode) : 
            fDevice(device), mx(matrix), inv(), tilemode(tilemode) {
                
                // set invW
                if (fDevice.width() == 1) {
                    invW = 0;
                } else {
                    invW = 1 / float(fDevice.width() - 1);
                }

                // set invH
                if (fDevice.height() == 1) {
                    invH = 0;
                } else {
                    invH = 1 / float(fDevice.height() - 1);
                }
            }

    /* isOpaque() */
    bool isOpaque() {        
        return fDevice.isOpaque();
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
        // build src
        GPoint src[count];
        for (int i = 0; i < count; i++) {
            GPoint thisPoint;
            thisPoint.x = x + i;
            thisPoint.y = y;
            src[i] = thisPoint;
        }

        GPoint dst[count];
        mapRow(x, y, count, dst);
        // inv.mapPoints(dst, src, count);

        // fill in row (floor and clamp)
        for (int i = 0; i < count; i++) {
            // tile
            float tiledX = dst[i].x;
            float tiledY = dst[i].y;

            // scale to unit [0,1]
            tiledX *= invW;
            tiledY *= invH;

            // apply tiling algorithm
            switch (tilemode) {
                
                // kClamp
                case GTileMode::kClamp:
                    tiledX = clamp(tiledX);
                    tiledY = clamp(tiledY);
                    break;

                // kRepeat
                case GTileMode::kRepeat:
                    tiledX = repeat(tiledX);
                    tiledY = repeat(tiledY);
                    break;

                // kMirror
                case GTileMode::kMirror:
                    tiledX = mirror(tiledX);
                    tiledY = mirror(tiledY);
                    break;
            }

            // scale back to bitmap
            tiledX *= (fDevice.width() - 1);
            tiledY *= (fDevice.height() - 1);

            row[i] = *(fDevice.getAddr(int(floor(tiledX)), int(floor(tiledY))));
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
    const GBitmap fDevice;
    const GMatrix mx;
    GMatrix inv;

    GTileMode tilemode;

    float invW;
    float invH;

    /* clampPoint()
    GPoint clampPoint(GPoint p) {
        
        float x = p.x;
        x *= invW;
        x = clamp(x);
        x *= (fDevice.width() - 1);

        float y = p.y;
        y *= invH;
        y = clamp(y);
        y *= (fDevice.height() - 1);

        return {x,y};
    }
    */

    /* repeatPoint()
    GPoint repeatPoint(GPoint p) {
        
        float x = p.x;
        x *= invW;
        x = repeat(x);
        x *= (fDevice.width() - 1);

        float y = p.y;
        y *= invH;
        y = repeat(y);
        y *= (fDevice.height() - 1);

        return {x,y};
    }
    */

    /* mirrorPoint()
    GPoint mirrorPoint(GPoint p) {
        
        float x = p.x;
        x *= invW;
        x = mirror(x);
        x *= (fDevice.width() - 1);

        float y = p.y;
        y *= invH;
        y = mirror(y);
        y *= (fDevice.height() - 1);

        return {x,y};
    }
    */
};

/* GCreateBitmapShader() */
std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localMatrix, GTileMode tilemode) {
    return std::shared_ptr<GShader>(new BitmapShader(bitmap,localMatrix,tilemode));
}

#endif