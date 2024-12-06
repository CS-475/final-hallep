#ifndef _g_canvas_h_
#define _g_canvas_h_

#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GPaint.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GShader.h"

#include <stack>

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device), matrices() {
        GMatrix identity = GMatrix();
        matrices.push(identity);
    }

    // DRAW FUNCTIONS
    void clear(const GColor& color) override;

    void drawRect(const GRect& rect, const GPaint& color) override;
    void drawConvexPolygon(const GPoint* points, int count, const GPaint& paint) override;
    void drawPath(const GPath& path, const GPaint& paint) override;

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                  int count, const int indices[], const GPaint& paint) override;
    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                  int level, const GPaint& paint) override;

    // MATRIX FUNCTIONS
    void save() override;
    void restore() override;
    void concat(const GMatrix& matrix) override;

private:
    const GBitmap fDevice;
    std::stack<GMatrix> matrices;
};

#endif