#include "canvas.h"
#include "blend.h"
#include "edge.h"

#include "bitmap_shader.h"
#include "linear_gradient.h"
#include "triangle_gradient.h"
#include "triangle_bitmap.h"
#include "composite_triangle.h"
#include "quad.h"

#include <vector>
#include <algorithm>
#include <iostream>

/***** MATRIX STACK METHODS *****/

/* save() */
void MyCanvas::save() {
    matrices.push(matrices.top());
}

/* restore() */
void MyCanvas::restore() {
    matrices.pop();
}

/* concat() */
void MyCanvas::concat(const GMatrix& matrix) {
    matrices.top() = matrices.top() * matrix;
}

/***** DRAW METHODS *****/

/* clear()
 * sets entire GCanvas (fDevice) to specified GColor (color) 
 */
void MyCanvas::clear(const GColor& color) {
    GPixel newPixel = convertColor2Pixel(color);

    int width = fDevice.width();
    int height = fDevice.height();

    GPixel* curr_row = nullptr;

    for (int y = 0; y < height; y++) {
        curr_row = fDevice.getAddr(0,y);
        for (int x = 0; x < width; x++) {
            curr_row[x] = newPixel;
        }
    }
}

/* drawRect()
 * calls drawConvexPolygon
 */
void MyCanvas::drawRect(const GRect& rect, const GPaint& color) {
    GPoint tl, tr, bl, br;

    tl.x = rect.left;
    tl.y = rect.top;

    tr.x = rect.right;
    tr.y = rect.top;

    bl.x = rect.left;
    bl.y = rect.bottom;

    br.x = rect.right;
    br.y = rect.bottom;

    GPoint points[4] = {tl, tr, br, bl};
    
    MyCanvas::drawConvexPolygon(points,4,color);
}

/* drawConvexPolygon() */
void MyCanvas::drawConvexPolygon(const GPoint* points, int count, const GPaint& paint) {
    // GBlendMode optBlendMode = get_optimized_blend(paint);
    GBlendMode optBlendMode = paint.getBlendMode();

    if (optBlendMode != GBlendMode::kDst) {
        GMatrix ctm = matrices.top();
        std::vector<Edge> edges;
        get_edges(&edges, points, count, fDevice.width(), fDevice.height(), ctm);

        // if still a polygon...
        if (edges.size() >= 2) {
            Edge edge1 = edges[0];
            Edge edge2 = edges[1];
            int nextEdge = 2;

            // get top (first row) and bottom (last row)
            int top = fDevice.height();
            int bottom = 0;
            for (int i = 0; i < edges.size(); i++) {
                
                // assign top
                if (edges[i].top < top) {
                    top = edges[i].top;
                }

                // assign bottom
                if (edges[i].bottom > bottom) {
                    bottom = edges[i].bottom;
                }
            }

            GPixel* curr_row = nullptr;
            GShader* shader = paint.peekShader();

            // use shader: paint.peekShader()
            if (shader) {
                bool validContext = (*shader).setContext(matrices.top());

                if (validContext) {
                    
                    // for each row... 
                    for (int y = top; y < bottom; y++) {

                        float y_ray = float(y + 0.5);
                        float x1 = edge1.eval_x(y_ray);
                        float x2 = edge2.eval_x(y_ray);

                        int left = int(round(std::min(x1, x2)));
                        int right = int(round(std::max(x1, x2)));
                        int count = right - left;

                        if (left < right) {

                            curr_row = fDevice.getAddr(left,y);
                            GPixel src[count];
                            (*shader).shadeRow(left,y,count,src);

                            for (int x = 0; x < count; x++) {

                                // curr_row[x] = src[x];
                                
                                switch (optBlendMode) {
                                    case GBlendMode::kClear:
                                        curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                        break;
                                    
                                    case GBlendMode::kSrc:
                                        curr_row[x] = src[x];
                                        break;

                                    case GBlendMode::kSrcOver:
                                        curr_row[x] = blend_kSrcOver(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOver:
                                        curr_row[x] = blend_kDstOver(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcIn:
                                        curr_row[x] = blend_kSrcIn(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstIn:
                                        curr_row[x] = blend_kDstIn(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcOut:
                                        curr_row[x] = blend_kSrcOut(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOut:
                                        curr_row[x] = blend_kDstOut(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcATop:
                                        curr_row[x] = blend_kSrcATop(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstATop:
                                        curr_row[x] = blend_kDstATop(&src[x],&curr_row[x]);
                                        break;

                                    case GBlendMode::kXor:
                                        curr_row[x] = blend_kXor(&src[x],&curr_row[x]);
                                        break;
                                }

                            }

                            // if edge1 is "expired"
                            if (edge1.bottom < y + 1.5) {
                                edge1 = edges[nextEdge];
                                nextEdge += 1;
                            }
                            
                            // if edge2 is "expired"
                            if (edge2.bottom < y + 1.5) {
                                edge2 = edges[nextEdge];
                                nextEdge += 1;
                            }
                        }
                        
                    }
                }
            }
            
            // use color: paint.getColor()
            else {
                // for each row... 
                for (int y = top; y < bottom; y++) {
                    curr_row = fDevice.getAddr(0,y);

                    float y_ray = float(y + 0.5);
                    float x1 = edge1.eval_x(y_ray);
                    float x2 = edge2.eval_x(y_ray);

                    int left = int(round(std::min(x1, x2)));
                    int right = int(round(std::max(x1, x2)));

                    for (int x = left; x < right; x++) {

                        switch (optBlendMode) {
                            case GBlendMode::kClear:
                                curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                break;
                            
                            case GBlendMode::kSrc:
                                curr_row[x] = convertColor2Pixel(paint.getColor());
                                break;

                            case GBlendMode::kSrcOver:
                                curr_row[x] = blend_kSrcOver(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kDstOver:
                                curr_row[x] = blend_kDstOver(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kSrcIn:
                                curr_row[x] = blend_kSrcIn(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kDstIn:
                                curr_row[x] = blend_kDstIn(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kSrcOut:
                                curr_row[x] = blend_kSrcOut(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kDstOut:
                                curr_row[x] = blend_kDstOut(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kSrcATop:
                                curr_row[x] = blend_kSrcATop(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kDstATop:
                                curr_row[x] = blend_kDstATop(paint.getColor(),&curr_row[x]);
                                break;

                            case GBlendMode::kXor:
                                curr_row[x] = blend_kXor(paint.getColor(),&curr_row[x]);
                                break;
                        }
                    
                    }

                    // if edge1 is "expired"
                    if (edge1.bottom < y + 1.5) {
                        edge1 = edges[nextEdge];
                        nextEdge += 1;
                    }
                    
                    // if edge2 is "expired"
                    if (edge2.bottom < y + 1.5) {
                        edge2 = edges[nextEdge];
                        nextEdge += 1;
                    }
                }
                
            }
        }
    }
}

/* drawPath() */
/*
void MyCanvas::drawPath(const GPath& path, const GPaint& paint) {
     // GBlendMode optBlendMode = get_optimized_blend(paint);
    GBlendMode optBlendMode = paint.getBlendMode();

    if (optBlendMode != GBlendMode::kDst) {
        GMatrix ctm = matrices.top();
        std::vector<Edge> edges;
        getEdges(&edges,path,fDevice.width(),fDevice.height(),ctm);

        // if still a polygon...
        if (edges.size() >= 2) {
            Edge edge1 = edges[0];
            Edge edge2 = edges[1];

            // get top (first row) and bottom (last row)
            int top = fDevice.height();
            int bottom = 0;
            for (int i = 0; i < edges.size(); i++) {
                
                // assign top
                if (edges[i].top < top) {
                    top = edges[i].top;
                }

                // assign bottom
                if (edges[i].bottom > bottom) {
                    bottom = edges[i].bottom;
                }
            }

            GPixel* curr_row = nullptr;
            GShader* shader = paint.peekShader();

            // use shader: paint.peekShader()
            if (shader) {

                bool validContext = (*shader).setContext(matrices.top());
                if (validContext) {

                    for (int y = top; y < bottom; y++) {
                        curr_row = fDevice.getAddr(left,y);

                        int i = 0;
                        int w = 0;
                        int left;

                        // for all active edges...
                        while ((i < edges.size()) && (edges[i].isValid(y))) {
                            int x = int(edge[i].eval_x(y));
                            if (w == 0) {
                                left = x;
                            }

                            w += edge[i].w;
                            if (w == 0) {
                                int right = x;
                                
                                GPixel src[count];
                                (*shader).shadeRow(left,y,count,src);

                                for (int x = 0; x < count; x++) {

                                    // curr_row[x] = src[x];
                                    
                                    switch (optBlendMode) {
                                        case GBlendMode::kClear:
                                            curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                            break;
                                        
                                        case GBlendMode::kSrc:
                                            curr_row[x] = src[x];
                                            break;

                                        case GBlendMode::kSrcOver:
                                            curr_row[x] = blend_kSrcOver(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kDstOver:
                                            curr_row[x] = blend_kDstOver(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kSrcIn:
                                            curr_row[x] = blend_kSrcIn(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kDstIn:
                                            curr_row[x] = blend_kDstIn(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kSrcOut:
                                            curr_row[x] = blend_kSrcOut(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kDstOut:
                                            curr_row[x] = blend_kDstOut(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kSrcATop:
                                            curr_row[x] = blend_kSrcATop(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kDstATop:
                                            curr_row[x] = blend_kDstATop(&src[x],&curr_row[x]);
                                            break;

                                        case GBlendMode::kXor:
                                            curr_row[x] = blend_kXor(&src[x],&curr_row[x]);
                                            break;
                                    }

                                }
                            }

                            // check edge validity;
                            // remove edge from vector if no longer valid
                            if (edge[i].isValid(y+1)) {
                                i += 1;
                            } else {
                                edges.erase(i);
                            }
                        }

                        assert(w == 0);

                        while ((i < edge.size()) && (edge[i].isValid(y+1))) {
                            i += 1;
                        }

                        std::sort(edges.begin(),edges.begin()+i,[y](const Edge& e1, const Ege& e1) {
                            return edge_sort_x(e1, e2, y);
                        });

                    }
                }
            }

            // use color: paint.getColor()
            else {

                for (int y = top; y < bottom; y++) {
                    curr_row = fDevice.getAddr(0,y);

                    int i = 0;
                    int w = 0;
                    int left;

                    // for all active edges...
                    while ((i < edges.size()) && (edges[i].isValid(y))) {
                        int x = int(edge[i].eval_x(y));
                        if (w == 0) {
                            left = x;
                        }

                        w += edge[i].w;
                        if (w == 0) {
                            int right = x;

                            for (int x = left; x < right; x++) {

                                switch (optBlendMode) {
                                    case GBlendMode::kClear:
                                        curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                        break;
                                    
                                    case GBlendMode::kSrc:
                                        curr_row[x] = convertColor2Pixel(paint.getColor());
                                        break;

                                    case GBlendMode::kSrcOver:
                                        curr_row[x] = blend_kSrcOver(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOver:
                                        curr_row[x] = blend_kDstOver(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcIn:
                                        curr_row[x] = blend_kSrcIn(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstIn:
                                        curr_row[x] = blend_kDstIn(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcOut:
                                        curr_row[x] = blend_kSrcOut(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOut:
                                        curr_row[x] = blend_kDstOut(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcATop:
                                        curr_row[x] = blend_kSrcATop(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstATop:
                                        curr_row[x] = blend_kDstATop(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kXor:
                                        curr_row[x] = blend_kXor(paint.getColor(),&curr_row[x]);
                                        break;
                                }
                            
                            }
                        }

                        // check edge validity;
                        // remove edge from vector if no longer valid
                        if (edge[i].isValid(y+1)) {
                            i += 1;
                        } else {
                            edges.erase(i);
                        }
                    }

                    assert(w == 0);

                    while ((i < edge.size()) && (edge[i].isValid(y+1))) {
                        i += 1;
                    }

                    std::sort(edges.begin(),edges.begin()+i,[y](const Edge& e1, const Ege& e1) {
                        return edge_sort_x(e1, e2, y);
                    });

                }

            }
        }
    }
}
*/

/* drawMesh() */
void MyCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) {
    
    // if something is specified
    if ((colors != nullptr) || (texs != nullptr)) {
        
        int n = 0;

        // for each triangle
        for (int i = 0; i < count; i++) {

            // get p vertices
            GPoint pVerts[3] = {verts[indices[n+0]], verts[indices[n+1]], verts[indices[n+2]]};

            // create GPaint with correct blendmode
            GPaint thisPaint = paint;

            // overspecified (colors and textures)
            if ((colors != nullptr) && (texs != nullptr)) {
                // get t vertices
                GPoint tVerts[3] = {texs[indices[n+0]], texs[indices[n+1]], texs[indices[n+2]]};

                // get colors
                GColor theseColors[3] = {colors[indices[n+0]], colors[indices[n+1]], colors[indices[n+2]]};

                // set CompositeTriangle as shader
                thisPaint.setShader(std::shared_ptr<GShader>(new CompositeTriangle(paint.peekShader(),pVerts,tVerts,theseColors)));
            }

            // only colors
            else if ((colors != nullptr) && (texs == nullptr)) {
                // get colors
                GColor theseColors[3] = {colors[indices[n+0]], colors[indices[n+1]], colors[indices[n+2]]};

                // set TriangleGradient as shader
                thisPaint.setShader(std::shared_ptr<GShader>(new TriangleGradient(pVerts, theseColors)));
            }

            // only textures
            else if ((colors == nullptr) && (texs != nullptr)) {
                // get t vertices
                GPoint tVerts[3] = {texs[indices[n+0]], texs[indices[n+1]], texs[indices[n+2]]};

                // set TriangleBitmap as shader
                thisPaint.setShader(std::shared_ptr<GShader>(new TriangleBitmap(paint.peekShader(),pVerts,tVerts)));
            }

            // call drawConvexPolygon()
            drawConvexPolygon(pVerts, 3, thisPaint);

            n += 3;
        }
    }

}

/* drawQuad() */
void MyCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint& paint) {
    
    // if something is specified
    if ((colors != nullptr) || (texs != nullptr)) {

        // get u/v stops
        float step = 1 / float(level + 1);

        std::vector<float> stops;
        stops.push_back(0);
        float s = 0;
        for (int i = 0; i < level; i++) {
            stops.push_back(s += step);
        }
        stops.push_back(1);

        // define indices
        int meshIndices[6] = {0, 1, 3, 1, 2, 3};

        float v1, v2, u1, u2;
        for (int v = 0; v < level+1; v++) {
            v1 = stops[v];
            v2 = stops[v+1];

            for (int u = 0; u < level+1; u++) {
                u1 = stops[u];
                u2 = stops[u+1];

                // overspecified (colors and textures)
                if ((colors != nullptr) && (texs != nullptr)) {
                    // get vertices
                    GPoint meshVerts[4];
                    getQuadPoints(verts, meshVerts, u1, u2, v1, v2);

                    // get colors
                    GColor meshColors[4];
                    getQuadColors(colors, meshColors, u1, u2, v1, v2);

                    // get texture coordinates
                    GPoint meshTexs[4];
                    getQuadPoints(texs, meshTexs, u1, u2, v1, v2);

                    // draw
                    drawMesh(meshVerts, meshColors, meshTexs, 2, meshIndices, paint);
                }

                // only colors
                else if ((colors != nullptr) && (texs == nullptr)) {
                    // get vertices
                    GPoint meshVerts[4];
                    getQuadPoints(verts, meshVerts, u1, u2, v1, v2);

                    // get colors
                    GColor meshColors[4];
                    getQuadColors(colors, meshColors, u1, u2, v1, v2);

                    // draw
                    drawMesh(meshVerts, meshColors, nullptr, 2, meshIndices, paint);
                }

                // only textures
                else if ((colors == nullptr) && (texs != nullptr)) {
                    // get vertices
                    GPoint meshVerts[4];
                    getQuadPoints(verts, meshVerts, u1, u2, v1, v2);

                    // get texture coordinates
                    GPoint meshTexs[4];
                    getQuadPoints(texs, meshTexs, u1, u2, v1, v2);

                    // draw
                    drawMesh(meshVerts, nullptr, meshTexs, 2, meshIndices, paint);
                }

            }
        }
    }
}

/* GCreateCanvas() */
std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

/* GDrawSomething() */
std::string GDrawSomething(GCanvas* canvas, GISize dim) {

    auto yo = GCreateLinearGradient({64, 64}, {128, 128}, {1, 1, 0.5, 1}, {1, 0.75, 0.5, 1}, GTileMode::kMirror);
    auto gb = GCreateLinearGradient({32, 223}, {223, 32}, {0.5, 1, 0.5, 1}, {0.75, 0.75, 1, 1}, GTileMode::kClamp);
    auto pp = GCreateLinearGradient({128, 75}, {128, 180}, {0.75, 0.5, 1, 1}, {1, 0.5, 1, 1}, GTileMode::kClamp);

    // yellow-orange gradient background
    canvas->drawRect(GRect::XYWH(0,0,255,255), GPaint(yo));
    
    // pink-purple gradient border
    GPathBuilder b;
    b.addRect(GRect::XYWH(10,10,235,235));
    b.addRect(GRect::XYWH(25,25,205,205), GPathDirection::kCCW);
    canvas->drawPath(*b.detach(), GPaint(pp));

    // blue-green gradient diamonds
    const GPoint dL[] { {10, 128}, {60, 178}, {110, 128}, {60, 78} };
    const GPoint dR[] { {245, 128}, {195, 178}, {145, 128}, {195, 78} };
    const GPoint dT[] { {128, 245}, {178, 195}, {128, 145}, {78, 195} };
    const GPoint dB[] { {128, 10}, {178, 60}, {128, 110}, {78, 60} };

    GPathBuilder bgd;
    bgd.addPolygon(dL,4);
    bgd.addPolygon(dR,4);
    bgd.addPolygon(dT,4);
    bgd.addPolygon(dB,4);
    canvas->drawPath(*bgd.detach(), GPaint(gb));

    // center diamond
    GPoint verts[5] = {{128,88}, {168,128}, {128,168}, {88,128}, {128,128}};
    GColor colors[5] = {{1, 0.4f, 0.4f, 1}, {1, 0.75, 0.25, 1}, {1, 0.4f, 0.4f, 1}, {1, 0.75, 0.25, 1}, {1, 1, 0.4f, 1}};
    int indices[12] = {0,1,4, 0,3,4, 1,2,4, 2,3,4};
    canvas->drawMesh(verts, colors, nullptr, 4, indices, GPaint());

    return "shape gradients";
}
